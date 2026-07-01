#include <gtest/gtest.h>
#include "network_builder.hpp"
#include <cmath>

namespace pf {
namespace test {

class NetworkBuilderTest : public ::testing::Test {
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

TEST_F(NetworkBuilderTest, YMatrix2Node) {
    auto island = make_2node_island();
    NetworkBuilder builder;
    auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);

    double Z_base = 110.0 * 110.0 / 100.0; // 121 Ohm
    double R_pu = 10.0 / Z_base;            // 0.0826
    double g = 1.0 / R_pu;                  // 12.1

    EXPECT_NEAR(Y(0, 0), -g, 1e-6);
    EXPECT_NEAR(Y(1, 1), -g, 1e-6);
    EXPECT_NEAR(Y(0, 1),  g, 1e-6);
    EXPECT_NEAR(Y(1, 0),  g, 1e-6);
}

TEST_F(NetworkBuilderTest, YMatrix3Node) {
    auto island = make_3node_island();
    NetworkBuilder builder;
    auto Y = builder.build_admittance_matrix(island, 110.0, 100.0);

    double Z_base = 110.0 * 110.0 / 100.0;
    double g12 = 1.0 / (10.0 / Z_base);
    double g23 = 1.0 / (5.0 / Z_base);
    double g13 = 1.0 / (20.0 / Z_base);

    EXPECT_NEAR(Y(0, 0), -(g12 + g13), 1e-6);
    EXPECT_NEAR(Y(1, 1), -(g12 + g23), 1e-6);
    EXPECT_NEAR(Y(2, 2), -(g23 + g13), 1e-6);
    EXPECT_NEAR(Y(0, 1), g12, 1e-6);
    EXPECT_NEAR(Y(0, 2), g13, 1e-6);
    EXPECT_NEAR(Y(1, 2), g23, 1e-6);
}

TEST_F(NetworkBuilderTest, PowerVector) {
    auto island = make_3node_island();
    NetworkBuilder builder;
    auto P = builder.build_power_vector(island);

    EXPECT_NEAR(P(0), 0.0, 1e-12);   // slack
    EXPECT_LT(P(1), 0.0);             // load: negative
    EXPECT_GT(P(2), 0.0);             // generator: positive
}

} // namespace test
} // namespace pf
