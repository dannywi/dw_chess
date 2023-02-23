#include <gtest/gtest.h>

#include "src/shared/type_list.hpp"

using namespace dwc::utils;

TEST(TypeList, Basic) {
  EXPECT_TRUE((std::is_same<head<float, int>::type, float>::value));
  EXPECT_TRUE((std::is_same<tail<float, int>::type, type_list<int>>::value));
  EXPECT_EQ((size<int, float>::value), 2);
  EXPECT_TRUE((std::is_same<at<0, long, float, double, int>::type, long>::value));
  EXPECT_TRUE((std::is_same<at<1, long, float, double, int>::type, float>::value));
  EXPECT_TRUE((std::is_same<at<2, long, float, double, int>::type, double>::value));
  EXPECT_TRUE((std::is_same<at<3, long, float, double, int>::type, int>::value));

  using TL = type_list<char, long, bool>;
  EXPECT_TRUE((std::is_same<head<TL>::type, char>::value));
  EXPECT_TRUE((std::is_same<tail<TL>::type, type_list<long, bool>>::value));
  EXPECT_EQ((size<TL>::value), 3);
  EXPECT_TRUE((std::is_same<at<0, TL>::type, char>::value));
  EXPECT_TRUE((std::is_same<at<1, TL>::type, long>::value));
  EXPECT_TRUE((std::is_same<at<2, TL>::type, bool>::value));

  EXPECT_TRUE((std::is_same<tail<tail<TL>::type>::type, type_list<bool>>::value));

  EXPECT_EQ(size<TL>::value, 3);
  EXPECT_EQ(size<tail<TL>::type>::value, 2);
  EXPECT_EQ(size<tail<tail<TL>::type>::type>::value, 1);
  EXPECT_EQ(size<tail<tail<tail<TL>::type>::type>::type>::value, 0);
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
  EXPECT_EQ((test::get_default<at<1, TL>::type>()), 0);
  EXPECT_EQ((test::get_default<at<2, TL>::type>()), 0);
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
  using F = typename at<I, TL>::type;
  F::act(v);
  if constexpr (I < size<TL>::value - 1) call_all_impl<I + 1, TL>(v);
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
  using F = typename head<TL>::type;
  F::act(v);
  if constexpr (size<typename tail<TL>::type>::value > 0) call_all_impl<typename tail<TL>::type>(v);
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
