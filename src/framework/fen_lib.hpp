#pragma once

#include <iostream>  // delete
#include <string_view>
#include <vector>

#include "basic_types.hpp"
#include "src/shared/utils.hpp"

namespace dwc::fen {
namespace _inner {
bool is_fen_num(char c) {
  return '1' <= c && c <= '8';
}
}  // namespace _inner
dwc::BoardT gen_board_from_fen(std::string_view b_str) {
  dwc::BoardT board;
  auto charPieceMap = getCharPieceMap();
  utils::StringVecT strings = utils::split(b_str, "/");
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
      board[file++][rank] = {it->second};
    }
  }
  return board;
}
}  // namespace dwc::fen
