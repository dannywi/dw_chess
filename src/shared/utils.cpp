#include "utils.hpp"

dwc::utils::StringVecT dwc::utils::split(std::string_view str, std::string_view delim) {
  dwc::utils::StringVecT res;
  size_t st = 0;
  while (st < str.length()) {
    size_t en = str.find(delim, st);
    if (en == std::string_view::npos) en = str.length();
    if (st < en) res.push_back(str.substr(st, en - st));
    st = en + delim.length();
  }
  return res;
}
