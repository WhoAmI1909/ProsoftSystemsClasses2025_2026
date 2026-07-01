#pragma once

#include "types.hpp"
#include <Eigen/Dense>

namespace pf {

class SolverError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class PowerFlowSolver {
public:
    IslandResult solve(const Island& island, const Eigen::MatrixXd& Y,
                       const Eigen::VectorXd& P, double tolerance, int max_iterations,
                       double base_voltage_kV);
};

} // namespace pf
