#include <cstdlib>
#include <cassert>
#include <utility>
//#include <iostream>
#include <murraycdp/utils/tuple_interlace.h>


int main() {
  using type_tuple_is = std::tuple<int, short>;
  using type_tuple_dc = std::tuple<double, char>;
  using type_tuple_idsc = std::tuple<int, double, short, char>;

  using type_tuple_interlaced =
    murraycdp::utils::tuple_interlace<type_tuple_is, type_tuple_dc>::type;

  static_assert(std::is_same<type_tuple_interlaced, type_tuple_idsc>::value,
      "unexpected tuple_inerlace()ed tuple type");

  return EXIT_SUCCESS;
}
