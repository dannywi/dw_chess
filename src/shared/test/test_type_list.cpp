#include <gtest/gtest.h>

#include "src/shared/type_list.hpp"

using namespace dwc::utils;

TEST(TypeList, Basic) {
  EXPECT_TRUE((std::is_same_v<head_t<float, int>, float>));
  EXPECT_TRUE((std::is_same_v<tail_t<float, int>, type_list<int>>));
  EXPECT_EQ((size_v<int, float>), 2);
  EXPECT_TRUE((std::is_same_v<at_t<0, long, float, double, int>, long>));
  EXPECT_TRUE((std::is_same_v<at_t<1, long, float, double, int>, float>));
  EXPECT_TRUE((std::is_same_v<at_t<2, long, float, double, int>, double>));
  EXPECT_TRUE((std::is_same_v<at_t<3, long, float, double, int>, int>));

  using TL = type_list<char, long, bool>;
  EXPECT_TRUE((std::is_same_v<head_t<TL>, char>));
  EXPECT_TRUE((std::is_same_v<tail_t<TL>, type_list<long, bool>>));
  EXPECT_EQ((size_v<TL>), 3);
  EXPECT_TRUE((std::is_same_v<at_t<0, TL>, char>));
  EXPECT_TRUE((std::is_same_v<at_t<1, TL>, long>));
  EXPECT_TRUE((std::is_same_v<at_t<2, TL>, bool>));

  EXPECT_TRUE((std::is_same_v<tail_t<tail_t<TL>>, type_list<bool>>));

  EXPECT_EQ(size_v<TL>, 3);
  EXPECT_EQ(size_v<tail_t<TL>>, 2);
  EXPECT_EQ(size_v<tail_t<tail_t<TL>>>, 1);
  EXPECT_EQ(size_v<tail_t<tail_t<tail_t<TL>>>>, 0);
}

namespace test {
template <typename T>
T get_default() {
  return T{};
}
}  // namespace test

TEST(TypeList, Usage01) {
  EXPECT_EQ(test::get_default<int>(), 0);
  EXPECT_EQ(test::get_default<bool>(), 0);

  using TL = type_list<double, float, int>;
  EXPECT_EQ((test::get_default<at_t<1, TL>>()), 0);
  EXPECT_EQ((test::get_default<at_t<2, TL>>()), 0);
}

namespace test {
template <int N>
struct add {
  static constexpr void act(int& v) { v += N; }
};
}  // namespace test

namespace test1 {
template <size_t I, typename TL>
void call_all_impl(int& v) {
  using F = at_t<I, TL>;
  F::act(v);
  if constexpr (I < size_v<TL> - 1) call_all_impl<I + 1, TL>(v);
}
template <typename TL>
void call_all(int& v) {
  call_all_impl<0, TL>(v);
}
}  // namespace test1

TEST(TypeList, Usage02_LoopWithIndex) {
  using TL = type_list<test::add<3>, test::add<4>>;
  int v = 10;
  test1::call_all<TL>(v);
  EXPECT_EQ(v, 17);
  test1::call_all<TL>(v);
  EXPECT_EQ(v, 24);
}

namespace test2 {
template <typename TL>
void call_all_impl(int& v) {
  using F = head_t<TL>;
  F::act(v);
  if constexpr (size_v < tail_t < TL >>> 0) call_all_impl<tail_t<TL>>(v);
}
template <typename TL>
void call_all(int& v) {
  call_all_impl<TL>(v);
}
}  // namespace test2

TEST(TypeList, Usage03_LoopWithTailRecursion) {
  using TL = type_list<test::add<30>, test::add<40>>;
  int v = 10;
  test2::call_all<TL>(v);
  EXPECT_EQ(v, 80);
  test2::call_all<TL>(v);
  EXPECT_EQ(v, 150);
}
