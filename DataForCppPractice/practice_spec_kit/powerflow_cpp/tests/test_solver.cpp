#include <gtest/gtest.h>
#include "power_flow_solver.hpp"
#include "network_builder.hpp"
#include <cmath>

namespace pf {
namespace test {

class PowerFlowSolverTest : public ::testing::Test {
protected:
    Island make_2node_island() {
        Island island;
        island.island_id = 0;
        island.slack_node_id = 1;
        island.nodes = {
            {1, "Slack", "slack", 115.0, 0.0, 0.0},
            {2, "Load", "load", 0.0, 50.0, -0.5}
        };
        island.global_to_local[1] = 0;
        island.global_to_local[2] = 1;
        Branch b;
        b.id = 1; b.from_node_id = 1; b.to_node_id = 2; b.resistance_ohm = 10.0;
        island.branches.push_back(b);
        return island;
    }

    Island make_3node_island() {
        Island island;
        island.island_id = 0;
        island.slack_node_id = 1;
        island.nodes = {
            {1, "Slack", "slack", 115.0, 0.0, 0.0},
            {2, "Load", "load", 0.0, 50.0, -0.5},
            {3, "Gen", "generator", 0.0, 30.0, 0.3}
        };
        island.global_to_local[1] = 0;
        island.global_to_local[2] = 1;
        island.global_to_local[3] = 2;
        Branch b1{1, "", 1, 2, 10.0};
        Branch b2{2, "", 2, 3, 5.0};
        Branch b3{3, "", 1, 3, 20.0};
        island.branches = {b1, b2, b3};
        return island;
    }
};

TEST_F(PowerFlowSolverTest, Solve2Node) {
    auto island = make_2node_island();
    NetworkBuilder builder;
    auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);
    auto P = builder.build_power_vector(island);

    PowerFlowSolver solver;
    auto result = solver.solve(island, Y, P, 0.001, 100, 110.0);
    EXPECT_TRUE(result.converged);
    EXPECT_LT(result.iterations, 50);

    double U_slack_pu = 115.0 / 110.0; // 1.04545
    EXPECT_NEAR(result.voltage_pu[0], U_slack_pu, 0.001);
    EXPECT_LT(result.voltage_pu[1], U_slack_pu); // load node voltage lower
    EXPECT_GT(result.voltage_pu[1], 0.9);
}

TEST_F(PowerFlowSolverTest, Solve3Node) {
    auto island = make_3node_island();
    NetworkBuilder builder;
    auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);
    auto P = builder.build_power_vector(island);

    PowerFlowSolver solver;
    auto result = solver.solve(island, Y, P, 0.001, 100, 110.0);
    EXPECT_TRUE(result.converged);
    EXPECT_LT(result.iterations, 50);

    double U_slack_pu = 115.0 / 110.0;
    EXPECT_NEAR(result.voltage_pu[0], U_slack_pu, 0.001);
    EXPECT_GT(result.voltage_pu[2], 0.9); // generator node
}

TEST_F(PowerFlowSolverTest, NonConvergence) {
    Island island;
    island.island_id = 0;
    island.slack_node_id = 1;
    island.nodes = {
        {1, "Slack", "slack", 115.0, 0.0, 0.0},
        {2, "Load", "load", 0.0, 1000.0, -10.0} // very large load
    };
    island.global_to_local[1] = 0;
    island.global_to_local[2] = 1;
    Branch b;
    b.id = 1; b.from_node_id = 1; b.to_node_id = 2; b.resistance_ohm = 100.0;
    island.branches.push_back(b);

    NetworkBuilder builder;
    auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);
    auto P = builder.build_power_vector(island);

    PowerFlowSolver solver;
    EXPECT_THROW(solver.solve(island, Y, P, 0.001, 3, 110.0), SolverError);
}

} // namespace test
} // namespace pf
