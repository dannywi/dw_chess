#include "board.hpp"

#include "legal_move.hpp"

void dwc::Board::check_move(Pos fr, Pos to) const {
  // board doesn't know chess rules, just having squares and pieces. These are "physical" validations.
  std::optional<Piece> piece = get(fr);
  if (!piece.has_value()) { throw std::logic_error("moving empty square"); }
  if (turn_.has_value() && piece.value().side != turn_.value()) { throw std::logic_error("moving wrong side"); }
  if (!legal_move::is_legal_move(*this, fr, {fr, to})) { throw std::logic_error("not legal move"); }
}
