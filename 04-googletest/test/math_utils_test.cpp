#include <gtest/gtest.h>
#include "math_utils.h"

// 测试加法功能
TEST(MathUtilsTest, Addition)
{
    EXPECT_EQ(MathUtils::add(2, 3), 5);
    EXPECT_EQ(MathUtils::add(-2, 3), 1);
    EXPECT_EQ(MathUtils::add(0, 0), 0);
    EXPECT_EQ(MathUtils::add(-5, -3), -8);
}

// 测试减法功能
TEST(MathUtilsTest, Subtraction)
{
    EXPECT_EQ(MathUtils::subtract(5, 3), 2);
    EXPECT_EQ(MathUtils::subtract(3, 5), -2);
    EXPECT_EQ(MathUtils::subtract(0, 0), 0);
    EXPECT_EQ(MathUtils::subtract(-2, -3), 1);
}

// 测试乘法功能
TEST(MathUtilsTest, Multiplication)
{
    EXPECT_EQ(MathUtils::multiply(2, 3), 6);
    EXPECT_EQ(MathUtils::multiply(-2, 3), -6);
    EXPECT_EQ(MathUtils::multiply(0, 5), 0);
    EXPECT_EQ(MathUtils::multiply(-2, -3), 6);
}

// 测试除法功能
TEST(MathUtilsTest, Division)
{
    // 正常除法
    EXPECT_DOUBLE_EQ(MathUtils::divide(6, 3), 2.0);
    EXPECT_DOUBLE_EQ(MathUtils::divide(5, 2), 2.5);
    EXPECT_DOUBLE_EQ(MathUtils::divide(-6, 3), -2.0);

    // 除零异常测试
    EXPECT_THROW(MathUtils::divide(5, 0), std::invalid_argument);
}

// 测试奇偶判断功能
TEST(MathUtilsTest, EvenCheck)
{
    EXPECT_TRUE(MathUtils::isEven(0));
    EXPECT_TRUE(MathUtils::isEven(2));
    EXPECT_TRUE(MathUtils::isEven(-4));
    EXPECT_FALSE(MathUtils::isEven(1));
    EXPECT_FALSE(MathUtils::isEven(-3));
}

// 参数化测试示例：测试多个输入输出组合
class AdditionTest : public ::testing::TestWithParam<std::tuple<int, int, int>>
{
};

TEST_P(AdditionTest, MultipleAdditionCases)
{
    int a = std::get<0>(GetParam());
    int b = std::get<1>(GetParam());
    int expected = std::get<2>(GetParam());

    EXPECT_EQ(MathUtils::add(a, b), expected);
}
/**
 * @brief 参数化测试示例：测试多个输入输出组合
 * 
 */
INSTANTIATE_TEST_SUITE_P(
    AdditionCases,
    AdditionTest,
    ::testing::Values(
        std::make_tuple(1, 1, 2),
        std::make_tuple(0, 0, 0),
        std::make_tuple(-1, 1, 0),
        std::make_tuple(10, -5, 5),
        std::make_tuple(-3, -2, -5)));