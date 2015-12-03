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

namespace murraycdp {
namespace utils{

/**
 * A type trait for nested std::vectors.
 *
 * This should be similar to standard C++ type traits:
 * http://en.cppreference.com/w/cpp/types
 *
 * For instance, if we do this:
 * @code
 *   using type_vec = vector_of_vectors<int, 0>::type;
 * @endcode
 * then type_vec will be @c std::vector<int> .
 * And if we do this:
 * @code
 *   using type_vec = vector_of_vectors<int, 1>::type;
 * @endcode
 * then type_vec will be @c std::vector<std::vector<int>> .
 * And if we do this:
 * @code
 *   using type_vec = vector_of_vectors<int, 2>::type;
 * @endcode
 * then type_vec will be @c std::vector<std::vector<std::vector<int>>> .
 *
 * @tparam T The type of the vector's elements.
 * @tparam N The number of levels of nested vectors in the type.
 */
template<class T, std::size_t N>
class vector_of_vectors {
public:
  using type = std::vector<typename vector_of_vectors<T, N-1>::type>;
};

/// @cond DOXYGEN_HIDDEN_SYMBOLS
template<class T>
class vector_of_vectors<T, 0> {
public:
  using type = std::vector<T>;
};
/// @endcond DOXYGEN_HIDDEN_SYMBOLS


/**
 * Resize a vector.
 *
 * @tparam T The type of the vector.
 * @tparam T_first_size The type of the size, for @a first_size.
 * @param first_size The size with which to call resize() on the vector itself.
 */
template<class T, class T_first_size, class... T_other_sizes>
void resize_vector_of_vectors(std::vector<T>& vector, T_first_size first_size) {
  vector.resize(first_size);
}

/**
 * Resize a vector of vectors.
 * For instance, when calling this on a std::vector<std::vector<std::vector<int>>>,
 * this is equivalent to:
 * @code
 *   vector.resize(a);
 *
 *   for (auto& item : vector) {
 *     item.resize(b);
 *     for (auto& inner_item : vector) {
 *       inner_item.resize(c);
 *     }
 *   }
 * @endcode
 *
 * @tparam T The type of the vector.
 * @tparam T_first_size The type of the size, for @a first_size.
 * @param first_size The size with which to call resize() on the vector itself.
 * @param other_sizes The sizes with which to call resize() on the vector's nested items.
 */
template<class T, class T_first_size, class... T_other_sizes>
void resize_vector_of_vectors(std::vector<std::vector<T>>& vector, T_first_size first_size, T_other_sizes... other_sizes) {
  vector.resize(first_size);

  std::for_each(vector.begin(), vector.end(),
    [other_sizes...](auto& item) {
      resize_vector_of_vectors(item, other_sizes...);
    }
  );
}


/**
 * Get an element from a vector.
 *
 * @tparam T_element the value_type of the innermost vector.
 * @tparam T The type of the vector.
 * @tparam T_first_index The type of the @a first_index parameter.
 * @tparam T_other_indices The types of the @a other_indices parameters.
 * @param first_index The top-level index.
 */
template<class T_element, class T, class T_first_index, class... T_other_sizes>
T_element& get_at_vector_of_vectors(std::vector<T>& vector, T_first_index first_index) {
  return vector[first_index];
}

/**
 * Get an element from a vector of vectors.
 * For instance, with a std::vector<std::vector<std::vector<int>>>,
 * calling this
 * @code
 * int val = get_at_vector_of_vectors<int>(vec, 1, 2, 3);
 * @endcode
 * is equivalent to:
 * @code
 * int val = vec[1, 2, 3];
 * @endcode
 *
 * @tparam T_element the value_type of the innermost vector.
 * @tparam T The type of the vector.
 * @tparam T_first_index The type of the @a first_index parameter.
 * @tparam T_other_indices The types of the @a other_indices parameters.
 * @param first_index The top-level index.
 * @param other_indices The other indices.
 */
template<class T_element, class T, class T_first_index, class... T_other_indices>
T_element& get_at_vector_of_vectors(std::vector<std::vector<T>>& vector, T_first_index first_index, T_other_indices... other_indices) {
  auto& sub = vector[first_index];
  return get_at_vector_of_vectors<T_element>(sub, other_indices...);
}


namespace {

/// Call f(a, b, c, d) with std::tuple<a, b, c, d>
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
 * Call @a f on a range of items in the vector.
 *
 * This is equivalent to:
 * @code
 *   for (std::size_t i = start; i < end; ++i) {
 *     f(i);
 *   }
 * @endcode
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

/**
 * Call @a f on a range of items in the vector of vectors.
 *
 * For instance, when calling this on a std::vector<std::vector<std::vector<int>>>,
 * this is equivalent to:
 * @code
 *   for (std::size_t i = i_start; i < i_end; ++i) {
 *     for (std::size_t j = j_start; i < j_end; ++i) {
 *       for (std::size_t k = k_start; i < k_end; ++i) {
 *         f(i, j, k);
 *       }
 *     }
 *   }
 * @endcode
 *
 * @param start The start of the range.
 * @param end One past the end of the range.
 */
template<class T, class T_function, class T_first_size_start, class T_first_size_end, class... T_other_sizes>
void for_vector_of_vectors(std::vector<std::vector<T>>& vector, T_function f, T_first_size_start start, T_first_size_end end, T_other_sizes... other_sizes) {

  for (T_first_size_start i = start; i < end; ++i) {
    const std::tuple<T_first_size_start> index_i(i);
    for_vector_of_vectors_with_indices(vector[i], f, index_i, other_sizes...);
  }
}

} //namespace utils
} //namespace murraycdp

#endif //__MURRAYCDP_VECTOR_OF_VECTORS_H
