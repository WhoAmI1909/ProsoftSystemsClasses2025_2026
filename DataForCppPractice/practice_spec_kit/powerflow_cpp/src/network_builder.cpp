#include "network_builder.hpp"
#include <iostream>
#include <stdexcept>

namespace pf {

Eigen::MatrixXd NetworkBuilder::build_admittance_matrix(const Island& island,
                                                         double base_voltage_kV, double base_power_MVA) {
    int M = static_cast<int>(island.nodes.size());
    Eigen::MatrixXd Y = Eigen::MatrixXd::Zero(M, M);

    double Z_base = (base_voltage_kV * base_voltage_kV) / base_power_MVA;

    for (const auto& branch : island.branches) {
        int i = island.global_to_local.at(branch.from_node_id);
        int j = island.global_to_local.at(branch.to_node_id);

        double R_pu = branch.resistance_ohm / Z_base;
        double g = 1.0 / R_pu;

        Y(i, i) -= g;
        Y(j, j) -= g;
        Y(i, j) += g;
        Y(j, i) += g;
    }

    std::cout << "[NetworkBuilder] Островок " << island.island_id
              << ": матрица Y " << M << "x" << M << " построена" << std::endl;

    return Y;
}

Eigen::VectorXd NetworkBuilder::build_power_vector(const Island& island) {
    int M = static_cast<int>(island.nodes.size());
    Eigen::VectorXd P(M);
    for (int i = 0; i < M; ++i) {
        P(i) = island.nodes[static_cast<size_t>(i)].p_pu;
    }
    return P;
}

} // namespace pf
