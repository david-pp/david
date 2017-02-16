#ifndef TINYWORLD_TINYTEST_H
#define TINYWORLD_TINYTEST_H

#include <memory>
#include <string>

struct TestBase
{
    std::string name;
};

class TestSuit : public TinyTestBase
{
public:
    static TestSuit& root()
    {
        static TestSuit suit;
        return suit;
    }
private:
};

#define TEST(suitname, testname)
#define EXPECT_EQ(expected, expr)
#define EXPECT_TRUE(expr)

#if 0

// Tests factorial of negative numbers.
TEST(FactorialTest, Negative) {
// This test is named "Negative", and belongs to the "FactorialTest"
// test case.
EXPECT_EQ(1, Factorial(-5));
EXPECT_EQ(1, Factorial(-1));
EXPECT_GT(Factorial(-10), 0);

// <TechnicalDetails>
//
// EXPECT_EQ(expected, actual) is the same as
//
//   EXPECT_TRUE((expected) == (actual))
//
// except that it will print both the expected value and the actual
// value when the assertion fails.  This is very helpful for
// debugging.  Therefore in this case EXPECT_EQ is preferred.
//
// On the other hand, EXPECT_TRUE accepts any Boolean expression,
// and is thus more general.
//
// </TechnicalDetails>
}

// Tests factorial of 0.
TEST(FactorialTest, Zero) {
EXPECT_EQ(1, Factorial(0));
}

// Macros for testing equalities and inequalities.
//
//    * {ASSERT|EXPECT}_EQ(v1, v2): Tests that v1 == v2
//    * {ASSERT|EXPECT}_NE(v1, v2): Tests that v1 != v2
//    * {ASSERT|EXPECT}_LT(v1, v2): Tests that v1 < v2
//    * {ASSERT|EXPECT}_LE(v1, v2): Tests that v1 <= v2
//    * {ASSERT|EXPECT}_GT(v1, v2): Tests that v1 > v2
//    * {ASSERT|EXPECT}_GE(v1, v2): Tests that v1 >= v2
//


// C-string Comparisons.  All tests treat NULL and any non-NULL string
// as different.  Two NULLs are equal.
//
//    * {ASSERT|EXPECT}_STREQ(s1, s2):     Tests that s1 == s2
//    * {ASSERT|EXPECT}_STRNE(s1, s2):     Tests that s1 != s2
//    * {ASSERT|EXPECT}_STRCASEEQ(s1, s2): Tests that s1 == s2, ignoring case
//    * {ASSERT|EXPECT}_STRCASENE(s1, s2): Tests that s1 != s2, ignoring case
//

// Macros for comparing floating-point numbers.
//
//    * {ASSERT|EXPECT}_FLOAT_EQ(val1, val2):
//         Tests that two float values are almost equal.
//    * {ASSERT|EXPECT}_DOUBLE_EQ(val1, val2):
//         Tests that two double values are almost equal.
//    * {ASSERT|EXPECT}_NEAR(v1, v2, abs_error):
//         Tests that v1 and v2 are within the given distance to each other.

#endif

#endif //TINYWORLD_TINYTEST_H
