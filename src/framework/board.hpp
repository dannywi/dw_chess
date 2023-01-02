#pragma once

#include <array>
#include <optional>

#include "basic_types.hpp"

namespace dwc {

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
  bool operator==(const Piece& p) const { return type == p.type && side == p.side; }
};

struct Square {
  std::optional<Piece> piece;
};

class Board {
  using BoardT = std::array<std::array<Square, 8>, 8>;
  BoardT board_;

 public:
  void set(Pos pos, Piece piece) { board_[pos.file][pos.rank].piece = piece; }
  void clear(Pos pos) { board_[pos.file][pos.rank].piece.reset(); }
  std::optional<Piece> get(Pos pos) const { return board_[pos.file][pos.rank].piece; }

  void reset_position() {
    set({"A1"}, {Type::ROOK, Side::WHITE});
    set({"B1"}, {Type::KNIGHT, Side::WHITE});
    set({"C1"}, {Type::BISHOP, Side::WHITE});
    set({"D1"}, {Type::QUEEN, Side::WHITE});
    set({"E1"}, {Type::KING, Side::WHITE});
    set({"F1"}, {Type::BISHOP, Side::WHITE});
    set({"G1"}, {Type::KNIGHT, Side::WHITE});
    set({"H1"}, {Type::ROOK, Side::WHITE});

    set({"A2"}, {Type::PAWN, Side::WHITE});
    set({"B2"}, {Type::PAWN, Side::WHITE});
    set({"C2"}, {Type::PAWN, Side::WHITE});
    set({"D2"}, {Type::PAWN, Side::WHITE});
    set({"E2"}, {Type::PAWN, Side::WHITE});
    set({"F2"}, {Type::PAWN, Side::WHITE});
    set({"G2"}, {Type::PAWN, Side::WHITE});
    set({"H2"}, {Type::PAWN, Side::WHITE});

    set({"A8"}, {Type::ROOK, Side::BLACK});
    set({"B8"}, {Type::KNIGHT, Side::BLACK});
    set({"C8"}, {Type::BISHOP, Side::BLACK});
    set({"D8"}, {Type::QUEEN, Side::BLACK});
    set({"E8"}, {Type::KING, Side::BLACK});
    set({"F8"}, {Type::BISHOP, Side::BLACK});
    set({"G8"}, {Type::KNIGHT, Side::BLACK});
    set({"H8"}, {Type::ROOK, Side::BLACK});

    set({"A7"}, {Type::PAWN, Side::BLACK});
    set({"B7"}, {Type::PAWN, Side::BLACK});
    set({"C7"}, {Type::PAWN, Side::BLACK});
    set({"D7"}, {Type::PAWN, Side::BLACK});
    set({"E7"}, {Type::PAWN, Side::BLACK});
    set({"F7"}, {Type::PAWN, Side::BLACK});
    set({"G7"}, {Type::PAWN, Side::BLACK});
    set({"H7"}, {Type::PAWN, Side::BLACK});
  }

  void move(Pos fr, Pos to) {
    // board doesn't know chess rules, just having squares and pieces
    std::optional<Piece> piece = get(fr);
    if (!piece.has_value()) { throw std::range_error("trying to move empty square"); }
    clear(fr);
    // todo: keep captured piece
    set(to, piece.value());
  }
};
}  // namespace dwc
