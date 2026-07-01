#pragma once

#include "types.hpp"
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace pf {

class DataLoaderError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class DataLoader {
public:
    NetworkData load_from_file(const std::string& path);

private:
    void validate_global_fields(const nlohmann::json& j);
    void validate_nodes(const nlohmann::json& j);
    void validate_branches(const nlohmann::json& j, const std::unordered_map<int, int>& node_id_set);
    std::vector<Island> detect_islands(const std::vector<Node>& nodes, const std::vector<Branch>& branches);
    void validate_islands(std::vector<Island>& islands);
    void assign_powers(NetworkData& data);
};

} // namespace pf
