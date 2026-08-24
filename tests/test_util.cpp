// library includes
#include <gtest/gtest.h>

// local includes
#include "../src/util.h"

TEST(Util, ShellJoin) {
    ASSERT_EQ(shellJoin({"test"}), "test");
    ASSERT_EQ(shellJoin({"test", "arg"}), "test arg");
    ASSERT_EQ(shellJoin({"test", "$arg"}), "test '$arg'");
    ASSERT_EQ(shellJoin({"test", "ar`"}), "test 'ar`'");
    ASSERT_EQ(shellJoin({"!"}), "'!'");

    ASSERT_EQ(shellJoin({"test", "abc'def"}), R"--(test 'abc'"'"'def')--");
}
