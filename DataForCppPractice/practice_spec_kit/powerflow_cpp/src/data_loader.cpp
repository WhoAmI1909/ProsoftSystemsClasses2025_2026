#include "data_loader.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <nlohmann/json.hpp>

namespace pf {

static bool file_exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

NetworkData DataLoader::load_from_file(const std::string& path) {
    if (!file_exists(path)) {
        throw DataLoaderError("Ошибка: файл '" + path + "' не найден. Проверьте путь и повторите запуск.");
    }

    std::ifstream file(path);
    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& e) {
        throw DataLoaderError("Ошибка: невалидный JSON в файле '" + path + "'. " + std::string(e.what()));
    }

    validate_global_fields(j);
    validate_nodes(j);

    NetworkData data;
    data.base_voltage_kV = j.at("base_voltage_kV").get<double>();
    data.base_power_MVA = j.at("base_power_MVA").get<double>();
    data.tolerance = j.at("tolerance").get<double>();
    data.max_iterations = j.at("max_iterations").get<int>();

    std::vector<Node> nodes;
    std::unordered_map<int, int> node_id_set;
    for (const auto& node_json : j.at("nodes")) {
        Node n;
        n.id = node_json.at("id").get<int>();
        n.name = node_json.value("name", "");
        n.type = node_json.at("type").get<std::string>();

        if (node_id_set.find(n.id) != node_id_set.end()) {
            throw DataLoaderError("Ошибка: дублирующийся ID узла " + std::to_string(n.id) + ".");
        }
        node_id_set[n.id] = 1;

        if (n.type == "slack") {
            if (!node_json.contains("voltage_kV")) {
                throw DataLoaderError("Ошибка: для slack-узла " + std::to_string(n.id) + " не указано voltage_kV.");
            }
            n.voltage_kV = node_json.at("voltage_kV").get<double>();
            if (n.voltage_kV <= 0.0) {
                throw DataLoaderError("Ошибка: voltage_kV узла " + std::to_string(n.id) + " должно быть > 0.");
            }
        } else if (n.type == "load" || n.type == "generator") {
            if (!node_json.contains("power_MW")) {
                throw DataLoaderError("Ошибка: для узла " + std::to_string(n.id) + " типа '" + n.type + "' не указано power_MW.");
            }
            n.power_MW = node_json.at("power_MW").get<double>();
            if (n.power_MW < 0.0) {
                throw DataLoaderError("Ошибка: power_MW узла " + std::to_string(n.id) + " должно быть >= 0.");
            }
        } else {
            throw DataLoaderError("Ошибка: неизвестный тип узла '" + n.type + "' для узла " + std::to_string(n.id) + ". Допустимые типы: slack, load, generator.");
        }

        nodes.push_back(n);
    }

    if (nodes.empty()) {
        throw DataLoaderError("Ошибка: список узлов пуст. Добавьте хотя бы один узел.");
    }

    validate_branches(j, node_id_set);

    std::vector<Branch> branches;
    for (const auto& branch_json : j.at("branches")) {
        Branch b;
        b.id = branch_json.at("id").get<int>();
        b.name = branch_json.value("name", "");
        b.from_node_id = branch_json.at("from_node_id").get<int>();
        b.to_node_id = branch_json.at("to_node_id").get<int>();
        b.resistance_ohm = branch_json.at("resistance_ohm").get<double>();

        if (b.resistance_ohm <= 0.0) {
            throw DataLoaderError("Ошибка: сопротивление ветви " + std::to_string(b.id) + " должно быть > 0 (значение: " + std::to_string(b.resistance_ohm) + ").");
        }

        b.g = 1.0 / b.resistance_ohm;
        branches.push_back(b);
    }

    if (branches.empty()) {
        throw DataLoaderError("Ошибка: список ветвей пуст. Добавьте хотя бы одну ветвь.");
    }

    std::cout << "[DataLoader] Файл '" << path << "' прочитан: узлов " << nodes.size() << ", ветвей " << branches.size() << std::endl;

    // Detect islands
    data.islands = detect_islands(nodes, branches);

    // Validate each island independently
    validate_islands(data.islands);

    // Assign signed powers
    assign_powers(data);

    return data;
}

void DataLoader::validate_global_fields(const nlohmann::json& j) {
    std::vector<std::string> required = {"base_voltage_kV", "base_power_MVA", "tolerance", "max_iterations", "nodes", "branches"};
    for (const auto& field : required) {
        if (!j.contains(field)) {
            throw DataLoaderError("Ошибка: отсутствует обязательное поле '" + field + "' в JSON.");
        }
    }

    if (j.at("base_voltage_kV").get<double>() <= 0.0) {
        throw DataLoaderError("Ошибка: base_voltage_kV должно быть > 0.");
    }
    if (j.at("base_power_MVA").get<double>() <= 0.0) {
        throw DataLoaderError("Ошибка: base_power_MVA должно быть > 0.");
    }
    if (j.at("tolerance").get<double>() <= 0.0) {
        throw DataLoaderError("Ошибка: tolerance должно быть > 0.");
    }
    if (j.at("max_iterations").get<int>() <= 0) {
        throw DataLoaderError("Ошибка: max_iterations должно быть > 0.");
    }
}

void DataLoader::validate_nodes(const nlohmann::json& j) {
    if (!j.contains("nodes") || !j.at("nodes").is_array()) {
        throw DataLoaderError("Ошибка: поле 'nodes' отсутствует или не является массивом.");
    }
    for (const auto& node : j.at("nodes")) {
        std::vector<std::string> required = {"id", "type"};
        for (const auto& field : required) {
            if (!node.contains(field)) {
                throw DataLoaderError("Ошибка: у узла отсутствует обязательное поле '" + field + "'.");
            }
        }
    }
}

void DataLoader::validate_branches(const nlohmann::json& j, const std::unordered_map<int, int>& node_id_set) {
    if (!j.contains("branches") || !j.at("branches").is_array()) {
        throw DataLoaderError("Ошибка: поле 'branches' отсутствует или не является массивом.");
    }
    std::set<int> branch_ids;
    for (const auto& branch : j.at("branches")) {
        std::vector<std::string> required = {"id", "from_node_id", "to_node_id", "resistance_ohm"};
        for (const auto& field : required) {
            if (!branch.contains(field)) {
                throw DataLoaderError("Ошибка: у ветви отсутствует обязательное поле '" + field + "'.");
            }
        }
        int bid = branch.at("id").get<int>();
        if (branch_ids.find(bid) != branch_ids.end()) {
            throw DataLoaderError("Ошибка: дублирующийся ID ветви " + std::to_string(bid) + ".");
        }
        branch_ids.insert(bid);

        int from = branch.at("from_node_id").get<int>();
        int to = branch.at("to_node_id").get<int>();
        if (node_id_set.find(from) == node_id_set.end()) {
            throw DataLoaderError("Ошибка: ветвь " + std::to_string(bid) + " ссылается на несуществующий узел from_node_id=" + std::to_string(from) + ".");
        }
        if (node_id_set.find(to) == node_id_set.end()) {
            throw DataLoaderError("Ошибка: ветвь " + std::to_string(bid) + " ссылается на несуществующий узел to_node_id=" + std::to_string(to) + ".");
        }
    }
}

std::vector<Island> DataLoader::detect_islands(const std::vector<Node>& nodes, const std::vector<Branch>& branches) {
    // Build adjacency list
    std::unordered_map<int, std::vector<int>> adj;
    std::unordered_map<int, int> node_lookup; // global id -> index in nodes vector
    for (size_t i = 0; i < nodes.size(); ++i) {
        node_lookup[nodes[i].id] = static_cast<int>(i);
        adj[nodes[i].id] = {};
    }
    for (const auto& b : branches) {
        adj[b.from_node_id].push_back(b.to_node_id);
        adj[b.to_node_id].push_back(b.from_node_id);
    }

    std::set<int> visited;
    std::vector<Island> islands;
    int island_counter = 0;

    for (const auto& node : nodes) {
        if (visited.find(node.id) != visited.end()) continue;

        // BFS
        std::set<int> component_nodes;
        std::queue<int> q;
        q.push(node.id);
        visited.insert(node.id);
        component_nodes.insert(node.id);

        while (!q.empty()) {
            int current = q.front(); q.pop();
            for (int neighbor : adj[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    component_nodes.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }

        Island island;
        island.island_id = island_counter++;

        // Collect nodes
        int local_idx = 0;
        int slack_count = 0;
        for (int gid : component_nodes) {
            const auto& n = nodes[static_cast<size_t>(node_lookup[gid])];
            island.nodes.push_back(n);
            island.global_to_local[gid] = local_idx++;
            if (n.type == "slack") {
                island.slack_node_id = n.id;
                ++slack_count;
            }
        }

        // Collect branches where both endpoints are in this component
        for (const auto& b : branches) {
            if (component_nodes.find(b.from_node_id) != component_nodes.end() &&
                component_nodes.find(b.to_node_id) != component_nodes.end()) {
                island.branches.push_back(b);
            }
        }

        islands.push_back(island);
    }

    std::cout << "[DataLoader] Выделено островков: " << islands.size() << std::endl;
    return islands;
}

void DataLoader::validate_islands(std::vector<Island>& islands) {
    bool any_valid = false;

    for (auto& island : islands) {
        bool has_error = false;

        // Check slack count
        int slack_count = 0;
        for (const auto& n : island.nodes) {
            if (n.type == "slack") ++slack_count;
        }

        if (slack_count == 0) {
            island.warnings.push_back("нет узла типа 'slack'");
            has_error = true;
        } else if (slack_count > 1) {
            island.warnings.push_back("более одного узла типа 'slack' (найдено " + std::to_string(slack_count) + ")");
            has_error = true;
        }

        if (!has_error) {
            any_valid = true;
        }
    }

    if (!any_valid) {
        std::string msg = "Ошибка: ни один островок не прошел валидацию.\n";
        for (const auto& island : islands) {
            for (const auto& w : island.warnings) {
                msg += "Островок " + std::to_string(island.island_id) + ": " + w + "\n";
            }
        }
        msg += "Исправьте ошибки и повторите запуск.";
        throw DataLoaderError(msg);
    }
}

void DataLoader::assign_powers(NetworkData& data) {
    for (auto& island : data.islands) {
        for (auto& node : island.nodes) {
            if (node.type == "generator") {
                node.p_pu = node.power_MW / data.base_power_MVA;
            } else if (node.type == "load") {
                node.p_pu = -node.power_MW / data.base_power_MVA;
            } else {
                node.p_pu = 0.0; // slack
            }
        }
    }
}

} // namespace pf
