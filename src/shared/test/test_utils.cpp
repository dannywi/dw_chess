#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/shared/utils.hpp"

using dwc::utils::split;
using dwc::utils::StringVecT;

TEST(StringSplit, Variants) {
  EXPECT_EQ(split("abcd|defg|1234", "|"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("abcd++defg++1234++", "++"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("/abcd/defg/1234/", "/"), StringVecT({"abcd", "defg", "1234"}));
  EXPECT_EQ(split("//", "/"), StringVecT({}));
  EXPECT_EQ(split("PPPP", "/"), StringVecT({"PPPP"}));
  EXPECT_EQ(split("/PPPP", "/"), StringVecT({"PPPP"}));
  EXPECT_EQ(split("////PPPP/AAAA////", "/"), StringVecT({"PPPP", "AAAA"}));
}
