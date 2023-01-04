#ifndef DWC_UTILS_HPP
#define DWC_UTILS_HPP

#include <string_view>
#include <vector>

namespace dwc::utils {
using StringVecT = std::vector<std::string_view>;
StringVecT split(std::string_view str, std::string_view delim);
}  // namespace dwc::utils

#endif
