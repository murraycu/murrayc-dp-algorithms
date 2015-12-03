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



#endif //__MURRAYCDP_VECTOR_OF_VECTORS_H