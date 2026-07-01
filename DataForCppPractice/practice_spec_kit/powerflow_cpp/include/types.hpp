#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

namespace pf {

struct Node {
    int id = 0;
    std::string name;
    std::string type;        // "slack", "load", "generator"
    double voltage_kV = 0.0; // only for slack
    double power_MW = 0.0;   // for load/generator (always positive in JSON)

    double p_pu = 0.0; // signed power in per unit (gen=+, load=-)
};

struct Branch {
    int id = 0;
    std::string name;
    int from_node_id = 0;
    int to_node_id = 0;
    double resistance_ohm = 0.0;

    double g = 0.0; // conductance = 1/R
};

struct Island {
    int island_id = 0;
    std::vector<Node> nodes;
    std::vector<Branch> branches;
    int slack_node_id = -1; // -1 if none
    std::unordered_map<int, int> global_to_local; // global node id -> local index (0..M-1)
    std::vector<std::string> warnings;            // validation warnings for this island
};

struct NetworkData {
    double base_voltage_kV = 0.0;
    double base_power_MVA = 0.0;
    double tolerance = 0.0;
    int max_iterations = 0;
    std::vector<Island> islands;
};

struct IslandResult {
    int island_id = 0;
    std::vector<double> voltage_pu;
    std::vector<double> voltage_kV;
    std::vector<double> flow_MW;
    std::vector<int> flow_direction; // +1 = from->to, -1 = to->from
    double total_generation_MW = 0.0;
    double total_load_MW = 0.0;
    double imbalance_MW = 0.0;
    bool converged = false;
    int iterations = 0;
    double final_residual = 0.0;
};

struct PowerFlowResult {
    std::vector<IslandResult> island_results;
    double total_generation_MW = 0.0;
    double total_load_MW = 0.0;
    double total_imbalance_MW = 0.0;
};

} // namespace pf
