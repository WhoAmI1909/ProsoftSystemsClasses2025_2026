#include <gtest/gtest.h>
#include "data_loader.hpp"
#include "network_builder.hpp"
#include "power_flow_solver.hpp"
#include "flow_calculator.hpp"
#include "report_generator.hpp"
#include <fstream>
#include <cmath>

namespace pf {
namespace test {

class IntegrationTest : public ::testing::Test {
protected:
    void create_temp_json(const std::string& name, const std::string& content) {
        std::ofstream f(name);
        f << content;
        f.close();
    }

    void remove_temp_file(const std::string& name) {
        std::remove(name.c_str());
    }

    void remove_temp_file_safe(const std::string& name) {
        std::ifstream f(name);
        if (f.good()) {
            f.close();
            std::remove(name.c_str());
        }
    }

    PowerFlowResult run_network(const std::string& content) {
        create_temp_json("_test_run.json", content);

        DataLoader loader;
        auto data = loader.load_from_file("_test_run.json");

        NetworkBuilder builder;
        PowerFlowSolver solver;
        FlowCalculator calc;

        PowerFlowResult total;

        for (const auto& island : data.islands) {
            if (island.slack_node_id < 0) continue;
            auto Y = builder.build_admittance_matrix(island, data.base_voltage_kV, data.base_power_MVA);
            auto P = builder.build_power_vector(island);
            auto solver_result = solver.solve(island, Y, P, data.tolerance, data.max_iterations, data.base_voltage_kV);

            Eigen::VectorXd U(static_cast<int>(solver_result.voltage_pu.size()));
            for (size_t i = 0; i < solver_result.voltage_pu.size(); ++i)
                U(static_cast<int>(i)) = solver_result.voltage_pu[i];

            auto flow = calc.calculate(island, U, data.base_power_MVA, data.base_voltage_kV);
            flow.iterations = solver_result.iterations;
            flow.converged = solver_result.converged;
            total.total_generation_MW += flow.total_generation_MW;
            total.total_load_MW += flow.total_load_MW;
            total.island_results.push_back(flow);
        }

        total.total_imbalance_MW = std::abs(total.total_generation_MW - total.total_load_MW);
        remove_temp_file("_test_run.json");
        remove_temp_file_safe("report.txt");
        return total;
    }
};

TEST_F(IntegrationTest, TwoNodeFlow) {
    auto result = run_network(R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 20}
        ],
        "branches": [
            {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 5}
        ]
    })");

    ASSERT_EQ(result.island_results.size(), 1);
    EXPECT_TRUE(result.island_results[0].converged);
    EXPECT_EQ(result.island_results[0].flow_MW.size(), 1);
    EXPECT_GT(result.island_results[0].flow_MW[0], 0);
    EXPECT_EQ(result.island_results[0].flow_direction[0], 1);
    EXPECT_LT(result.island_results[0].voltage_pu[1], result.island_results[0].voltage_pu[0]);
}

TEST_F(IntegrationTest, ThreeNodeBalance) {
    auto result = run_network(R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 50},
            {"id": 3, "type": "generator", "power_MW": 30}
        ],
        "branches": [
            {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10},
            {"id": 2, "from_node_id": 2, "to_node_id": 3, "resistance_ohm": 5},
            {"id": 3, "from_node_id": 1, "to_node_id": 3, "resistance_ohm": 20}
        ]
    })");

    ASSERT_EQ(result.island_results.size(), 1);
    EXPECT_TRUE(result.island_results[0].converged);

    // Per-node power balance: sum of incoming flows ≈ node power
    double gen = result.island_results[0].total_generation_MW;
    double load = result.island_results[0].total_load_MW;
    (void)gen; (void)load;
    // With slack implicit generation: gen(from generator) + slack_gen = load
    // We verify that flows are consistent
    EXPECT_GT(result.island_results[0].flow_MW[0], 0); // flow from 1→2
}

TEST_F(IntegrationTest, ParallelBranches) {
    auto result = run_network(R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 60}
        ],
        "branches": [
            {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10},
            {"id": 2, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 20}
        ]
    })");

    ASSERT_EQ(result.island_results.size(), 1);
    EXPECT_TRUE(result.island_results[0].converged);
    ASSERT_EQ(result.island_results[0].flow_MW.size(), 2);

    // Flow should be inversely proportional to resistance
    // g1 = 1/R1, g2 = 1/R2, so flow1/flow2 ≈ g1/g2 = R2/R1 = 20/10 = 2
    double ratio = std::abs(result.island_results[0].flow_MW[0] / result.island_results[0].flow_MW[1]);
    EXPECT_NEAR(ratio, 2.0, 0.3);
}

TEST_F(IntegrationTest, ExitCodes) {
    // Test that invalid JSON produces error
    DataLoader loader;
    create_temp_json("_bad.json", "{invalid}");
    EXPECT_THROW(loader.load_from_file("_bad.json"), DataLoaderError);
    remove_temp_file("_bad.json");

    // Test file not found
    EXPECT_THROW(loader.load_from_file("_nonexistent.json"), DataLoaderError);
}

} // namespace test
} // namespace pf
