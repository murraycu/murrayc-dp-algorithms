#include <cstdlib>
#include <cassert>
#include <utility>
//#include <iostream>
#include <murraycdp/utils/tuple_cdr.h>


void test_tuple_cdr() {
  using type_tuple_isd = std::tuple<int, short, double>;
  using type_tuple_sd = std::tuple<short, double>;
  using type_tuple_suffix = murraycdp::utils::tuple_cdr<type_tuple_isd>::type;

  static_assert(std::is_same<type_tuple_suffix, type_tuple_sd>::value,
      "unexpected tuple_cdr()ed tuple type");
}

void test_cdr() {
  auto t_larger = std::make_tuple(nullptr, std::string("hello"),
    std::string("world"));
  auto t_suffix = murraycdp::utils::cdr<decltype(t_larger)>(t_larger);
  assert(std::get<0>(t_suffix) == "hello");
  assert(std::get<1>(t_suffix) == "world");

  using type_tuple_suffix = std::tuple<std::string, std::string>;
  static_assert(std::is_same<decltype(t_suffix), type_tuple_suffix>::value,
    "unexpected cdr()ed tuple type");
}

int main() {
  test_tuple_cdr();
  test_cdr();

  return EXIT_SUCCESS;
}
