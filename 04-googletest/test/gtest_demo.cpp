#include <gtest/gtest.h>
#include "math_utils.h"
#include "test_utils.h"
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <limits>
#include <climits>

// ============================================================================
// 1. 基本断言：ASSERT_* vs EXPECT_*
// ============================================================================

// ASSERT_* 会在失败时立即终止测试
TEST(BasicAssertions, AssertMacros) {
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQ(2 + 2, 4);
    ASSERT_NE(2, 3);
    ASSERT_LT(1, 2);  // Less Than
    ASSERT_LE(2, 2);  // Less or Equal
    ASSERT_GT(3, 2);  // Greater Than
    ASSERT_GE(3, 3);  // Greater or Equal
}

// EXPECT_* 会在失败时继续执行测试
TEST(BasicAssertions, ExpectMacros) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_EQ(2 + 2, 4);
    EXPECT_NE(2, 3);
    EXPECT_LT(1, 2);
    EXPECT_LE(2, 2);
    EXPECT_GT(3, 2);
    EXPECT_GE(3, 3);
    
    // 即使这个失败，测试也会继续
    EXPECT_EQ(1, 2) << "这个断言会失败，但测试会继续";
}

// ============================================================================
// 2. 字符串断言
// ============================================================================

TEST(StringAssertions, StringComparison) {
    const char* cstr1 = "hello";
    const char* cstr2 = "world";
    const char* cstr3 = "hello";
    
    std::string str1 = "hello";
    std::string str2 = "world";
    std::string str3 = "hello";
    
    // C字符串比较
    EXPECT_STREQ(cstr1, cstr3);      // 相等
    EXPECT_STRNE(cstr1, cstr2);      // 不相等
    EXPECT_STRCASEEQ("Hello", "hello");  // 忽略大小写相等
    EXPECT_STRCASENE("Hello", "World");  // 忽略大小写不相等
    
    // std::string 比较（使用 == 和 !=）
    EXPECT_EQ(str1, str3);
    EXPECT_NE(str1, str2);
}

// ============================================================================
// 3. 浮点数比较
// ============================================================================

TEST(FloatingPointAssertions, FloatComparison) {
    float f1 = 1.0f;
    float f2 = 1.0000001f;
    double d1 = 0.1 + 0.2;
    double d2 = 0.3;
    
    // 浮点数近似相等（使用ULP）
    EXPECT_FLOAT_EQ(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(f1, f2);  // 非常接近
    
    // 双精度浮点数近似相等
    EXPECT_DOUBLE_EQ(1.0, 1.0);
    EXPECT_NEAR(0.1 + 0.2, 0.3, 0.0001);  // 在误差范围内
    
    // 使用自定义误差范围
    EXPECT_NEAR(3.14159, 3.14, 0.01);
}

// ============================================================================
// 4. 异常测试
// ============================================================================

TEST(ExceptionAssertions, ExceptionTesting) {
    // 测试抛出特定类型的异常
    EXPECT_THROW(MathUtils::divide(5, 0), std::invalid_argument);
    EXPECT_THROW(ExceptionDemo::throwRuntimeError(), std::runtime_error);
    EXPECT_THROW(ExceptionDemo::throwInvalidArgument(), std::invalid_argument);
    
    // 测试抛出任何异常
    EXPECT_ANY_THROW(MathUtils::divide(5, 0));
    EXPECT_ANY_THROW(ExceptionDemo::throwInt());
    
    // 测试不抛出异常
    EXPECT_NO_THROW(MathUtils::add(1, 2));
    EXPECT_NO_THROW(ExceptionDemo::noThrow());
    
    // 测试抛出特定值的异常
    try {
        ExceptionDemo::throwInt();
        FAIL() << "应该抛出int异常";
    } catch (int value) {
        EXPECT_EQ(value, 42);
    }
}

// ============================================================================
// 5. 死亡测试（Death Tests）
// ============================================================================

// 测试程序终止（abort）
TEST(DeathTests, AbortTest) {
    EXPECT_DEATH(DeathTestDemo::abortProgram(), ".*");
}

// 测试程序退出码
TEST(DeathTests, ExitCodeTest) {
    EXPECT_EXIT(DeathTestDemo::exitWithCode(1), ::testing::ExitedWithCode(1), ".*");
}

// 测试除以零（在某些平台上会导致SIGFPE）
// 注意：在Windows上除以零可能不会触发信号，所以这个测试可能不会按预期工作
// 在实际使用中，应该根据平台特性调整死亡测试
#ifdef _WIN32
// Windows上除以零可能不会触发信号，所以跳过或使用不同的测试方式
TEST(DeathTests, DISABLED_DivideByZeroTest) {
    // Windows上这个测试被禁用
    EXPECT_DEATH(DeathTestDemo::divideByZero(), ".*");
}
#else
TEST(DeathTests, DivideByZeroTest) {
    EXPECT_DEATH(DeathTestDemo::divideByZero(), ".*");
}
#endif

// ============================================================================
// 6. 测试夹具（Test Fixtures）
// ============================================================================

// 定义一个测试夹具
class CounterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前执行
        counter_.reset();
        counter_.increment();  // 初始化为1
    }
    
    void TearDown() override {
        // 每个测试后执行
        // 这里可以清理资源
    }
    
    Counter counter_;
};

// 使用测试夹具的测试
TEST_F(CounterTest, InitialValue) {
    EXPECT_EQ(counter_.get(), 1);
}

TEST_F(CounterTest, Increment) {
    counter_.increment();
    EXPECT_EQ(counter_.get(), 2);
}

TEST_F(CounterTest, Decrement) {
    counter_.decrement();
    EXPECT_EQ(counter_.get(), 0);
}

TEST_F(CounterTest, Reset) {
    counter_.increment();
    counter_.increment();
    counter_.reset();
    EXPECT_EQ(counter_.get(), 0);
}

// ============================================================================
// 7. 测试套件级别的设置和清理
// ============================================================================

class TestSuiteFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // 整个测试套件执行前执行一次
        shared_resource_ = new int(100);
    }
    
    static void TearDownTestSuite() {
        // 整个测试套件执行后执行一次
        delete shared_resource_;
        shared_resource_ = nullptr;
    }
    
    static int* shared_resource_;
};

int* TestSuiteFixture::shared_resource_ = nullptr;

TEST_F(TestSuiteFixture, Test1) {
    ASSERT_NE(shared_resource_, nullptr);
    EXPECT_EQ(*shared_resource_, 100);
}

TEST_F(TestSuiteFixture, Test2) {
    ASSERT_NE(shared_resource_, nullptr);
    *shared_resource_ = 200;
    EXPECT_EQ(*shared_resource_, 200);
}

// ============================================================================
// 8. 参数化测试（Value-Parameterized Tests）
// ============================================================================

// 定义参数化测试
class IsEvenTest : public ::testing::TestWithParam<int> {};

TEST_P(IsEvenTest, TestsIsEven) {
    int value = GetParam();
    bool expected = (value % 2 == 0);
    EXPECT_EQ(MathUtils::isEven(value), expected);
}

INSTANTIATE_TEST_SUITE_P(
    EvenNumbers,
    IsEvenTest,
    ::testing::Values(0, 2, 4, 6, 8, -2, -4)
);

INSTANTIATE_TEST_SUITE_P(
    OddNumbers,
    IsEvenTest,
    ::testing::Values(1, 3, 5, 7, -1, -3)
);

// 多个参数的参数化测试
class AdditionParamTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

TEST_P(AdditionParamTest, MultipleParams) {
    int a = std::get<0>(GetParam());
    int b = std::get<1>(GetParam());
    int expected = std::get<2>(GetParam());
    EXPECT_EQ(MathUtils::add(a, b), expected);
}

INSTANTIATE_TEST_SUITE_P(
    AdditionCases,
    AdditionParamTest,
    ::testing::Values(
        std::make_tuple(1, 1, 2),
        std::make_tuple(0, 0, 0),
        std::make_tuple(-1, 1, 0),
        std::make_tuple(10, -5, 5),
        std::make_tuple(-3, -2, -5),
        std::make_tuple(100, 200, 300)
    )
);

// 使用范围生成参数
class RangeTest : public ::testing::TestWithParam<int> {};

TEST_P(RangeTest, RangeValues) {
    int value = GetParam();
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 10);
}

INSTANTIATE_TEST_SUITE_P(
    Range,
    RangeTest,
    ::testing::Range(0, 11)  // 0到10
);

// 使用布尔值组合
class BoolCombinationTest : public ::testing::TestWithParam<std::tuple<bool, bool>> {};

TEST_P(BoolCombinationTest, BoolCombinations) {
    bool a = std::get<0>(GetParam());
    bool b = std::get<1>(GetParam());
    bool result = a && b;
    EXPECT_EQ(result, a && b);
}

INSTANTIATE_TEST_SUITE_P(
    BoolCombinations,
    BoolCombinationTest,
    ::testing::Combine(
        ::testing::Bool(),
        ::testing::Bool()
    )
);

// ============================================================================
// 9. 类型化测试（Typed Tests）
// ============================================================================

// 定义类型列表
typedef ::testing::Types<int, float, double> NumericTypes;

template<typename T>
class TypedStackTest : public ::testing::Test {};

TYPED_TEST_SUITE(TypedStackTest, NumericTypes);

TYPED_TEST(TypedStackTest, PushAndPop) {
    Stack<TypeParam> stack;
    EXPECT_TRUE(stack.empty());
    
    stack.push(static_cast<TypeParam>(1));
    stack.push(static_cast<TypeParam>(2));
    
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 2);
    
    TypeParam top = stack.top();
    EXPECT_EQ(top, static_cast<TypeParam>(2));
    
    TypeParam popped = stack.pop();
    EXPECT_EQ(popped, static_cast<TypeParam>(2));
    EXPECT_EQ(stack.size(), 1);
}

// ============================================================================
// 10. 谓词断言（Predicate Assertions）
// ============================================================================

bool IsPositive(int n) {
    return n > 0;
}

bool IsEvenNumber(int n) {
    return n % 2 == 0;
}

TEST(PredicateAssertions, PredicateTest) {
    EXPECT_PRED1(IsPositive, 5);
    EXPECT_PRED1(IsEvenNumber, 4);
    
    // 多参数谓词
    EXPECT_PRED2([](int a, int b) { return a > b; }, 5, 3);
}

// ============================================================================
// 11. 容器断言
// ============================================================================

TEST(ContainerAssertions, VectorTest) {
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    std::vector<int> vec2 = {1, 2, 3, 4, 5};
    std::vector<int> vec3 = {1, 2, 3, 4, 6};
    
    EXPECT_EQ(vec1, vec2);
    EXPECT_NE(vec1, vec3);
    EXPECT_EQ(vec1.size(), 5);
}

TEST(ContainerAssertions, StringVectorTest) {
    std::vector<std::string> vec = {"hello", "world", "test"};
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], "hello");
    EXPECT_EQ(vec[1], "world");
    EXPECT_EQ(vec[2], "test");
}

// ============================================================================
// 12. 自定义断言消息
// ============================================================================

TEST(CustomMessages, AssertionMessages) {
    int expected = 10;
    int actual = 5;
    
    // 使用 << 操作符添加自定义消息
    EXPECT_EQ(actual, expected) << "实际值应该是 " << expected 
                                 << " 但得到 " << actual;
    
    // 多个条件检查
    int value = 15;
    EXPECT_GT(value, 10) << "值应该大于10";
    EXPECT_LT(value, 20) << "值应该小于20";
}

// ============================================================================
// 13. 测试部分结果（Test Part Results）
// ============================================================================

TEST(TestPartResults, NonFatalFailures) {
    // 非致命失败不会停止测试
    EXPECT_EQ(1, 2) << "第一个失败";
    EXPECT_EQ(3, 4) << "第二个失败";
    
    // 检查是否有非致命失败
    if (::testing::Test::HasNonfatalFailure()) {
        // 可以在这里做一些清理工作
    }
}

TEST(TestPartResults, FatalFailures) {
    // 致命失败会停止测试
    ASSERT_EQ(1, 1);  // 这个会通过
    // ASSERT_EQ(1, 2);  // 如果取消注释，这之后的代码不会执行
    EXPECT_EQ(2, 2);  // 这行会执行（如果上面的断言通过）
}

// ============================================================================
// 14. 禁用测试（DISABLED_）
// ============================================================================

// 被禁用的测试不会运行，除非使用 --gtest_also_run_disabled_tests
TEST(DISABLED_DisabledTests, ThisTestIsDisabled) {
    EXPECT_TRUE(false);  // 这个测试被禁用，不会运行
}

TEST_F(CounterTest, DISABLED_DisabledFixtureTest) {
    EXPECT_EQ(1, 2);  // 这个测试被禁用
}

// ============================================================================
// 15. 测试失败和成功宏
// ============================================================================

TEST(FailureMacros, FailMacro) {
    // FAIL() 宏会立即失败测试
    // FAIL() << "这个测试总是失败";
    
    // SUCCEED() 宏显式标记测试成功
    SUCCEED() << "测试成功";
}

// ============================================================================
// 16. 字符串处理测试
// ============================================================================

TEST(StringProcessorTests, ToUpper) {
    EXPECT_EQ(StringProcessor::toUpper("hello"), "HELLO");
    EXPECT_EQ(StringProcessor::toUpper("Hello World"), "HELLO WORLD");
    EXPECT_EQ(StringProcessor::toUpper("123abc"), "123ABC");
}

TEST(StringProcessorTests, ToLower) {
    EXPECT_EQ(StringProcessor::toLower("HELLO"), "hello");
    EXPECT_EQ(StringProcessor::toLower("Hello World"), "hello world");
}

TEST(StringProcessorTests, Reverse) {
    EXPECT_EQ(StringProcessor::reverse("hello"), "olleh");
    EXPECT_EQ(StringProcessor::reverse(""), "");
    EXPECT_EQ(StringProcessor::reverse("a"), "a");
}

TEST(StringProcessorTests, IsEmpty) {
    EXPECT_TRUE(StringProcessor::isEmpty(""));
    EXPECT_FALSE(StringProcessor::isEmpty("hello"));
}

TEST(StringProcessorTests, Length) {
    EXPECT_EQ(StringProcessor::length(""), 0);
    EXPECT_EQ(StringProcessor::length("hello"), 5);
    EXPECT_EQ(StringProcessor::length("hello world"), 11);
}

// ============================================================================
// 17. 边界值测试
// ============================================================================

TEST(BoundaryTests, IntegerBoundaries) {
    // 测试整数边界
    EXPECT_EQ(MathUtils::add(0, 0), 0);
    EXPECT_EQ(MathUtils::add(INT_MAX, 0), INT_MAX);
    EXPECT_EQ(MathUtils::add(INT_MIN, 0), INT_MIN);
}

TEST(BoundaryTests, DivisionBoundaries) {
    EXPECT_DOUBLE_EQ(MathUtils::divide(0, 5), 0.0);
    EXPECT_DOUBLE_EQ(MathUtils::divide(5, 1), 5.0);
    EXPECT_THROW(MathUtils::divide(5, 0), std::invalid_argument);
}

// ============================================================================
// 18. 复杂场景测试
// ============================================================================

TEST(ComplexScenarios, MultipleOperations) {
    int result = MathUtils::add(5, 3);
    result = MathUtils::multiply(result, 2);
    result = MathUtils::subtract(result, 4);
    EXPECT_EQ(result, 12);
}

TEST(ComplexScenarios, ConditionalLogic) {
    int value = 10;
    if (MathUtils::isEven(value)) {
        value = MathUtils::multiply(value, 2);
    } else {
        value = MathUtils::add(value, 1);
    }
    EXPECT_EQ(value, 20);
    EXPECT_TRUE(MathUtils::isEven(value));
}

// ============================================================================
// 19. 性能相关测试（简单示例）
// ============================================================================

TEST(PerformanceTests, SimpleOperation) {
    // 简单的性能测试示例
    const int iterations = 1000;
    int sum = 0;
    
    for (int i = 0; i < iterations; ++i) {
        sum = MathUtils::add(sum, i);
    }
    
    EXPECT_GT(sum, 0);
    EXPECT_EQ(sum, (iterations - 1) * iterations / 2);
}

// ============================================================================
// 20. 测试记录属性（用于XML报告）
// ============================================================================

TEST(PropertyRecording, RecordProperties) {
    ::testing::Test::RecordProperty("测试类型", "功能测试");
    ::testing::Test::RecordProperty("优先级", "高");
    ::testing::Test::RecordProperty("执行时间", "100ms");
    
    EXPECT_TRUE(true);
}

// ============================================================================
// 21. 使用测试过滤器的示例（通过命令行参数控制）
// ============================================================================

// 这些测试可以通过 --gtest_filter 来过滤
// 例如: --gtest_filter=FilterTests.* 只运行这个测试套件
TEST(FilterTests, Test1) {
    EXPECT_TRUE(true);
}

TEST(FilterTests, Test2) {
    EXPECT_TRUE(true);
}

TEST(OtherTests, Test1) {
    EXPECT_TRUE(true);
}

// ============================================================================
// 22. 测试命名约定示例
// ============================================================================

// 使用描述性的测试套件和测试名称
TEST(MathUtils_Addition_Test, PositiveNumbers) {
    EXPECT_EQ(MathUtils::add(1, 2), 3);
}

TEST(MathUtils_Addition_Test, NegativeNumbers) {
    EXPECT_EQ(MathUtils::add(-1, -2), -3);
}

TEST(MathUtils_Addition_Test, MixedNumbers) {
    EXPECT_EQ(MathUtils::add(-1, 2), 1);
}

// ============================================================================
// 主函数（可选，如果使用libgtest_main.a则不需要）
// ============================================================================

// 注意：如果链接了libgtest_main.a，则不需要main函数
// 如果需要自定义main函数，可以取消下面的注释

/*
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/

