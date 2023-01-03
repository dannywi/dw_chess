#pragma once
#include <iostream>  // delete
#include <map>
#include <vector>

#include "board.hpp"

namespace dwc::legal_move {
using MovesT = std::vector<Move>;

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
  std::vector<MoveDirection> directions;
  MoveLimit limit;
};

using MoverDictT = std::map<dwc::Piece, Mover>;

MoverDictT get_mover_dict() {
  // todo: make constexpr
  MoverDictT dict;
  dict[{Type::PAWN, Side::WHITE}] = dict[{Type::PAWN, Side::BLACK}] = {};
  dict[{Type::KING, Side::WHITE}] = dict[{Type::KING, Side::BLACK}] = {{
                                                                           MoveDirection::DOWN,
                                                                           MoveDirection::UP,
                                                                           MoveDirection::LEFT,
                                                                           MoveDirection::RIGHT,
                                                                           MoveDirection::UPLEFT,
                                                                           MoveDirection::UPRIGHT,
                                                                           MoveDirection::DOWNLEFT,
                                                                           MoveDirection::DOWNRIGHT,
                                                                       },
                                                                       MoveLimit::ONCE};
  dict[{Type::QUEEN, Side::WHITE}] = dict[{Type::QUEEN, Side::BLACK}] = {{
                                                                             MoveDirection::DOWN,
                                                                             MoveDirection::UP,
                                                                             MoveDirection::LEFT,
                                                                             MoveDirection::RIGHT,
                                                                             MoveDirection::UPLEFT,
                                                                             MoveDirection::UPRIGHT,
                                                                             MoveDirection::DOWNLEFT,
                                                                             MoveDirection::DOWNRIGHT,
                                                                         },
                                                                         MoveLimit::UNLIMITED};
  dict[{Type::BISHOP, Side::WHITE}] = dict[{Type::BISHOP, Side::BLACK}] = {{
                                                                               MoveDirection::UPLEFT,
                                                                               MoveDirection::UPRIGHT,
                                                                               MoveDirection::DOWNLEFT,
                                                                               MoveDirection::DOWNRIGHT,
                                                                           },
                                                                           MoveLimit::UNLIMITED};
  dict[{Type::KNIGHT, Side::WHITE}] = dict[{Type::KNIGHT, Side::BLACK}] = {{
                                                                               MoveDirection::KN_UPLEFT,
                                                                               MoveDirection::KN_LEFTUP,
                                                                               MoveDirection::KN_UPRIGHT,
                                                                               MoveDirection::KN_RIGHTUP,
                                                                               MoveDirection::KN_DOWNLEFT,
                                                                               MoveDirection::KN_LEFTDOWN,
                                                                               MoveDirection::KN_DOWNRIGHT,
                                                                               MoveDirection::KN_RIGHTDOWN,
                                                                           },
                                                                           MoveLimit::ONCE};
  dict[{Type::ROOK, Side::WHITE}] = dict[{Type::ROOK, Side::BLACK}] = {{
                                                                           MoveDirection::DOWN,
                                                                           MoveDirection::UP,
                                                                           MoveDirection::LEFT,
                                                                           MoveDirection::RIGHT,
                                                                       },
                                                                       MoveLimit::UNLIMITED};
  return dict;
}

struct MoveDiff {
  Pos::FileT file;
  Pos::RankT rank;
};

MoveDiff get_move_diff(MoveDirection md) {
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
  for (auto dir : mover.directions) {
    MoveDiff md = get_move_diff(dir);
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
  // todo: diagonal captures
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

MovesT get_moves(const Board& board, Pos pos) {
  auto piece = board.get(pos);
  if (!piece.has_value()) return {};
  auto dict = _inner::get_mover_dict();
  auto mover = dict[piece.value()];
  auto moves = _inner::get_moves_from_mover(board, piece.value(), pos, mover);
  _inner::insert(moves, _inner::get_extra_moves(board, piece.value(), pos));
  return moves;
}

}  // namespace dwc::legal_move
