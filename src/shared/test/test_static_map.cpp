#include <gtest/gtest.h>

#include "src/shared/static_map.hpp"

using namespace dwc;

TEST(StaticMap, Basic) {
  constexpr utils::Map<size_t, char, 2> m{{std::make_pair(100, 'B'), std::make_pair(200, 'C')}};
  EXPECT_EQ(m[100], 'B');
  EXPECT_EQ(m[200], 'C');
  EXPECT_NE(m[200], 'D');
  EXPECT_EQ(m.size(), 2);
  EXPECT_THROW(m[3], std::logic_error);
}

TEST(StaticMap, Struct) {
  struct Key {
    uint16_t one;
    float two;
    constexpr bool operator==(const Key& o) const { return o.one == one && o.two == two; }
  };

  struct Value {
    std::pair<std::string_view, char> v;
    constexpr bool operator==(const Value& o) const { return o.v == v; }
    constexpr bool operator!=(const Value& o) const { return !operator==(o); }
  };

  constexpr utils::Map<Key, Value, 3> m{{
      std::make_pair<Key, Value>({3, 1.1}, {std::make_pair("Rambo", 'I')}),
      std::make_pair<Key, Value>({3, 0.9}, {std::make_pair("Terminator", '2')}),
      std::make_pair<Key, Value>({4, 1.1}, {std::make_pair("Die Hard", 'X')}),
  }};

  EXPECT_EQ((m[{3, 1.1}]), (Value{std::make_pair("Rambo", 'I')}));
  EXPECT_NE((m[{3, 1.1}]), (Value{std::make_pair("Terminator", '2')}));

  EXPECT_EQ((m[{3, 0.9}]), (Value{std::make_pair("Terminator", '2')}));
  EXPECT_NE((m[{3, 0.9}]), (Value{std::make_pair("First", 'B')}));

  EXPECT_EQ((m[{4, 1.1}]), (Value{std::make_pair("Die Hard", 'X')}));
  EXPECT_NE((m[{4, 1.1}]), (Value{std::make_pair("Rambo", 'I')}));

  EXPECT_EQ(m.size(), 3);
  EXPECT_THROW((m[{3, 1.11}]), std::logic_error);
}
