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

}

namespace {
  int guipp_failed_test_count = 0;
  int guipp_test_count = 0;
}

void run_test_ (const std::string& name, testing::test_function& fn) {
  std::cout << name << " started" << std::endl;
  ++guipp_test_count;
  try {
    fn();
    std::cout << name << " passed" << std::endl;

  } catch (std::exception& ex) {
    ++guipp_failed_test_count;
    std::cerr << name << " Test failed with " << ex.what() << std::endl;
  }
}

class delimited : public std::string {};

std::istream& operator>> (std::istream& is, delimited& output) {
  std::getline(is, output, ' ');
  return is;
}

#ifdef X11
int main (int argc, char* argv[]) {
  testing::start_params params = { std::vector<std::string>(argv, argv + argc) };
#endif // X11

#ifdef WIN32
int APIENTRY WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR lpCmdLine, _In_ int) {
  std::istringstream iss(lpCmdLine);
  typedef std::istream_iterator<delimited> iterator;
  testing::start_params params = { std::vector<std::string>(iterator{iss}, iterator{}), hInstance };
#endif // WIN32

  std::cout << "Running tests" << std::endl;

  test_main(params);

  if (guipp_failed_test_count) {
    std::cerr << guipp_failed_test_count << " of " << guipp_test_count << " tests failed" << std::endl;
  } else {
    std::cout << "all " << guipp_test_count << " tests passed" << std::endl;
  }
  return guipp_failed_test_count;
}
