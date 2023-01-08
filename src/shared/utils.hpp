#ifndef DWC_UTILS_HPP
#define DWC_UTILS_HPP

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
  TaggedArithmeticT() : v_(0) {}
  TaggedArithmeticT(T v) : v_(v) {}

  // conversions, only add necessary ones JIT
  // with own type
  TaggedArithmeticT operator+(TaggedArithmeticT o) const { return v_ + o.v_; }
  TaggedArithmeticT operator-(TaggedArithmeticT o) const { return v_ - o.v_; }
  bool operator==(TaggedArithmeticT o) const { return v_ == o.v_; }

  // with fundamental types
  template <typename U, typename = fund_check<U>>
  TaggedArithmeticT operator+(U v) const {
    return v_ + v;
  }

  template <typename U, typename = fund_check<U>>
  TaggedArithmeticT operator-(U v) const {
    return v_ - v;
  }

  template <typename U, typename = fund_check<U>>
  bool operator==(U v) const {
    return v_ == v;
  }

  template <typename U, typename = fund_check<U>>
  bool operator<(U v) const {
    return v_ < v;
  }

  template <typename U, typename = fund_check<U>>
  bool operator<=(U v) const {
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

}  // namespace dwc::utils

#endif
