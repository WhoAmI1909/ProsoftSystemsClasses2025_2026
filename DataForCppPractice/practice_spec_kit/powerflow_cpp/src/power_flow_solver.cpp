#include "power_flow_solver.hpp"
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace pf {

IslandResult PowerFlowSolver::solve(const Island& island, const Eigen::MatrixXd& Y,
                                     const Eigen::VectorXd& P, double tolerance, int max_iterations,
                                     double base_voltage_kV) {
    int M = static_cast<int>(island.nodes.size());
    int slack_local = island.global_to_local.at(island.slack_node_id);
    double slack_voltage_pu = island.nodes[static_cast<size_t>(slack_local)].voltage_kV / base_voltage_kV;

    Eigen::VectorXd U = Eigen::VectorXd::Ones(M); // initial guess U=1.0 pu
    U(slack_local) = slack_voltage_pu; // set slack to actual per-unit voltage

    IslandResult result;
    result.island_id = island.island_id;

    for (int iter = 0; iter < max_iterations; ++iter) {
        Eigen::VectorXd I(M);
        for (int i = 0; i < M; ++i) {
            if (i == slack_local) {
                I(i) = 0.0;
            } else {
                double u = U(i);
                if (std::abs(u) < 1e-12) u = 1.0;
                I(i) = -P(i) / u;
            }
        }

        Eigen::VectorXd U_new = U;
        double max_diff = 0.0;

        for (int i = 0; i < M; ++i) {
            if (i == slack_local) {
                U_new(i) = slack_voltage_pu; // slack is fixed
                continue;
            }

            double sum = 0.0;
            for (int j = 0; j < M; ++j) {
                if (j != i) {
                    sum += Y(i, j) * U_new(j);
                }
            }

            double y_ii = Y(i, i);
            if (std::abs(y_ii) < 1e-12) {
                throw SolverError("Ошибка: нулевой диагональный элемент Y(" +
                                  std::to_string(i) + "," + std::to_string(i) +
                                  ") в островке " + std::to_string(island.island_id) +
                                  ". Возможно, узел изолирован.");
            }

            double u_new = (I(i) - sum) / y_ii;
            double diff = std::abs(u_new - U(i));
            if (diff > max_diff) max_diff = diff;
            U_new(i) = u_new;
        }

        U = U_new;

        if (max_diff < tolerance) {
            result.converged = true;
            result.iterations = iter + 1;
            result.final_residual = max_diff;

            std::cout << "[PowerFlowSolver] Островок " << island.island_id
                      << ": сошлось за " << (iter + 1) << " итераций, невязка="
                      << max_diff << std::endl;

            // Store voltages
            for (int i = 0; i < M; ++i) {
                result.voltage_pu.push_back(U(i));
            }
            return result;
        }
    }

    // Not converged
    result.converged = false;
    result.iterations = max_iterations;
    for (int i = 0; i < M; ++i) {
        result.voltage_pu.push_back(U(i));
    }

    // Compute final residual
        double final_res = 0.0;
    for (int i = 0; i < M; ++i) {
        if (i == slack_local) continue;
        double u = U(i);
        if (std::abs(u) < 1e-12) u = 1.0;
        double I_i = -P(i) / u;
        double sum = 0.0;
        for (int j = 0; j < M; ++j) {
            if (j != i) sum += Y(i, j) * U(j);
        }
        double u_new = (I_i - sum) / Y(i, i);
        double diff = std::abs(u_new - U(i));
        if (diff > final_res) final_res = diff;
    }
    result.final_residual = final_res;

    std::cerr << "[PowerFlowSolver] Островок " << island.island_id
              << ": НЕ сошлось за " << max_iterations << " итераций, "
              << "текущая невязка=" << final_res << std::endl;

    throw SolverError("Ошибка: в островке " + std::to_string(island.island_id) +
                      " решение не сошлось за " + std::to_string(max_iterations) +
                      " итераций (финальная невязка: " + std::to_string(final_res) +
                      "). Увеличьте max_iterations или проверьте корректность данных.");
}

} // namespace pf
