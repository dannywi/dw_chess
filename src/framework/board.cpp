#include "board.hpp"

#include <iostream>

#include "legal_move.hpp"

namespace dwc {
void Board::check_move(Pos fr, Pos to) const {
  std::optional<Piece> piece = get(fr);
  if (!piece.has_value()) { throw std::logic_error("moving empty square"); }
  if (state_.turn.has_value() && piece.value().side != state_.turn.value()) {
    throw std::logic_error("moving wrong side");
  }
  if (!legal_move::is_legal_move(*this, fr, {fr, to})) { throw std::logic_error("not legal move"); }
}

MovesT Board::get_moves(Pos pos) const {
  MovesT moves;
  call_movers<MoverUpdaterList>(pos, moves);
  return moves;
}

void Board::dump_moves(Pos pos) const {
  auto moves = get_moves(pos);
  std::cout << "Available Moves " << pos << std::endl;
  for (const auto& i : moves) { std::cout << "- " << i << std::endl; }
}

void Board::move(Move move) {
  check_move(move.fr, move.to);
  std::optional<Piece> piece = get(move.fr);

  // todo: make this atomic?
  clear(move.fr);
  set(move.to, piece.value());

  call_updaters<MoverUpdaterList>(state_, move);
}

bool Board::is_threatened(Pos pos) const {
  auto tgt = get(pos);
  if (!tgt.has_value()) { return false; }

  // todo: create an iterator, using generator coroutine if possible
  for (char rank = '8'; rank >= '1'; --rank) {
    for (char file = 'A'; file <= 'H'; ++file) {
      Pos curr_pos{std::string{file, rank}};
      auto curr_pcs = get(curr_pos);
      if (!curr_pcs.has_value() || curr_pcs->side == tgt->side) { continue; }
      auto moves = get_moves(curr_pos);
      for (const auto& move : moves) {
        if (move.to == pos) { return true; }
      }
    }
  }

  return false;
}
}  // namespace dwc
