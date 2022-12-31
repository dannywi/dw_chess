#pragma once

#include <array>
#include <optional>

namespace dwc {
// move to own file

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

char getPieceSymbol(Type type, Side side) {
  // todo: make constexpr
  char map[cast_t(Type::SIZE)][cast_t(Side::SIZE)];

  map[cast_t(Type::PAWN)][cast_t(Side::WHITE)] = 'P';
  map[cast_t(Type::KNIGHT)][cast_t(Side::WHITE)] = 'N';
  map[cast_t(Type::BISHOP)][cast_t(Side::WHITE)] = 'B';
  map[cast_t(Type::ROOK)][cast_t(Side::WHITE)] = 'R';
  map[cast_t(Type::QUEEN)][cast_t(Side::WHITE)] = 'Q';
  map[cast_t(Type::KING)][cast_t(Side::WHITE)] = 'K';

  map[cast_t(Type::PAWN)][cast_t(Side::BLACK)] = 'p';
  map[cast_t(Type::KNIGHT)][cast_t(Side::BLACK)] = 'n';
  map[cast_t(Type::BISHOP)][cast_t(Side::BLACK)] = 'b';
  map[cast_t(Type::ROOK)][cast_t(Side::BLACK)] = 'r';
  map[cast_t(Type::QUEEN)][cast_t(Side::BLACK)] = 'q';
  map[cast_t(Type::KING)][cast_t(Side::BLACK)] = 'k';

  return map[cast_t(type)][cast_t(side)];
}

struct Pos {
  uint8_t file;
  uint8_t rank;
  Pos(std::string_view pos) {
    auto check_pos = [pos]() { return pos.size() == 2; };
    auto check_file = [pos]() {
      auto v = pos[0];
      return ('a' <= v && v <= 'h') || ('A' <= v && v <= 'H');
    };
    auto check_rank = [pos]() {
      auto v = pos[1];
      return ('1' <= v && v <= '8');
    };
    if (!check_pos() || !check_file() || !check_rank()) { throw std::range_error("invalid position"); }

    file = pos[0] - (('a' <= pos[0] && pos[0] <= 'h') ? 'a' : 'A');
    rank = pos[1] - '1';
  }

  Pos() = delete;
};

struct Piece {
  Type type;
  Side side;
};

struct Square {
  std::optional<Piece> piece;
};

class Board {
  std::array<std::array<Square, 8>, 8> board_;

  void initialize() {}

 public:
  Board() { initialize(); }
  bool test() const { return true; }

  void set(Pos pos, Piece piece) { board_[pos.file][pos.rank].piece = piece; }

  void clear(Pos pos) { board_[pos.file][pos.rank].piece.reset(); }

  std::optional<Piece> get(Pos pos) { return board_[pos.file][pos.rank].piece; }
};
}  // namespace dwc
