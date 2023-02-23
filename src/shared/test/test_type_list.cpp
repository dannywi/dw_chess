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
}
