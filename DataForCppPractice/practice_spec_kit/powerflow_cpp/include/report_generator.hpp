#pragma once

#include "types.hpp"
#include <iostream>

namespace pf {

class ReportGenerator {
public:
    void generate(const NetworkData& data, const PowerFlowResult& result);

private:
    void print_voltage_table_to_stream(std::ostream& os, const Island& island,
                                        const IslandResult& result, double base_voltage_kV);
    void print_flow_table_to_stream(std::ostream& os, const Island& island,
                                     const IslandResult& result);
    void print_balance_table_to_stream(std::ostream& os, const Island& island,
                                        const IslandResult& result);
    void print_summary_to_stream(std::ostream& os, const PowerFlowResult& result);
};

} // namespace pf
