// testlib.h
//

#include <sstream>
#include <iostream>

using namespace std;

namespace testing {

  namespace detail {

    template<typename T>
    inline void print_value (std::ostream& out, const T& v) {
      out << v;
    }

    template<>
    inline void print_value <unsigned long> (std::ostream& out, const unsigned long& v) {
      out << "0x" << std::hex << v << std::dec;
    }

    template<>
    inline void print_value <unsigned int> (std::ostream& out, const unsigned int& v) {
      out << "0x" << std::hex << v << std::dec;
    }

    template<>
    inline void print_value <unsigned short> (std::ostream& out, const unsigned short& v) {
      out << "0x" << std::hex << v << std::dec;
    }

    template<>
    inline void print_value <unsigned char> (std::ostream& out, const unsigned char& v) {
      out << "0x" << std::hex << (unsigned short)v << std::dec;
    }

    inline void print_to_stream (std::ostream&) {}

    template<typename Head, typename...Tail>
    void print_to_stream (std::ostream& stream, const Head& h, const Tail&... t) {
      stream << h;
      print_to_stream(stream, t...);
    }

  } // namespace detail

  template<typename T1, typename T2, typename ... Arguments>
  void create_error_message (std::ostream& os,
                             const T1& testValue,
                             const T2& expectedValue,
                             const char* testName,
                             const char* expectedName,
                             const char* equality,
                             const char* fileName,
                             const int   lineNumber,
                             const Arguments... args) {
    os << fileName << ":" << lineNumber << ": "
       << "Expected " << testName << " "
       << "to be " << equality << " '";
    detail::print_value(os, expectedValue);
    os << "' (" << expectedName << ") but it was '";
    detail::print_value(os, testValue);
    os << "'";
    detail::print_to_stream(os, args...);
  }

  template<typename T1, typename T2, typename ... Arguments>
  void throw_error (const T1& testValue,
                    const T2& expectedValue,
                    const char* testName,
                    const char* expectedName,
                    const char* equality,
                    const char* fileName,
                    const int   lineNumber,
                    const Arguments... args) {
    std::ostringstream os;
    create_error_message(os, testValue, expectedValue, testName, expectedName,
                         equality, fileName, lineNumber, args...);
    throw std::runtime_error(os.str());
  }

  template<typename T1, typename T2, typename ... Arguments>
  void log_err (const T1& testValue,
                  const T2& expectedValue,
                  const char* testName,
                  const char* expectedName,
                  const char* equality,
                  const char* fileName,
                  const int   lineNumber,
                  const Arguments... args) {
    create_error_message(std::cerr, testValue, expectedValue,
                         testName, expectedName, equality,
                         fileName, lineNumber, args...);
  }

  template<typename T1, typename T2>
  bool equal_test (const T1& testValue,
                   const T2& expectedValue) {
    return testValue == expectedValue;
  }

  template<typename T1, typename T2>
  bool lower_test (const T1& testValue,
                   const T2& expectedValue) {
    return testValue < expectedValue;
  }

  template<typename T1, typename T2>
  bool lower_equal_test (const T1& testValue,
                         const T2& expectedValue) {
    return testValue <= expectedValue;
  }

  template<typename T1, typename T2>
  bool higher_test (const T1& testValue,
                    const T2& expectedValue) {
    return testValue > expectedValue;
  }

  template<typename T1, typename T2>
  bool higher_equal_test (const T1& testValue,
                          const T2& expectedValue) {
    return testValue >= expectedValue;
  }

  template<>
  bool equal_test (const double& testValue,
                   const double& expectedValue);

  std::string string_from_file (const char* filename);

  typedef void (test_function)();

} // namespace testing

/**
 * @brief test_main
 *
 * Has to be defined by user.
 */
void test_main ();

void run_test_ (const std::string& name, testing::test_function& fn);

#define run_test(a) run_test_(#a, a)

#define EXPECT_EQUAL(test, expect, ...)\
  if (!(testing::equal_test(test, expect))) testing::throw_error(test, expect, #test, #expect, "equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_NOT_EQUAL(test, expect, ...)\
  if (testing::equal_test(test, expect)) testing::throw_error(test, expect, #test, #expect, "not equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_LOWER(test, expect, ...)\
  if (!testing::lower_test(test, expect)) testing::throw_error(test, expect, #test, #expect, "lower than", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_LOWER_OR_EQUAL(test, expect, ...)\
  if (!testing::lower_equal_test(test, expect)) testing::throw_error(test, expect, #test, #expect, "lower or equal than", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_HIGHER(test, expect, ...)\
  if (!testing::higher_test(test, expect)) testing::throw_error(test, expect, #test, #expect, "higher than", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_HIGHER_OR_EQUAL(test, expect, ...)\
  if (!testing::higher_equal_test(test, expect)) testing::throw_error(test, expect, #test, #expect, "higher or equal than", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_TRUE(test, ...)\
  if (!testing::equal_test<bool, bool>(test, true)) testing::throw_error<bool, bool>(test, true, #test, "true", "equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define EXPECT_FALSE(test, ...)\
  if (!testing::equal_test<bool, bool>(test, false)) testing::throw_error<bool, bool>(test, false, #test, "false", "equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_EQUAL(test, expect, ...)\
  if (!(testing::equal_test(test, expect))) testing::log_err(test, expect, #test, #expect, "equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_NOT_EQUAL(test, expect, ...)\
  if (testing::equal_test(test, expect)) testing::log_err(test, expect, #test, #expect, "not equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_LOWER(test, expect, ...)\
  if (!testing::lower_test(test, expect)) testing::log_err(test, expect, #test, #expect, "lower than", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_LOWER_OR_EQUAL(test, expect, ...)\
  if (!testing::lower_equal_test(test, expect)) testing::log_err(test, expect, #test, #expect, "lower or equal than", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_HIGHER(test, expect, ...)\
  if (!testing::higher_test(test, expect)) testing::log_err(test, expect, #test, #expect, "higher than", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_HIGHER_OR_EQUAL(test, expect, ...)\
  if (!testing::higher_equal_test(test, expect)) testing::log_err(test, expect, #test, #expect, "higher or equal than", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_TRUE(test, ...)\
  if (!testing::equal_test<bool, bool>(test, true)) testing::log_err<bool, bool>(test, true, #test, "true", "equal", __FILE__, __LINE__, ##__VA_ARGS__);

#define TEST_FALSE(test, ...)\
  if (!testing::equal_test<bool, bool>(test, false)) testing::log_err<bool, bool>(test, false, #test, "false", "equal", __FILE__, __LINE__, ##__VA_ARGS__);
