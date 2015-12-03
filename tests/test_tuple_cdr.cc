#include <cstdlib>
#include <cassert>
#include <utility>
//#include <iostream>
#include <murraycdp/utils/tuple_cdr.h>

void test_cdr() {
  auto t_larger = std::make_tuple(nullptr, std::string("hello"),
    std::string("world"));
  auto t_suffix = murraycdp::utils::cdr<decltype(t_larger)>(t_larger);
  assert(std::get<0>(t_suffix) == "hello");
  assert(std::get<1>(t_suffix) == "world");
}

int main() {
  test_cdr();

  return EXIT_SUCCESS;
}
