/* Copyright (C) 2015 Murray Cumming
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef __MURRAYCDP_VECTOR_OF_VECTORS_H
#define __MURRAYCDP_VECTOR_OF_VECTORS_H

#include <vector>
#include <algorithm>
#include <tuple>

/**
 * A type trait for nested std::vectors.
 *
 * This should be similar to standard C++ type traits:
 * http://en.cppreference.com/w/cpp/types
 *
 * For instance, if we do this:
 *   using type_vec = vector_of_vectors<int, 0>::type;
 * then type_vec will be
 *   std::vector<int>> .
 * and if we do this:
 *   using type_vec = vector_of_vectors<int, 1>::type;
 * then type_vec will be
 *   std::vector<std::vector<int>> .
 * and if we do this:
 *   using type_vec = vector_of_vectors<int, 2>::type;
 * then type_vec will be
 *   std::vector<std::vector<std::vector<int>>>.
 *
 * @tparam T The type of the vector's elements.
 * @tparam N The number of levels of nested vectors in the type.
 */
template<class T, std::size_t N>
class vector_of_vectors {
public:
  using type = std::vector<typename vector_of_vectors<T, N-1>::type>;
};
 
template<class T>
class vector_of_vectors<T, 0> {
public:
  using type = std::vector<T>;
};

template<class T, class T_first_size, class... T_sizes>
void resize_vector_of_vectors(std::vector<T>& vector, T_first_size first_size) {
  vector.resize(first_size);
}

template<class T, class T_first_size, class... T_other_sizes>
void resize_vector_of_vectors(std::vector<std::vector<T>>& vector, T_first_size first_size, T_other_sizes... other_sizes) {
  vector.resize(first_size);

  std::for_each(vector.begin(), vector.end(),
    [other_sizes...](auto& item) {
      resize_vector_of_vectors(item, other_sizes...);
    }
  );
}

namespace {

/// Call get_goal_cell(a, b, c, d) with std::tuple<a, b, c, d>
template<class T_function, class T_tuple, std::size_t... Is>
void call_with_tuple(T_function f,
  const T_tuple& tuple, std::index_sequence<Is...>) {
  f(std::get<Is>(tuple)...);
}

template<class T, class T_function, class T_tuple_indices, class T_first_size_start, class T_first_size_end>
void for_vector_of_vectors_with_indices(std::vector<T>& /* vector */, T_function f, const T_tuple_indices& indices, T_first_size_start start, T_first_size_end end) {
  for (T_first_size_start i = start; i < end; ++i) {
    const std::tuple<T_first_size_start> index_i(i);
    const auto indices_with_i = std::tuple_cat(indices, index_i);

    constexpr std::size_t tuple_size = std::tuple_size<T_tuple_indices>::value + 1;
    call_with_tuple(f, indices_with_i,
      std::make_index_sequence<tuple_size>());
  }
}

template<class T, class T_function, class T_tuple_indices, class T_first_size_start, class T_first_size_end, class... T_other_sizes>
void for_vector_of_vectors_with_indices(std::vector<std::vector<T>>& vector, T_function f, const T_tuple_indices& indices, T_first_size_start start, T_first_size_end end, T_other_sizes... other_sizes) {
  for (T_first_size_start i = start; i < end; ++i) {
    const std::tuple<T_first_size_start> index_i(i);
    const auto indices_with_i = std::tuple_cat(indices, index_i);
    for_vector_of_vectors_with_indices(vector[i], f, indices_with_i, other_sizes...);
  }
}

} //anonymous namespace

/**
 * Call @a f on each item in the vector.
 *
 * This is equivalent to:
 *   for (std::size_t i = start; i < end; ++i) {
 *     f(vector[i]);
 *   }
 *
 * @param start The start of the range.
 * @param end One past the end of the range.
 */
template<class T, class T_function, class T_first_size_start, class T_first_size_end>
void for_vector_of_vectors(std::vector<T>& /* vector */, T_function f, T_first_size_start start, T_first_size_end end) {
  for (T_first_size_start i = start; i < end; ++i) {
    f(i);
  }
}

template<class T, class T_function, class T_first_size_start, class T_first_size_end, class... T_other_sizes>
void for_vector_of_vectors(std::vector<std::vector<T>>& vector, T_function f, T_first_size_start start, T_first_size_end end, T_other_sizes... other_sizes) {

  for (T_first_size_start i = start; i < end; ++i) {
    const std::tuple<T_first_size_start> index_i(i);
    for_vector_of_vectors_with_indices(vector[i], f, index_i, other_sizes...);
  }
}





#endif //__MURRAYCDP_VECTOR_OF_VECTORS_H
