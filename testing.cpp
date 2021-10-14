// SettingsFile Test: Test for reading and writing the camera settings file
//

#include "testing.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <iterator>

using namespace std;


namespace testing {

  template<>
  bool equal_test (const double& testValue,
                   const double& expectedValue) {
    // doubles may differ below 1.0E-7
    const double diff = fabs(testValue - expectedValue);
    return (diff < 1.0E-7);
  }

  std::string string_from_file (const char* filename) {
    std::ifstream file(filename);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  }

  std::function<log_target> error_log = [] (const std::string& s) {
    std::cerr << s << std::endl;
  };

  std::function<log_target> info_log = [] (const std::string& s) {
    std::cout << s << std::endl;
  };

  void set_error_log (std::function<log_target> t) {
    error_log = t;
  }

  void set_info_log (std::function<log_target> t) {
    info_log = t;
  }

  void log_error (const std::string& s) {
    error_log(s);
  }

  void log_info (const std::string& s) {
    info_log(s);
  }

  std::function<void()> test_init;
  std::function<void()> test_fini;

  void set_test_init (std::function<void()> f) {
    test_init = f;
  }

  void set_test_fini (std::function<void()> f) {
    test_fini = f;
  }

}

namespace {
  int guipp_failed_test_count = 0;
  int guipp_test_count = 0;
}

//struct ostreamfmt {

//};
#define ostreamfmt(a) static_cast<const std::ostringstream&>(std::ostringstream() << a).str()

void run_test_ (const std::string& name, testing::test_function& fn) {
  testing::log_info(ostreamfmt(name << " started"));
  ++guipp_test_count;
  try {
    fn();
    testing::log_info(ostreamfmt(name << " passed"));

  } catch (std::exception& ex) {
    ++guipp_failed_test_count;
    testing::log_error(ostreamfmt(name << " Test failed with " << ex.what()));
  }
}

class delimited : public std::string {};

std::istream& operator>> (std::istream& is, delimited& output) {
  std::getline(is, output, ' ');
  return is;
}

#if X11 || __APPLE__
int main (int argc, char* argv[]) {
  testing::start_params params = { std::vector<std::string>(argv, argv + argc) };
#endif // X11

#ifdef WIN32
int APIENTRY WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR lpCmdLine, _In_ int) {
  // From https://gist.github.com/mojocorp/d1a0b400abd790f44a53
  if (AttachConsole(ATTACH_PARENT_PROCESS)) {
#pragma warning(push)
#pragma warning(disable: 4996)
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#pragma warning(pop)
  }
  std::istringstream iss(lpCmdLine);
  typedef std::istream_iterator<delimited> iterator;
  testing::start_params params = { std::vector<std::string>(iterator{iss}, iterator{}), hInstance };
#endif // WIN32

  if (testing::test_init) {
    testing::test_init();
  }

  testing::log_info(ostreamfmt("Running tests"));

  try {
    test_main(params);
  } catch (std::exception& ex) {
    testing::log_error(ostreamfmt("Test failed with " << ex.what()));
  } catch (...) {
    testing::log_error("Test failed unknown");
  }

  if (guipp_failed_test_count) {
    testing::log_error(ostreamfmt(guipp_failed_test_count << " of " << guipp_test_count << " tests failed"));
  } else {
    testing::log_info(ostreamfmt("all " << guipp_test_count << " tests passed"));
  }

  if (testing::test_fini) {
    testing::test_fini();
  }

  return guipp_failed_test_count;
}
