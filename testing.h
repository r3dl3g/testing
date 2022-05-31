/**
* @copyright (c) 2020-2021 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    testing lib
*
* @brief     Small c++ testing framework.
*
* @license   MIT license. See accompanying file LICENSE.txt
*
*/


#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <regex>
#ifdef WIN32
#undef NOMINMAX
#define NOMINMAX
#include <windows.h>
#endif // WIN32

#include <util/vector_util.h>


using namespace std;

namespace testing {

  namespace detail {

    template<typename T>
    struct print {
      static inline void value (std::ostream& os, const T& v) {
        os << v;
      }
    };

    template<typename T>
    void print_value (std::ostream& os, const T& v) {
      print<std::remove_const<T>::type>::value(os, v);
    }

    template<typename T>
    std::ostream& print_vector_or_array (std::ostream& os, const T& v);

    template<typename T>
    std::ostream& print_ptr (std::ostream& os, const T& v);

    template<typename T>
    struct print<std::vector<T>> {
      static inline void value (std::ostream& os, const std::vector<T>& v) {
        print_vector_or_array(os << "vector:", v);
      }
    };

    template<typename T, std::size_t S>
    struct print<std::array<T, S>> {
      static inline void value(std::ostream& os, const std::array<T, S>& v) {
        print_vector_or_array(os << "array:", a);
      }
    };

    template<typename T>
    struct print<std::unique_ptr<T>> {
      static inline void value (std::ostream& os, const std::unique_ptr<T>& p) {
        print_ptr(os << "unique_ptr:", p);
      }
    };

    template<typename T>
    struct print<std::shared_ptr<T>> {
      static inline void value (std::ostream& os, const std::shared_ptr<T>& p) {
        print_ptr(os << "shared_ptr:", p);
      }
    };

    template<typename K, typename V, typename C>
    struct print<std::map<K, V, C>> {
      static inline void value (std::ostream& os, const std::map<K, V, C>& m) {
        os << "map:{";
        for (const auto& e : m) {
          print_value(os, e.first);
          os << "=";
          print_value(os, e.second);
        }
        os << "}";
      }
    };

    template<typename T1, typename T2>
    struct print< std::pair<T1, T2>> {
      static inline void value (std::ostream& os, const std::pair<T1, T2>& m) {
        os << "pair:[";
        print_value(os, m.first);
        os << ",";
        print_value(os, m.second);
        os << "]";
      }
    };

    template<>
    struct print<std::string> {
      static inline void value (std::ostream& os, const std::string& m) {
        os << "\"" << m << "\"";
      }
    };

    template<>
    struct print<char*> {
      static inline void value (std::ostream& os, char const* m) {
        os << "\"" << m << "\"";
      }
    };

    template<>
    struct print<unsigned long> {
      static inline void value (std::ostream& out, const unsigned long& v) {
        out << "0x" << std::hex << v << std::dec;
      }
    };

    template<>
    struct print<unsigned int> {
      static inline void value (std::ostream& out, const unsigned int& v) {
        out << "0x" << std::hex << v << std::dec;
      }
    };

    template<>
    struct print<unsigned short> {
      static inline void value (std::ostream& out, const unsigned short& v) {
        out << "0x" << std::hex << v << std::dec;
      }
    };

    template<>
    struct print<unsigned char> {
      static inline void value (std::ostream& out, const unsigned char& v) {
        out << "0x" << std::hex << (int)v << std::dec;
      }
    };

    template<typename T>
    std::ostream& print_vector_or_array (std::ostream& os, const T& v) {
      os << "[";
      bool first = true;
      for (const auto& i : v) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        print_value(os, i);
      }
      return os << "]";
    }

    template<typename T>
    std::ostream& print_ptr (std::ostream& os, const T& p) {
      if (p) {
        os << *p;
      } else {
        os << "null";
      }
      return os;
    }

    inline void print_to_stream (std::ostream&) {}

    template<typename Head, typename...Tail>
    void print_to_stream (std::ostream& stream, const Head& h, const Tail&... tail) {
      stream << h;
      print_to_stream(stream, tail...);
    }

  } // namespace detail

  void set_test_init (std::function<void()>);
  void set_test_fini (std::function<void()>);

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

  typedef void (log_target) (const std::string&);
  void set_error_log (std::function<log_target>);
  void set_info_log (std::function<log_target>);

  void log_error (const std::string&);
  void log_info (const std::string&);

  template<typename T1, typename T2, typename ... Arguments>
  void log_err (const T1& testValue,
                  const T2& expectedValue,
                  const char* testName,
                  const char* expectedName,
                  const char* equality,
                  const char* fileName,
                  const int   lineNumber,
                  const Arguments... args) {
    std::ostringstream buffer;
    create_error_message(buffer, testValue, expectedValue,
                         testName, expectedName, equality,
                         fileName, lineNumber, args...);
    log_error(buffer.str());
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

  struct start_params {
    std::vector<std::string> args;
  #ifdef WIN32
    HINSTANCE hInstance;
  #endif // WIN32
  };

  inline bool regex_test (const std::string& testValue,
                          const std::string& regexExpression) {
    return std::regex_match(testValue, std::regex(regexExpression));
  }

} // namespace testing

/**
 * @brief test_main
 *
 * Has to be defined by user.
 */
void test_main (const testing::start_params&);

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

#define EXPECT_REGEX(test, expect, ...)\
  if (!(testing::regex_test(test, expect))) testing::throw_error(test, expect, #test, #expect, "regex match", __FILE__, __LINE__, ##__VA_ARGS__);

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

#define TEST_REGEX(test, expect, ...)\
  if (!(testing::regex_test(test, expect))) testing::log_err(test, expect, #test, #expect, "regex match", __FILE__, __LINE__, ##__VA_ARGS__);

