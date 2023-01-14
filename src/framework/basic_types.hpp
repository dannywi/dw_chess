#pragma once

#include <array>
#include <map>

#include "src/shared/utils.hpp"

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
constexpr T cast_t(ENUM e) {
  return static_cast<T>(e);
}

struct Pos {
  using FileT = utils::TaggedArithmeticT<10, int8_t>;
  using RankT = utils::TaggedArithmeticT<20, int8_t>;
  FileT file;
  RankT rank;
  Pos(FileT file, RankT rank) : file(file), rank(rank) {}
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
    if (!check_pos() || !check_file() || !check_rank()) { throw std::logic_error("invalid position"); }

    file = pos[0] - (('a' <= pos[0] && pos[0] <= 'h') ? 'a' : 'A');
    rank = pos[1] - '1';
  }

  Pos() = delete;

  bool operator==(const Pos& p) { return file == p.file && rank == p.rank; }
};

struct Move {
  Pos fr;
  Pos to;
  bool operator==(const Move& m) { return fr == m.fr && to == m.to; }
};

struct Piece {
  Type type;
  Side side;
  bool operator==(const Piece& p) const { return ordinal() == p.ordinal(); }
  bool operator<(const Piece& p) const { return ordinal() < p.ordinal(); }

  using ordinal_t = size_t;
  constexpr ordinal_t ordinal() const { return cast_t(type) * (cast_t(Side::SIZE)) + cast_t(side); }
};

struct Square {
  std::optional<Piece> piece;
};

using BoardT = std::array<std::array<Square, 8>, 8>;

std::map<Piece, char> getPieceCharMap() {
  std::map<Piece, char> map;

  map[{Type::PAWN, Side::WHITE}] = 'P';
  map[{Type::KNIGHT, Side::WHITE}] = 'N';
  map[{Type::BISHOP, Side::WHITE}] = 'B';
  map[{Type::ROOK, Side::WHITE}] = 'R';
  map[{Type::QUEEN, Side::WHITE}] = 'Q';
  map[{Type::KING, Side::WHITE}] = 'K';

  map[{Type::PAWN, Side::BLACK}] = 'p';
  map[{Type::KNIGHT, Side::BLACK}] = 'n';
  map[{Type::BISHOP, Side::BLACK}] = 'b';
  map[{Type::ROOK, Side::BLACK}] = 'r';
  map[{Type::QUEEN, Side::BLACK}] = 'q';
  map[{Type::KING, Side::BLACK}] = 'k';

  return map;
}

std::map<char, Piece> getCharPieceMap() {
  std::map<Piece, char> inv = getPieceCharMap();
  std::map<char, Piece> map;
  for (auto v : inv) { map[v.second] = v.first; }

  return map;
}

}  // namespace dwc
