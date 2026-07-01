#include "utils.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace pf::utils {

std::string repeat_char(char c, int count) {
    return std::string(static_cast<std::size_t>(count > 0 ? count : 0), c);
}

std::string center_text(const std::string& text, int width) {
    int len = static_cast<int>(text.length());
    if (len >= width) return text;
    int left = (width - len) / 2;
    int right = width - len - left;
    return repeat_char(' ', left) + text + repeat_char(' ', right);
}

std::string format_number(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    if (std::abs(value) < 1e-9) {
        oss << 0.0;
    } else {
        oss << value;
    }
    return oss.str();
}

std::string pad_right(const std::string& text, int width) {
    int len = static_cast<int>(text.length());
    if (len >= width) return text;
    return text + repeat_char(' ', width - len);
}

std::string pad_left(const std::string& text, int width) {
    int len = static_cast<int>(text.length());
    if (len >= width) return text;
    return repeat_char(' ', width - len) + text;
}

} // namespace pf::utils
