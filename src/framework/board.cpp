#include "board.hpp"

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

void Board::move(Move move) {
  check_move(move.fr, move.to);
  std::optional<Piece> piece = get(move.fr);

  // todo: make this atomic?
  clear(move.fr);
  set(move.to, piece.value());

  call_updaters<MoverUpdaterList>(state_, move);
}
}  // namespace dwc
