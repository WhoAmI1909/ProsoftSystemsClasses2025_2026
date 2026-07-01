#include "data_loader.hpp"
#include "network_builder.hpp"
#include "power_flow_solver.hpp"
#include "flow_calculator.hpp"
#include "report_generator.hpp"
#include <iostream>
#include <string>
#include <cmath>

int main(int argc, char* argv[]) {
    try {
        std::string file_path = "data/input.json";
        if (argc > 1) {
            file_path = argv[1];
        }

        std::cout << "PowerFlowCalc - Расчет установившегося режима электрической сети\n";
        std::cout << "Файл: " << file_path << "\n\n";

        pf::DataLoader loader;
        pf::NetworkData data = loader.load_from_file(file_path);

        // Warn about islands with errors and skip them
        for (const auto& island : data.islands) {
            if (!island.warnings.empty()) {
                for (const auto& w : island.warnings) {
                    std::cerr << "Предупреждение: островок " << island.island_id
                              << " пропущен — " << w << "\n";
                }
            }
        }

        pf::NetworkBuilder builder;
        pf::PowerFlowSolver solver;
        pf::FlowCalculator calculator;
        pf::ReportGenerator reporter;

        pf::PowerFlowResult total_result;

        for (const auto& island : data.islands) {
            if (island.slack_node_id < 0) continue;

            auto Y = builder.build_admittance_matrix(island, data.base_voltage_kV, data.base_power_MVA);
            auto P = builder.build_power_vector(island);

            try {
                auto solver_result = solver.solve(island, Y, P, data.tolerance, data.max_iterations, data.base_voltage_kV);

                // Convert voltage vector to Eigen for flow calculator
                Eigen::VectorXd U(static_cast<int>(solver_result.voltage_pu.size()));
                for (size_t i = 0; i < solver_result.voltage_pu.size(); ++i) {
                    U(static_cast<int>(i)) = solver_result.voltage_pu[i];
                }

                auto flow_result = calculator.calculate(island, U,
                                                         data.base_power_MVA, data.base_voltage_kV);
                flow_result.iterations = solver_result.iterations;
                flow_result.final_residual = solver_result.final_residual;
                flow_result.converged = solver_result.converged;

                total_result.total_generation_MW += flow_result.total_generation_MW;
                total_result.total_load_MW += flow_result.total_load_MW;
                total_result.island_results.push_back(flow_result);
            } catch (const pf::SolverError& e) {
                std::cerr << e.what() << std::endl;
                return 2;
            }
        }

        total_result.total_imbalance_MW = std::abs(total_result.total_generation_MW - total_result.total_load_MW);

        if (total_result.island_results.empty()) {
            std::cerr << "Ошибка: ни один островок не был рассчитан." << std::endl;
            return 1;
        }

        reporter.generate(data, total_result);
        return 0;

    } catch (const pf::DataLoaderError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Непредвиденная ошибка: " << e.what() << std::endl;
        return 3;
    }
}
