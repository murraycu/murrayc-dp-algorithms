#include <cstdlib>
#include <cassert>
#include <utility>
#include <murraycdp/vector_of_vectors.h>

void test_type() {
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
}

void test_resize() {
  using type_vec0 = vector_of_vectors<int, 0>::type;
  type_vec0 vec0;
  resize_vector_of_vectors(vec0, 10);
  assert(vec0.size() == 10);

  using type_vec1 = vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  resize_vector_of_vectors(vec1, 10, 20);
  assert(vec1.size() == 10);
  assert(vec1[0].size() == 20);

  using type_vec2 = vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  resize_vector_of_vectors(vec2, 10, 20, 30);
  assert(vec2.size() == 10);
  assert(vec2[0].size() == 20);
  assert(vec2[0][0].size() == 30);
}

int main() {
  test_type();

  test_resize();

  return EXIT_SUCCESS;
}
