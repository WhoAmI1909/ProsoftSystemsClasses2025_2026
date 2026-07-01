#include <gtest/gtest.h>
#include "data_loader.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace pf {
namespace test {

class DataLoaderTest : public ::testing::Test {
protected:
    DataLoader loader;

    void create_temp_json(const std::string& name, const std::string& content) {
        std::ofstream f(name);
        f << content;
        f.close();
    }

    void remove_temp_file(const std::string& name) {
        std::remove(name.c_str());
    }
};

TEST_F(DataLoaderTest, FileNotFound) {
    EXPECT_THROW(loader.load_from_file("nonexistent_file.json"), DataLoaderError);
}

TEST_F(DataLoaderTest, InvalidJson) {
    create_temp_json("test_invalid.json", "{not valid json");
    EXPECT_THROW(loader.load_from_file("test_invalid.json"), DataLoaderError);
    remove_temp_file("test_invalid.json");
}

TEST_F(DataLoaderTest, MissingRequiredFields) {
    create_temp_json("test_missing.json", R"({"base_voltage_kV": 110})");
    EXPECT_THROW(loader.load_from_file("test_missing.json"), DataLoaderError);
    remove_temp_file("test_missing.json");
}

TEST_F(DataLoaderTest, Load2NodeNetwork) {
    create_temp_json("test_2.json", R"({
        "base_voltage_kV": 110,
        "base_power_MVA": 100,
        "tolerance": 0.001,
        "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 50}
        ],
        "branches": [
            {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10}
        ]
    })");
    auto data = loader.load_from_file("test_2.json");
    EXPECT_EQ(data.islands.size(), 1);
    EXPECT_EQ(data.islands[0].nodes.size(), 2);
    EXPECT_EQ(data.islands[0].branches.size(), 1);
    EXPECT_EQ(data.base_voltage_kV, 110);
    EXPECT_EQ(data.base_power_MVA, 100);
    EXPECT_EQ(data.tolerance, 0.001);
    EXPECT_EQ(data.max_iterations, 100);
    remove_temp_file("test_2.json");
}

TEST_F(DataLoaderTest, Load3NodeNetwork) {
    create_temp_json("test_3.json", R"({
        "base_voltage_kV": 110,
        "base_power_MVA": 100,
        "tolerance": 0.001,
        "max_iterations": 100,
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
    auto data = loader.load_from_file("test_3.json");
    EXPECT_EQ(data.islands.size(), 1);
    EXPECT_EQ(data.islands[0].nodes.size(), 3);
    EXPECT_EQ(data.islands[0].branches.size(), 3);
    remove_temp_file("test_3.json");
}

TEST_F(DataLoaderTest, TwoIslands) {
    create_temp_json("test_islands.json", R"({
        "base_voltage_kV": 110,
        "base_power_MVA": 100,
        "tolerance": 0.001,
        "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 30},
            {"id": 3, "type": "slack", "voltage_kV": 220},
            {"id": 4, "type": "load", "power_MW": 40}
        ],
        "branches": [
            {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 5},
            {"id": 2, "from_node_id": 3, "to_node_id": 4, "resistance_ohm": 8}
        ]
    })");
    auto data = loader.load_from_file("test_islands.json");
    EXPECT_EQ(data.islands.size(), 2);
    remove_temp_file("test_islands.json");
}

TEST_F(DataLoaderTest, NoSlackNode) {
    create_temp_json("test_noslack.json", R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [{"id": 1, "type": "load", "power_MW": 50}],
        "branches": []
    })");
    EXPECT_THROW(loader.load_from_file("test_noslack.json"), DataLoaderError);
    remove_temp_file("test_noslack.json");
}

TEST_F(DataLoaderTest, DuplicateNodeId) {
    create_temp_json("test_dupid.json", R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 1, "type": "load", "power_MW": 50}
        ],
        "branches": [{"id": 1, "from_node_id": 1, "to_node_id": 1, "resistance_ohm": 10}]
    })");
    EXPECT_THROW(loader.load_from_file("test_dupid.json"), DataLoaderError);
    remove_temp_file("test_dupid.json");
}

TEST_F(DataLoaderTest, InvalidBranchReference) {
    create_temp_json("test_badref.json", R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [{"id": 1, "type": "slack", "voltage_kV": 115}],
        "branches": [{"id": 1, "from_node_id": 1, "to_node_id": 99, "resistance_ohm": 10}]
    })");
    EXPECT_THROW(loader.load_from_file("test_badref.json"), DataLoaderError);
    remove_temp_file("test_badref.json");
}

TEST_F(DataLoaderTest, NegativeResistance) {
    create_temp_json("test_negr.json", R"({
        "base_voltage_kV": 110, "base_power_MVA": 100, "tolerance": 0.001, "max_iterations": 100,
        "nodes": [
            {"id": 1, "type": "slack", "voltage_kV": 115},
            {"id": 2, "type": "load", "power_MW": 50}
        ],
        "branches": [{"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": -5}]
    })");
    EXPECT_THROW(loader.load_from_file("test_negr.json"), DataLoaderError);
    remove_temp_file("test_negr.json");
}

TEST_F(DataLoaderTest, SignedPowers) {
    create_temp_json("test_sign.json", R"({
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
    auto data = loader.load_from_file("test_sign.json");
    EXPECT_LT(data.islands[0].nodes[1].p_pu, 0.0);   // load: negative
    EXPECT_GT(data.islands[0].nodes[2].p_pu, 0.0);    // generator: positive
    remove_temp_file("test_sign.json");
}

} // namespace test
} // namespace pf
