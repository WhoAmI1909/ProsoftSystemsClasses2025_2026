#pragma once

#include "types.hpp"
#include <Eigen/Dense>

namespace pf {

class NetworkBuilder {
public:
    Eigen::MatrixXd build_admittance_matrix(const Island& island, double base_voltage_kV, double base_power_MVA);
    Eigen::VectorXd build_power_vector(const Island& island);
};

} // namespace pf
