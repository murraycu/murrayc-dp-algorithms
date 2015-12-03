#include <cstdlib>
#include <cassert>
#include <utility>
//#include <iostream>
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

void test_for0() {
  using type_vec0 = vector_of_vectors<int, 0>::type;
  type_vec0 vec0;
  resize_vector_of_vectors(vec0, 10);

  std::size_t n = 0;
  for_vector_of_vectors(vec0,
    [&n] (int i) {
      n += i;
    },
    (std::size_t)0, (std::size_t)10);
  //std::cout << "n=" << n << std::endl;
  assert(n == 45);
}

void test_for1() {
  using type_vec1 = vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  resize_vector_of_vectors(vec1, 10, 20);

  std::size_t n = 0;
  for_vector_of_vectors(vec1,
    [&n] (int i) {
      n += i;
    },
    (std::size_t)0, (std::size_t)10,
    (std::size_t)0, (std::size_t)20);
  //std::cout << "n=" << n << std::endl;
  assert(n == 1900);
}

void test_for2() {
  using type_vec2 = vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  resize_vector_of_vectors(vec2, 10, 20, 30);

  std::size_t n = 0;
  for_vector_of_vectors(vec2,
    [&n] (int i) {
      n += i;
    },
    (std::size_t)0, (std::size_t)10,
    (std::size_t)0, (std::size_t)20,
    (std::size_t)0, (std::size_t)30);
  //std::cout << "n=" << n << std::endl;
  assert(n == 87000);
}

void test_for() {
  test_for0();
  test_for1();
  test_for2();
}

int main() {
  test_type();
  test_resize();
  test_for();

  return EXIT_SUCCESS;
}
