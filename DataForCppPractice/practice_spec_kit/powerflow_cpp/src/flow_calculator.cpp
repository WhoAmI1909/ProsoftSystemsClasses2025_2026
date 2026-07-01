#include "flow_calculator.hpp"
#include <iostream>
#include <cmath>

namespace pf {

double FlowCalculator::compute_g_pu(double resistance_ohm, double base_voltage_kV, double base_power_MVA) const {
    double Z_base = (base_voltage_kV * base_voltage_kV) / base_power_MVA;
    double R_pu = resistance_ohm / Z_base;
    return 1.0 / R_pu;
}

IslandResult FlowCalculator::calculate(const Island& island, const Eigen::VectorXd& U,
                                        double base_power_MVA, double base_voltage_kV) {
    IslandResult result;
    result.island_id = island.island_id;
    result.voltage_pu.assign(U.data(), U.data() + U.size());
    result.converged = true;

    for (int i = 0; i < static_cast<int>(island.nodes.size()); ++i) {
        result.voltage_kV.push_back(U(i) * base_voltage_kV);
    }

    double total_gen = 0.0;
    double total_load = 0.0;

    for (const auto& n : island.nodes) {
        if (n.type == "generator") {
            total_gen += n.power_MW;
        } else if (n.type == "load") {
            total_load += n.power_MW;
        }
    }

    for (const auto& branch : island.branches) {
        int i = island.global_to_local.at(branch.from_node_id);
        int j = island.global_to_local.at(branch.to_node_id);

        double g_pu = compute_g_pu(branch.resistance_ohm, base_voltage_kV, base_power_MVA);

        double du = U(i) - U(j);
        double I_ij = du * g_pu;
        double P_ij = U(i) * I_ij;

        double flow_mw = P_ij * base_power_MVA;
        result.flow_MW.push_back(flow_mw);
        result.flow_direction.push_back(flow_mw >= 0.0 ? 1 : -1);
    }

    result.total_generation_MW = total_gen;
    result.total_load_MW = total_load;
    // In lossless DC model, slack compensates the difference
    result.imbalance_MW = std::abs(total_gen - total_load);

    std::cout << "[FlowCalculator] Островок " << island.island_id
              << ": рассчитано " << island.branches.size() << " ветвей"
              << ", генерация=" << total_gen << " МВт, нагрузка=" << total_load << " МВт"
              << std::endl;

    return result;
}

} // namespace pf
