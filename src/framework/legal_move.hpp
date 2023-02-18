#pragma once
#include <array>
#include <iostream>  // delete
#include <map>
#include <vector>

#include "board.hpp"

namespace dwc::legal_move {

namespace _inner {
enum class MoveDirection : uint8_t {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UPLEFT,
  UPRIGHT,
  DOWNLEFT,
  DOWNRIGHT,
  KN_UPLEFT,
  KN_LEFTUP,
  KN_UPRIGHT,
  KN_RIGHTUP,
  KN_DOWNLEFT,
  KN_LEFTDOWN,
  KN_DOWNRIGHT,
  KN_RIGHTDOWN,
  SIZE,
};

enum class MoveLimit : uint8_t {
  ONCE,
  UNLIMITED,
};

struct Mover {
  // todo: error-prone if used with array::size(), encapsulate
  // if MoveDirections is a class, then maybe get_mover_dict can be a constexpr switch statement
  std::array<MoveDirection, 8> directions;
  size_t directions_size{0};
  MoveLimit limit{MoveLimit::ONCE};
};

using MoverDictT = std::array<Mover, cast_t(Type::SIZE) * cast_t(Side::SIZE)>;

constexpr MoverDictT get_mover_dict() {
  MoverDictT dict{};
  dict[Piece{Type::PAWN, Side::WHITE}.ordinal()] = dict[Piece{Type::PAWN, Side::BLACK}.ordinal()] = Mover{};
  dict[Piece{Type::KING, Side::WHITE}.ordinal()] = dict[Piece{Type::KING, Side::BLACK}.ordinal()] =
      Mover{{
                MoveDirection::DOWN,
                MoveDirection::UP,
                MoveDirection::LEFT,
                MoveDirection::RIGHT,
                MoveDirection::UPLEFT,
                MoveDirection::UPRIGHT,
                MoveDirection::DOWNLEFT,
                MoveDirection::DOWNRIGHT,
            },
            8,
            MoveLimit::ONCE};
  dict[Piece{Type::QUEEN, Side::WHITE}.ordinal()] = dict[Piece{Type::QUEEN, Side::BLACK}.ordinal()] =
      Mover{{
                MoveDirection::DOWN,
                MoveDirection::UP,
                MoveDirection::LEFT,
                MoveDirection::RIGHT,
                MoveDirection::UPLEFT,
                MoveDirection::UPRIGHT,
                MoveDirection::DOWNLEFT,
                MoveDirection::DOWNRIGHT,
            },
            8,
            MoveLimit::UNLIMITED};
  dict[Piece{Type::BISHOP, Side::WHITE}.ordinal()] =
      dict[Piece{Type::BISHOP, Side::BLACK}.ordinal()] = {{
                                                              MoveDirection::UPLEFT,
                                                              MoveDirection::UPRIGHT,
                                                              MoveDirection::DOWNLEFT,
                                                              MoveDirection::DOWNRIGHT,
                                                          },
                                                          4,
                                                          MoveLimit::UNLIMITED};
  dict[Piece{Type::KNIGHT, Side::WHITE}.ordinal()] =
      dict[Piece{Type::KNIGHT, Side::BLACK}.ordinal()] = {{
                                                              MoveDirection::KN_UPLEFT,
                                                              MoveDirection::KN_LEFTUP,
                                                              MoveDirection::KN_UPRIGHT,
                                                              MoveDirection::KN_RIGHTUP,
                                                              MoveDirection::KN_DOWNLEFT,
                                                              MoveDirection::KN_LEFTDOWN,
                                                              MoveDirection::KN_DOWNRIGHT,
                                                              MoveDirection::KN_RIGHTDOWN,
                                                          },
                                                          8,
                                                          MoveLimit::ONCE};
  dict[Piece{Type::ROOK, Side::WHITE}.ordinal()] =
      dict[Piece{Type::ROOK, Side::BLACK}.ordinal()] = {{
                                                            MoveDirection::DOWN,
                                                            MoveDirection::UP,
                                                            MoveDirection::LEFT,
                                                            MoveDirection::RIGHT,
                                                        },
                                                        4,
                                                        MoveLimit::UNLIMITED};
  return dict;
}

struct MoveDiff {
  Pos::FileT file;
  Pos::RankT rank;
  constexpr MoveDiff(Pos::FileT f, Pos::RankT r) : file(f), rank(r) {}
};

constexpr MoveDiff get_move_diff(MoveDirection md) {
  switch (md) {
    case MoveDirection::UP:
      return {0, 1};
    case MoveDirection::DOWN:
      return {0, -1};
    case MoveDirection::LEFT:
      return {-1, 0};
    case MoveDirection::RIGHT:
      return {1, 0};
    case MoveDirection::UPLEFT:
      return {-1, 1};
    case MoveDirection::UPRIGHT:
      return {1, 1};
    case MoveDirection::DOWNLEFT:
      return {-1, -1};
    case MoveDirection::DOWNRIGHT:
      return {1, -1};
    case MoveDirection::KN_UPLEFT:
      return {2, -1};
    case MoveDirection::KN_LEFTUP:
      return {1, -2};
    case MoveDirection::KN_UPRIGHT:
      return {2, 1};
    case MoveDirection::KN_RIGHTUP:
      return {1, 2};
    case MoveDirection::KN_DOWNLEFT:
      return {-2, -1};
    case MoveDirection::KN_LEFTDOWN:
      return {-1, -2};
    case MoveDirection::KN_DOWNRIGHT:
      return {-2, 1};
    case MoveDirection::KN_RIGHTDOWN:
      return {-1, 2};
    case MoveDirection::SIZE:
      break;
  }
  throw std::logic_error("invalid move direction");
  return {0, 0};
}

Pos move(Pos fr, MoveDiff md) {
  return {static_cast<Pos::FileT>(fr.file + md.file), static_cast<Pos::RankT>(fr.rank + md.rank)};
}

bool can_move(const Board& board, Piece piece, Pos pos, MoveDiff md) {
  Pos new_pos = move(pos, md);
  auto inside = [](auto v) { return 0 <= v && v < 8; };
  if (!inside(new_pos.file) || !inside(new_pos.rank)) return false;

  auto tgt_piece = board.get(new_pos);
  if (tgt_piece.has_value() && tgt_piece.value().side == piece.side) return false;
  return true;
}

MovesT get_moves_from_mover(const Board& board, Piece piece, Pos pos, Mover mover) {
  MovesT moves;
  for (size_t i = 0; i < mover.directions_size; ++i) {
    MoveDiff md = get_move_diff(mover.directions[i]);
    Pos curr_pos = pos;
    size_t limit = mover.limit == MoveLimit::ONCE ? 1 : 8;
    for (size_t i = 0; i < limit; ++i) {
      if (can_move(board, piece, curr_pos, md)) {
        Pos new_pos = move(curr_pos, md);
        moves.push_back({pos, new_pos});
        if (board.get(new_pos).has_value()) break;  // stop on capture square
        curr_pos = new_pos;
      } else {
        break;
      }
    }
  }
  return moves;
}

MovesT get_extra_moves_pawn(Board board, Piece piece, Pos pos) {
  if (piece.type != Type::PAWN) return {};

  MovesT moves;
  uint8_t mult = piece.side == Side::WHITE ? 1 : -1;

  auto add_ahead = [&](Pos::RankT rank) {
    Pos pos_ahead{pos.file, rank};
    auto ahead = board.get(pos_ahead);
    if (!ahead.has_value()) { moves.push_back({pos, pos_ahead}); }
  };

  // basic step
  add_ahead(pos.rank + mult);

  // one more if starting from second rank
  if (pos.rank == (piece.side == Side::WHITE ? 1 : 6)) { add_ahead(pos.rank + 2 * mult); }

  // diagonal capture if possible
  auto within = [](auto v) { return 0 <= v && v <= 7; };
  auto inside = [within](Pos pos) { return within(pos.file) && within(pos.rank); };
  auto has_opp = [&board](Side my_side, Pos pos) {
    auto p = board.get(pos);
    return p.has_value() && p->side != my_side;
  };
  auto add_capt = [&](Pos pos_cap) {
    if (inside(pos_cap) && has_opp(piece.side, pos_cap)) moves.push_back({pos, pos_cap});
  };
  add_capt({static_cast<Pos::FileT>(pos.file - 1), static_cast<Pos::RankT>(pos.rank + mult)});
  add_capt({static_cast<Pos::FileT>(pos.file + 1), static_cast<Pos::RankT>(pos.rank + mult)});

  // todo: en passant
  // todo: promotion (may need new struct in Move)
  return moves;
}

template <typename CONT>
void insert(CONT& a, const CONT& b) {
  a.insert(end(a), begin(b), end(b));
}

MovesT get_extra_moves(Board board, Piece piece, Pos pos) {
  MovesT moves;
  insert(moves, get_extra_moves_pawn(board, piece, pos));
  // todo: castling
  return moves;
}

}  // namespace _inner

bool is_legal_move(const Board& board, Pos pos, Move move) {
  auto moves = board.get_moves(pos);
  for (auto m : moves) {
    if (m == move) return true;
  }
  return false;
}

}  // namespace dwc::legal_move

namespace dwc::legal_move {
class MoverPawnAhead {
 public:
  MovesT get_moves(const dwc::Board& board, Pos pos) const {
    MovesT moves;
    auto piece = board.get(pos);
    if (!piece.has_value()) return {};
    uint8_t mult = piece.value().side == Side::WHITE ? 1 : -1;
    auto add_ahead = [&](Pos::RankT rank) {
      Pos pos_ahead{pos.file, rank};
      auto ahead = board.get(pos_ahead);
      if (!ahead.has_value()) { moves.push_back({pos, pos_ahead}); }
    };

    add_ahead(pos.rank + mult);
    return moves;
  };
};
}  // namespace dwc::legal_move
