#include <gtest/gtest.h>
#include "report_generator.hpp"
#include "utils.hpp"
#include <sstream>

namespace pf {
namespace test {

TEST(UtilsTest, FormatNumber) {
    EXPECT_EQ(utils::format_number(1.2345, 2), "1.23");
    EXPECT_EQ(utils::format_number(0.0, 3), "0.000");
    EXPECT_EQ(utils::format_number(1.0, 1), "1.0");
}

TEST(UtilsTest, PadRight) {
    EXPECT_EQ(utils::pad_right("abc", 6), "abc   ");
    EXPECT_EQ(utils::pad_right("abcdef", 3), "abcdef");
}

TEST(UtilsTest, PadLeft) {
    EXPECT_EQ(utils::pad_left("42", 5), "   42");
}

TEST(UtilsTest, CenterText) {
    std::string c = utils::center_text("ID", 6);
    EXPECT_EQ(c.length(), 6);
    EXPECT_NE(c.find("ID"), std::string::npos);
}

TEST(UtilsTest, RepeatChar) {
    EXPECT_EQ(utils::repeat_char('-', 5), "-----");
    EXPECT_EQ(utils::repeat_char('a', 0), "");
}

TEST(ReportGeneratorTest, ReportDoesNotThrow) {
    NetworkData data;
    data.base_voltage_kV = 110;
    PowerFlowResult result;
    IslandResult ir;
    ir.island_id = 0;
    ir.voltage_pu = {1.0, 0.95};
    ir.voltage_kV = {110.0, 104.5};
    ir.flow_MW = {50.0};
    ir.flow_direction = {1};
    ir.total_generation_MW = 0;
    ir.total_load_MW = 50;
    ir.imbalance_MW = 50;
    ir.converged = true;
    ir.iterations = 5;
    result.island_results.push_back(ir);
    result.total_generation_MW = 0;
    result.total_load_MW = 50;
    result.total_imbalance_MW = 50;

    Island island;
    island.island_id = 0;
    island.slack_node_id = 1;
    island.nodes = {{1, "S", "slack", 115.0}, {2, "L", "load", 0.0}};
    island.branches = {{1, "Line", 1, 2, 10.0}};
    data.islands.push_back(island);

    ReportGenerator gen;
    EXPECT_NO_THROW(gen.generate(data, result));
}

} // namespace test
} // namespace pf
