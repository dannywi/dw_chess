#ifndef DWC_UTILS_HPP
#define DWC_UTILS_HPP

#include <algorithm>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <vector>

namespace dwc::utils {

// String split
using StringVecT = std::vector<std::string_view>;
StringVecT split(std::string_view str, std::string_view delim);

// Non-convertible arithmetic type
template <size_t TAG, typename T = int32_t>
class TaggedArithmeticT {
  static_assert(std::is_arithmetic<T>::value || std::is_fundamental<T>::value);
  T v_;

  template <typename U>
  using fund_check = typename std::enable_if<std::is_arithmetic<U>::value || std::is_fundamental<U>::value>::type;

 public:
  constexpr TaggedArithmeticT() : v_(0) {}
  constexpr TaggedArithmeticT(T v) : v_(v) {}

  // conversions, only add necessary ones JIT
  // with own type
  constexpr TaggedArithmeticT operator+(TaggedArithmeticT o) const { return v_ + o.v_; }
  constexpr TaggedArithmeticT operator-(TaggedArithmeticT o) const { return v_ - o.v_; }
  constexpr bool operator==(TaggedArithmeticT o) const { return v_ == o.v_; }
  constexpr bool operator!=(TaggedArithmeticT o) const { return !operator==(o); }

  // with fundamental types
  template <typename U, typename = fund_check<U>>
  constexpr TaggedArithmeticT operator+(U v) const {
    return v_ + v;
  }

  template <typename U, typename = fund_check<U>>
  constexpr TaggedArithmeticT operator-(U v) const {
    return v_ - v;
  }

  template <typename U, typename = fund_check<U>>
  constexpr bool operator==(U v) const {
    return v_ == v;
  }

  template <typename U, typename = fund_check<U>>
  constexpr bool operator<(U v) const {
    return v_ < v;
  }

  constexpr bool operator<(TaggedArithmeticT o) const { return v_ < o.v_; }

  template <typename U, typename = fund_check<U>>
  constexpr bool operator<=(U v) const {
    return v_ <= v;
  }

  template <typename U, typename = fund_check<U>>
  friend U operator+(U v, TaggedArithmeticT t) {
    return v + t.v_;
  }

  template <typename U, typename = fund_check<U>>
  friend U operator-(U v, TaggedArithmeticT t) {
    return v - t.v_;
  }

  template <typename U, typename = fund_check<U>>
  friend bool operator==(U v, TaggedArithmeticT t) {
    return v == t.v_;
  }

  template <typename U, typename = fund_check<U>>
  friend bool operator<=(U v, TaggedArithmeticT t) {
    return v <= t.v_;
  }

  // conversion to fundamental
  template <typename U, typename = fund_check<U>>
  operator U() const {
    return static_cast<U>(v_);
  }

  // printing util
  friend std::ostream& operator<<(std::ostream& os, const TaggedArithmeticT& v) {
    if constexpr (sizeof(T) < sizeof(int16_t))
      os << static_cast<int>(v.v_);
    else
      os << v.v_;
    return os;
  }
};

// component search util
template <class CONT, class T>
bool contains(const CONT& c, T v) {
  return std::find(begin(c), end(c), v) != end(c);
}

}  // namespace dwc::utils

#endif
