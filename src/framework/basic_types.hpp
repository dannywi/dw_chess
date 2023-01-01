#pragma once

namespace dwc {

enum class Type : uint8_t {
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  SIZE,
};

enum class Side : uint8_t {
  WHITE,
  BLACK,
  SIZE,
};

template <typename ENUM, typename T = size_t>
T cast_t(ENUM e) {
  return static_cast<T>(e);
}
}  // namespace dwc
