#include <cstdlib>
#include <cassert>
#include <utility>
#include <murraycdp/vector_of_vectors.h>

int main() {
  
  //The preprocessor doesn't like having it all inside the assert() macro.
  //It complains about there being 2 arguments instead of 1.
  //So we do it like this:
  using type_vec0 = vector_of_vectors<int, 0>::type;
  const auto is_same0 = std::is_same< type_vec0,
    std::vector<int> >::value;
  assert(is_same0);

  using type_vec1 = vector_of_vectors<int, 1>::type;
  const auto is_same1 = std::is_same<type_vec1,
    std::vector<std::vector<int>> >::value;
  assert(is_same1);

  using type_vec2= vector_of_vectors<int, 2>::type;
  const auto is_same2 = std::is_same< type_vec2,
    std::vector<std::vector<std::vector<int>>> >::value;
  assert(is_same2);

  return EXIT_SUCCESS;
}
