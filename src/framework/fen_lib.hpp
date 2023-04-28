#pragma once

#include <iostream>  // delete
#include <set>
#include <string_view>

#include "basic_types.hpp"
#include "src/shared/utils.hpp"

namespace dwc::fen {
namespace _inner {
bool is_fen_num(char c) {
  return '1' <= c && c <= '8';
}

utils::StringVecT split_segments(std::string_view fen_str) {
  return utils::split(fen_str, " ");
}

dwc::BoardT parse_board_pos(std::string_view str) {
  dwc::BoardT b;
  auto charPieceMap = getCharPieceMap();
  utils::StringVecT strings = utils::split(str, "/");
  if (strings.size() != 8) throw std::runtime_error("fen board ill formatted - rank");

  for (size_t rank = 0; rank < 8; ++rank) {
    size_t rank_inv = 7 - rank;
    std::string_view rank_str = strings[rank_inv];
    if (rank_str.size() > 8) throw std::runtime_error("fen board ill formatted - file");
    size_t file = 0;
    for (size_t i = 0; i < rank_str.size(); ++i) {
      char c = rank_str.at(i);
      if (_inner::is_fen_num(c)) {
        // if number 1 - 8, then fill in blanks, i.e. skip
        size_t skip_count = static_cast<size_t>(c - '0');
        file += skip_count;
        if (file > 8) throw std::runtime_error("fen board ill formatted - skip count");
        continue;
      }
      auto it = charPieceMap.find(c);
      if (it == charPieceMap.end()) throw std::runtime_error("fen board ill formatted - notation");
      b[file++][rank] = {it->second};
    }
  }
  return b;
}

std::optional<dwc::Side> parse_side(std::string_view str) {
  std::optional<dwc::Side> ret;
  if (str.size() != 1) throw std::runtime_error("fen string ill formatted - turn side");
  switch (str[0]) {
    case 'w':
    case 'W':
      ret = dwc::Side::WHITE;
      break;
    case 'b':
    case 'B':
      ret = dwc::Side::BLACK;
      break;
    default:
      throw std::runtime_error("fen string ill formatted - unrecognized turn side");
  }
  return ret;
}

std::set<dwc::Piece> parse_castling(std::string_view str) {
  if (str.size() > 4) throw std::runtime_error("fen string ill formatted - too many castling entries");
  std::set<dwc::Piece> castling;
  std::set<dwc::Piece> allowed_values{
      {Type::KING, Side::WHITE}, {Type::QUEEN, Side::WHITE}, {Type::KING, Side::BLACK}, {Type::QUEEN, Side::BLACK}};
  const auto charPieceMap = dwc::getCharPieceMap();

  for (char c : str) {
    auto it = charPieceMap.find(c);
    if (it == charPieceMap.end() || allowed_values.find(it->second) == allowed_values.end()) {
      throw std::runtime_error("fen string ill formatted - unrecognized castling entry");
    }
    dwc::Piece p = it->second;
    if (castling.find(p) != castling.end()) { throw std::runtime_error("fen string ill formatted - double entry"); }
    castling.insert(p);
  }
  return castling;
}

}  // namespace _inner

class FenParser {
  utils::StringVecT segments_;
  dwc::BoardT board_;
  std::optional<dwc::Side> turn_side_;
  std::set<dwc::Piece> castling_;

 public:
  FenParser(std::string_view fen_str)
      : segments_(_inner::split_segments(fen_str)), board_(_inner::parse_board_pos(segments_[0])) {
    if (segments_.size() >= 2) turn_side_ = _inner::parse_side(segments_[1]);
    if (segments_.size() >= 3) castling_ = _inner::parse_castling(segments_[2]);
    if (segments_.size() > 3) throw std::runtime_error("fen string has too many segments");
  }

  dwc::BoardT get_board_pos() const { return board_; }
  std::optional<dwc::Side> get_turn_side() const { return turn_side_; }
  std::set<dwc::Piece> get_castling() const { return castling_; }
};

}  // namespace dwc::fen
