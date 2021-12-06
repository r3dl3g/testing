# C++ test framework

Testing is a small c++ test framework.

## Motivation

Goal was to have a independent test framework that makes use of modern c++ (c++11),
that can easily integrated into own code and into own cmake project files.

## Usage

### In cmake

In your CMakeLists.txt include this project directory, enable cmake testing,
and add the library testing to your target:

```cmake

add_subdirectory(testing)
enable_testing()

add_executable(MyTest MyTest.cpp)
target_link_libraries(MyTest testing)
add_test(MyTest ${CMAKE_CURRENT_BINARY_DIR}/MyTest)

```

### In C++ code

To write a test, first include testing.h.

```c++

#include <testing/testing.h>

```

Then write your test method.
You can use EXPECT_EQUAL/NOT_EQUAL/LOWER/LOWER_OR_EQUAL/HIGHER/HIGHER_OR_EQUAL/TRUE/FALSE to
check your results against an expected value.
Optional you can give one or more additional params, to be logged when the result was unexpected.
The optional params will be appended by ostream (<<) to the error message.

```c++

void my_test () {

  int i = 2 * 2;

  EXPECT_EQUAL(i, 4, "The calculated value was '", i, "'!");
  EXPECT_NOT_EQUAL(i, 0);
  EXPECT_TRUE(i > 0);

}

```

Then all written test have to be called in a test_main function capsuled by 'run_test'.

```c++

void test_main (const testing::start_params& params) {
  testing::log_info("Running test " __FILE__);

  run_test(my_test);

}

```

## Additional

Instead of EXPECT_, all test can also be done with the prefix TEST_.

This will perform the same check, but will in case of failure only log the
failure but ot count it as a failed test.

```c++

void my_test () {

  int i = 2 * 2;

  TEST_EQUAL(i, 4, "The calculated value was '", i, "'!");
  TEST_NOT_EQUAL(i, 0);
  TEST_TRUE(i > 0);

}

```
