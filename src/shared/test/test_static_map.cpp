#include <gtest/gtest.h>

#include "src/shared/static_map.hpp"

using namespace dwc;

TEST(StaticMap, Basic) {
  constexpr utils::Map<size_t, char, 2> m{{std::make_pair(100, 'B'), std::make_pair(200, 'C')}};
  EXPECT_EQ(m[100], 'B');
  EXPECT_EQ(m[200], 'C');
  EXPECT_EQ(m.size(), 2);
  EXPECT_THROW(m[3], std::logic_error);
}
