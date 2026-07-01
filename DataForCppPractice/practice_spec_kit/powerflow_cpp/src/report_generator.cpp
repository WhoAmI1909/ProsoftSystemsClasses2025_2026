#include "report_generator.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace pf {

static std::string separator(char L, char M, char R, const std::vector<int>& Ws) {
    std::ostringstream ss;
    ss << L;
    for (size_t i = 0; i < Ws.size(); ++i) {
        if (i > 0) ss << M;
        ss << utils::repeat_char('-', Ws[i]);
    }
    ss << R;
    return ss.str();
}

void ReportGenerator::generate(const NetworkData& data, const PowerFlowResult& result) {
    std::ostringstream report;

    for (const auto& island : data.islands) {
        if (island.slack_node_id < 0) continue;

        bool found = false;
        for (const auto& r : result.island_results) {
            if (r.island_id == island.island_id) {
                report << "\n=== Островок " << island.island_id << " ===\n\n";
                print_voltage_table_to_stream(report, island, r, data.base_voltage_kV);
                report << "\n";
                print_flow_table_to_stream(report, island, r);
                report << "\n";
                print_balance_table_to_stream(report, island, r);
                found = true;
                break;
            }
        }
        if (!found) {
            report << "\n=== Островок " << island.island_id << " (ПРОПУЩЕН) ===\n";
            for (const auto& w : island.warnings) {
                report << "  " << w << "\n";
            }
        }
    }

    print_summary_to_stream(report, result);

    std::cout << report.str();

    std::ofstream out("report.txt");
    if (out.is_open()) {
        out << report.str();
        out.close();
        std::cout << "\n[ReportGenerator] Отчет сохранен в report.txt" << std::endl;
    }
}

void ReportGenerator::print_voltage_table_to_stream(std::ostream& os, const Island& island,
                                                     const IslandResult& result, double /*base_voltage_kV*/) {
    std::vector<int> W = {6, 20, 10, 10};
    int total = 0; for (int w : W) total += w; total += static_cast<int>(W.size()) - 1;

    os << "Таблица напряжений в узлах (островок " << island.island_id << "):\n";
    os << separator('+', '+', '+', W) << "\n";
    os << "|" << utils::center_text("ID", W[0])
       << "|" << utils::center_text("Название", W[1])
       << "|" << utils::center_text("U, о.е.", W[2])
       << "|" << utils::center_text("U, кВ", W[3]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";

    for (size_t i = 0; i < island.nodes.size(); ++i) {
        const auto& n = island.nodes[i];
        double u_pu = result.voltage_pu[i];
        double u_kv = result.voltage_kV[i];
        std::string name = n.name.empty() ? "Узел " + std::to_string(n.id) : n.name;
        os << "|" << utils::center_text(std::to_string(n.id), W[0])
           << "|" << utils::pad_right(name, W[1])
           << "|" << utils::pad_left(utils::format_number(u_pu, 4), W[2])
           << "|" << utils::pad_left(utils::format_number(u_kv, 2), W[3]) << "|\n";
    }
    os << separator('+', '+', '+', W) << "\n";
}

void ReportGenerator::print_flow_table_to_stream(std::ostream& os, const Island& island,
                                                  const IslandResult& result) {
    std::vector<int> W = {6, 20, 6, 6, 10, 10};
    int total = 0; for (int w : W) total += w; total += static_cast<int>(W.size()) - 1;

    os << "Таблица потоков мощности по ветвям (островок " << island.island_id << "):\n";
    os << separator('+', '+', '+', W) << "\n";
    os << "|" << utils::center_text("ID", W[0])
       << "|" << utils::center_text("Название", W[1])
       << "|" << utils::center_text("От", W[2])
       << "|" << utils::center_text("До", W[3])
       << "|" << utils::center_text("P, МВт", W[4])
       << "|" << utils::center_text("Напр-е", W[5]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";

    for (size_t k = 0; k < island.branches.size(); ++k) {
        const auto& b = island.branches[k];
        double flow = result.flow_MW[k];
        int dir = result.flow_direction[k];
        std::string name = b.name.empty() ? "Ветвь " + std::to_string(b.id) : b.name;
        std::string direction = (dir >= 0)
            ? (std::to_string(b.from_node_id) + "\xe2\x86\x92" + std::to_string(b.to_node_id))
            : (std::to_string(b.to_node_id) + "\xe2\x86\x92" + std::to_string(b.from_node_id));
        os << "|" << utils::center_text(std::to_string(b.id), W[0])
           << "|" << utils::pad_right(name, W[1])
           << "|" << utils::center_text(std::to_string(b.from_node_id), W[2])
           << "|" << utils::center_text(std::to_string(b.to_node_id), W[3])
           << "|" << utils::pad_left(utils::format_number(std::abs(flow), 3), W[4])
           << "|" << utils::center_text(direction, W[5]) << "|\n";
    }
    os << separator('+', '+', '+', W) << "\n";
}

void ReportGenerator::print_balance_table_to_stream(std::ostream& os, const Island& island,
                                                     const IslandResult& result) {
    std::vector<int> W = {22, 20};
    int total = 0; for (int w : W) total += w; total += static_cast<int>(W.size()) - 1;

    double imbalance_pct = 0.0;
    double max_p = std::max(result.total_generation_MW, result.total_load_MW);
    if (max_p > 0.0) {
        imbalance_pct = (result.imbalance_MW / max_p) * 100.0;
    }

    os << "Сводка баланса мощностей (островок " << island.island_id << "):\n";
    os << separator('+', '+', '+', W) << "\n";
    os << "|" << utils::pad_right("Параметр", W[0])
       << "|" << utils::pad_left("Значение", W[1]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";

    os << "|" << utils::pad_right("Суммарная генерация", W[0])
       << "|" << utils::pad_left(utils::format_number(result.total_generation_MW, 3) + " МВт", W[1]) << "|\n";
    os << "|" << utils::pad_right("Суммарная нагрузка", W[0])
       << "|" << utils::pad_left(utils::format_number(result.total_load_MW, 3) + " МВт", W[1]) << "|\n";
    os << "|" << utils::pad_right("Невязка", W[0])
       << "|" << utils::pad_left(utils::format_number(result.imbalance_MW, 3) + " МВт (" +
                                 utils::format_number(imbalance_pct, 1) + "%)", W[1]) << "|\n";
    os << "|" << utils::pad_right("Статус сходимости", W[0])
       << "|" << utils::pad_left(result.converged ? "Сошлось" : "Не сошлось", W[1]) << "|\n";
    os << "|" << utils::pad_right("Число итераций", W[0])
       << "|" << utils::pad_left(std::to_string(result.iterations), W[1]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";
}

void ReportGenerator::print_summary_to_stream(std::ostream& os, const PowerFlowResult& result) {
    std::vector<int> W = {22, 20};
    int total = 0; for (int w : W) total += w; total += static_cast<int>(W.size()) - 1;

    os << "\n=== Сводка по всем островкам ===\n\n";
    os << separator('+', '+', '+', W) << "\n";
    os << "|" << utils::pad_right("Параметр", W[0])
       << "|" << utils::pad_left("Значение", W[1]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";

    os << "|" << utils::pad_right("Суммарная генерация", W[0])
       << "|" << utils::pad_left(utils::format_number(result.total_generation_MW, 3) + " МВт", W[1]) << "|\n";
    os << "|" << utils::pad_right("Суммарная нагрузка", W[0])
       << "|" << utils::pad_left(utils::format_number(result.total_load_MW, 3) + " МВт", W[1]) << "|\n";
    double imbalance_pct = 0.0;
    double max_p = std::max(result.total_generation_MW, result.total_load_MW);
    if (max_p > 0.0) {
        imbalance_pct = (result.total_imbalance_MW / max_p) * 100.0;
    }
    os << "|" << utils::pad_right("Суммарная невязка", W[0])
       << "|" << utils::pad_left(utils::format_number(result.total_imbalance_MW, 3) + " МВт (" +
                                 utils::format_number(imbalance_pct, 1) + "%)", W[1]) << "|\n";
    os << "|" << utils::pad_right("Число островков", W[0])
       << "|" << utils::pad_left(std::to_string(result.island_results.size()), W[1]) << "|\n";
    os << separator('+', '+', '+', W) << "\n";
}

} // namespace pf
