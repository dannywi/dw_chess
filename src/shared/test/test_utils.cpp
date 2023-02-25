#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/shared/utils.hpp"

using dwc::utils::split;
using dwc::utils::StringVecT;
using dwc::utils::TaggedArithmeticT;

TEST(StringSplit, Variants) {
  EXPECT_EQ(split("abcd|defg|1234", "|"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("abcd++defg++1234++", "++"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("/abcd/defg/1234/", "/"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("//", "/"), StringVecT({}));
  EXPECT_EQ(split("PPPP", "/"), StringVecT({"PPPP"}));
  EXPECT_EQ(split("/PPPP", "/"), StringVecT({"PPPP"}));
  EXPECT_EQ(split("////PPPP/AAAA////", "/"), StringVecT({"PPPP", "AAAA"}));
}

TEST(TaggedArithmeticT, Operator01) {
  using Type1 = TaggedArithmeticT<1, int8_t>;
  Type1 a{10}, b{-8}, c, d{12};

  EXPECT_EQ(a + c, 10);
  EXPECT_EQ(a + b, 2);
  EXPECT_EQ(a + b + d, 14);

  EXPECT_EQ(b + 3, Type1{-5});
  EXPECT_EQ(b + 3, -5);
  EXPECT_EQ(b - 3, Type1{-11});
  EXPECT_EQ(b - 3, -11);

  EXPECT_EQ(3 + a, 13);
  EXPECT_EQ(3 + a, Type1{13});
  EXPECT_EQ(11 - d, -1);
  EXPECT_EQ(11 - d, Type1{-1});

  EXPECT_EQ(15 + a, 25);
  EXPECT_EQ(15 + a, Type1{25});
  EXPECT_EQ(15 - a, 5);
  EXPECT_EQ(15 - a, Type1{5});

  EXPECT_EQ(1 - a + b - 3 - 4 - c + d + 12, 0);
  EXPECT_EQ(a + 1 + b - 3 - 4 - c + d + 12, 20);

  EXPECT_FALSE(22 == a);
  EXPECT_FALSE(a == 22);
}

TEST(TaggedArithmeticT, Operator02) {
  using Type = TaggedArithmeticT<1, uint64_t>;

  EXPECT_TRUE(Type{10} < 11UL);
  EXPECT_TRUE(Type{10} <= 10UL);
  EXPECT_TRUE(Type{10} <= 11UL);
}

TEST(TaggedArithmeticT, Conversion01) {
  using Type1 = TaggedArithmeticT<8, int16_t>;
  using Type2 = TaggedArithmeticT<9, int16_t>;
  using Type3 = TaggedArithmeticT<9, double>;

  EXPECT_FALSE((std::is_convertible<Type1, Type2>::value));
  EXPECT_FALSE((std::is_convertible<Type1, Type3>::value));
  EXPECT_FALSE((std::is_convertible<Type2, Type3>::value));

  // convertible if fundamental type at left
  EXPECT_EQ(3 + Type1{2}, 4 + Type2{1});
  EXPECT_EQ(3 + Type1{2}, 4 + Type3{1});
}

TEST(TaggedArithmeticT, Conversion02) {
  using Type1 = TaggedArithmeticT<1, int16_t>;
  using Type2 = TaggedArithmeticT<1, int16_t>;

  EXPECT_TRUE((std::is_convertible<Type1, Type2>::value));
}

TEST(TaggedArithmeticT, Conversion04) {
  using Type1 = TaggedArithmeticT<4, float>;
  EXPECT_EQ(Type1{3} + 3, 6);

  int arr[] = {3, 4, 5};
  EXPECT_EQ(arr[static_cast<int>(Type1{1})], 4);
}

namespace TestContains {
template <class CONT>
void Action1() {
  using dwc::utils::contains;
  CONT cont{-3, 99, 2, -10};
  EXPECT_TRUE(contains(cont, -3));
  EXPECT_TRUE(contains(cont, 99));
  EXPECT_TRUE(contains(cont, 2));
  EXPECT_FALSE(contains(cont, -3.3));
  EXPECT_FALSE(contains(cont, 0));
}

template <class CONT, class... REST>
void Action() {
  Action1<CONT>();
  if constexpr (sizeof...(REST) > 0) Action<REST...>();
}
}  // namespace TestContains

TEST(BasicUtils, Contains) {
  TestContains::Action<std::vector<int>, std::set<double>, std::array<float, 4>>();
}
