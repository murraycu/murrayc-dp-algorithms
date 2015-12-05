#include <cstdlib>
#include <cassert>
#include <utility>
//#include <iostream>
#include <murraycdp/utils/vector_of_vectors.h>

void test_type() {
  //The preprocessor doesn't like having it all inside the assert() macro.
  //It complains about there being 2 arguments instead of 1.
  //So we do it like this:

  using type_vec0 = murraycdp::utils::vector_of_vectors<int, 0>::type;
  constexpr auto is_same0 = std::is_same<type_vec0, int>::value;
  static_assert(is_same0,
    "unexpected type");

  using type_vec1 = murraycdp::utils::vector_of_vectors<int, 1>::type;
  constexpr auto is_same1 = std::is_same< type_vec1,
    std::vector<int> >::value;
  static_assert(is_same1,
    "unexpected type");

  using type_vec2 = murraycdp::utils::vector_of_vectors<int, 2>::type;
  constexpr auto is_same2 = std::is_same<type_vec2,
    std::vector<std::vector<int>> >::value;
  static_assert(is_same2,
    "unexpected type");

  using type_vec3= murraycdp::utils::vector_of_vectors<int, 3>::type;
  constexpr auto is_same3 = std::is_same< type_vec3,
    std::vector<std::vector<std::vector<int>>> >::value;
  static_assert(is_same3,
    "unexpected type");
}

void test_resize() {
  using type_vec0 = murraycdp::utils::vector_of_vectors<int, 0>::type;
  type_vec0 vec0 = type_vec0(); //This will be an int.
  murraycdp::utils::resize_vector_of_vectors(vec0);
  //assert(vec1.size() == 10);

  using type_vec1 = murraycdp::utils::vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  murraycdp::utils::resize_vector_of_vectors(vec1, 10);
  assert(vec1.size() == 10);

  using type_vec2 = murraycdp::utils::vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  murraycdp::utils::resize_vector_of_vectors(vec2, 10, 20);
  assert(vec2.size() == 10);
  assert(vec2[0].size() == 20);

  using type_vec3 = murraycdp::utils::vector_of_vectors<int, 3>::type;
  type_vec3 vec3;
  murraycdp::utils::resize_vector_of_vectors(vec3, 10, 20, 30);
  assert(vec3.size() == 10);
  assert(vec3[0].size() == 20);
  assert(vec3[0][0].size() == 30);
}

void test_get_at() {
  //This will actuall just be int:
  using type_vec0 = murraycdp::utils::vector_of_vectors<int, 0>::type;
  type_vec0 vec0;
  vec0 = 9;
  assert(vec0 == 9);

  //This will be std::vector<int>
  using type_vec1 = murraycdp::utils::vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  murraycdp::utils::resize_vector_of_vectors(vec1, 10);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec1, 3) = 9;
  const auto val1 = murraycdp::utils::get_at_vector_of_vectors<int>(vec1, 3);
  assert(val1 == 9);

  using type_vec2 = murraycdp::utils::vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  murraycdp::utils::resize_vector_of_vectors(vec2, 10, 20);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec2, 3, 4) = 9;
  const auto val2 = murraycdp::utils::get_at_vector_of_vectors<int>(vec2, 3, 4);
  assert(val2 == 9);

  using type_vec3 = murraycdp::utils::vector_of_vectors<int, 3>::type;
  type_vec3 vec3;
  murraycdp::utils::resize_vector_of_vectors(vec3, 10, 20, 30);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec3, 3, 4, 5) = 9;
  const auto val3 = murraycdp::utils::get_at_vector_of_vectors<int>(vec3, 3, 4, 5);
  assert(val3 == 9);
}

void test_get_at_const() {
  using type_vec1 = murraycdp::utils::vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  murraycdp::utils::resize_vector_of_vectors(vec1, 10);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec1, 3) = 9;
  const type_vec1 vec1_const = vec1;
  const auto val1 = murraycdp::utils::get_at_vector_of_vectors<int>(vec1_const, 3);
  assert(val1 == 9);

  using type_vec2 = murraycdp::utils::vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  murraycdp::utils::resize_vector_of_vectors(vec2, 10, 20);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec2, 3, 4) = 9;
  const type_vec2& vec2_const = vec2;
  const auto val2 = murraycdp::utils::get_at_vector_of_vectors<int>(vec2_const, 3, 4);
  assert(val2 == 9);

  using type_vec3 = murraycdp::utils::vector_of_vectors<int, 3>::type;
  type_vec3 vec3;
  murraycdp::utils::resize_vector_of_vectors(vec3, 10, 20, 30);
  murraycdp::utils::get_at_vector_of_vectors<int>(vec3, 3, 4, 5) = 9;
  const type_vec3& vec3_const = vec3;
  const auto val3 = murraycdp::utils::get_at_vector_of_vectors<int>(vec3_const, 3, 4, 5);
  assert(val3 == 9);
}

void test_for1() {
  using type_vec1 = murraycdp::utils::vector_of_vectors<int, 1>::type;
  type_vec1 vec1;
  murraycdp::utils::resize_vector_of_vectors(vec1, 10);

  std::size_t n = 0;
  murraycdp::utils::for_vector_of_vectors(vec1,
    [&n] (int i) {
      n += i;
    },
    (std::size_t)0, (std::size_t)10);
  //std::cout << "n=" << n << std::endl;
  assert(n == 45);
}

void test_for2() {
  using type_vec2 = murraycdp::utils::vector_of_vectors<int, 2>::type;
  type_vec2 vec2;
  murraycdp::utils::resize_vector_of_vectors(vec2, 10, 20);

  std::size_t n = 0;
  murraycdp::utils::for_vector_of_vectors(vec2,
    [&n] (int i, int j) {
      n += (i + j);
    },
    (std::size_t)0, (std::size_t)10,
    (std::size_t)0, (std::size_t)20);
  //std::cout << "n=" << n << std::endl;
  assert(n == 2800);
}

void test_for3() {
  using type_vec3 = murraycdp::utils::vector_of_vectors<int, 3>::type;
  type_vec3 vec3;
  murraycdp::utils::resize_vector_of_vectors(vec3, 10, 20, 30);

  std::size_t n = 0;
  murraycdp::utils::for_vector_of_vectors(vec3,
    [&n] (int i, int j, int k) {
      n += (i + j + k);
    },
    (std::size_t)0, (std::size_t)10,
    (std::size_t)0, (std::size_t)20,
    (std::size_t)0, (std::size_t)30);
  //std::cout << "n=" << n << std::endl;
  assert(n == 171000);
}

void test_for() {
  test_for1();
  test_for2();
  test_for3();
}

int main() {
  test_type();
  test_resize();
  test_get_at();
  test_get_at_const();
  test_for();

  return EXIT_SUCCESS;
}
