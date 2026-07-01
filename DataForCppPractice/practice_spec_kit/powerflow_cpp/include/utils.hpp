#pragma once

#include <string>
#include <vector>
#include "types.hpp"

namespace pf::utils {

std::string repeat_char(char c, int count);
std::string center_text(const std::string& text, int width);
std::string format_number(double value, int precision = 3);
std::string pad_right(const std::string& text, int width);
std::string pad_left(const std::string& text, int width);

} // namespace pf::utils
