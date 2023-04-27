#pragma once

#include <array>

namespace dwc::utils {
template <typename K, typename V, size_t N>
class Map {
 public:
  using data_t = std::array<std::pair<K, V>, N>;
  constexpr Map(const data_t& d) : data_(d) {}
  V operator[](K k) const {
    // todo: make non linear complexity
    for (const auto& item : data_) {
      if (item.first == k) return item.second;
    }
    throw std::logic_error("no match for key");
  }

  constexpr size_t size() const { return N; }

 private:
  data_t data_;
};
}  // namespace dwc::utils
