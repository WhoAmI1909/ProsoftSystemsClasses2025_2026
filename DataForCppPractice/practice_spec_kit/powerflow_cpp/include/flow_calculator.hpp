#pragma once

#include "types.hpp"
#include <Eigen/Dense>

namespace pf {

class FlowCalculator {
public:
    IslandResult calculate(const Island& island, const Eigen::VectorXd& U,
                           double base_power_MVA, double base_voltage_kV);

private:
    double compute_g_pu(double resistance_ohm, double base_voltage_kV, double base_power_MVA) const;
};

} // namespace pf
