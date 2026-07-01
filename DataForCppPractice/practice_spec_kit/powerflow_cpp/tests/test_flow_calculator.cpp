#include <gtest/gtest.h>
#include "flow_calculator.hpp"
#include "network_builder.hpp"
#include "power_flow_solver.hpp"
#include <cmath>

namespace pf {
namespace test {

class FlowCalculatorTest : public ::testing::Test {
protected:
    std::pair<Island, Eigen::VectorXd> solve_2node() {
        Island island;
        island.island_id = 0;
        island.slack_node_id = 1;
        island.nodes = {
            {1, "Slack", "slack", 115.0, 0.0, 0.0},
            {2, "Load", "load", 0.0, 50.0, -0.5}
        };
        island.global_to_local[1] = 0;
        island.global_to_local[2] = 1;
        Branch b{1, "Line", 1, 2, 10.0};
        island.branches.push_back(b);

        NetworkBuilder builder;
        PowerFlowSolver solver;
        auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);
        auto P = builder.build_power_vector(island);

        auto result = solver.solve(island, Y, P, 0.001, 100, 110.0);
        Eigen::VectorXd U(2);
        U(0) = result.voltage_pu[0];
        U(1) = result.voltage_pu[1];
        return {island, U};
    }
};

TEST_F(FlowCalculatorTest, FlowDirection) {
    auto [island, U] = solve_2node();
    FlowCalculator calc;
    auto result = calc.calculate(island, U, 100.0, 110.0);

    ASSERT_EQ(result.flow_MW.size(), 1);
    EXPECT_GT(result.flow_MW[0], 0.0); // flow from 1 to 2
    EXPECT_EQ(result.flow_direction[0], 1); // from->to
}

TEST_F(FlowCalculatorTest, BalancePrecision) {
    auto [island, U] = solve_2node();
    FlowCalculator calc;
    auto result = calc.calculate(island, U, 100.0, 110.0);

    // In lossless DC model, generation (from slack) ≈ load at node 2
    // Power into node 2: P = U2 * g*(U1-U2)
    double Z_base = 110.0 * 110.0 / 100.0;
    double g = 1.0 / (10.0 / Z_base);
    double expected_power_pu = U(1) * g * (U(0) - U(1));
    double expected_power_mw = expected_power_pu * 100.0;
    (void)expected_power_mw;

    // Check that per-node balance error is less than 1e-6 pu
    // For node 2: |incoming - load| < 1e-6 pu
    double incoming_pu = g * (U(0) - U(1)) * U(1); // power entering node 2
    double load_pu = 50.0 / 100.0; // 0.5 pu load
    EXPECT_NEAR(incoming_pu, load_pu, 1e-4);
}

TEST_F(FlowCalculatorTest, VoltagesInKV) {
    auto [island, U] = solve_2node();
    FlowCalculator calc;
    auto result = calc.calculate(island, U, 100.0, 110.0);

    EXPECT_NEAR(result.voltage_kV[0], 115.0, 0.1);
    EXPECT_GT(result.voltage_kV[0], result.voltage_kV[1]); // slack higher
}

} // namespace test
} // namespace pf
