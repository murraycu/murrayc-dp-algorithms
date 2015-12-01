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

#ifndef _MURRAYCDP_TUPLE_HASH_H
#define _MURRAYCDP_TUPLE_HASH_H

#include <tuple>

//Based on this: http://stackoverflow.com/a/7115547/1123654
namespace hash_tuple {

template <typename T_tuple>
class hash {
public:
  size_t
  operator()(const T_tuple& tuple) const {                                              
    return std::hash<T_tuple>()(tuple);                                 
  }                                              
};

namespace  {

template <class T_tuple>
inline void hash_combine(std::size_t& seed, const T_tuple& v) {
  seed ^= hash_tuple::hash<T_tuple>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
class HashValueImpl {
public:
  static void apply(size_t& seed, const Tuple& tuple) {
    HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
    hash_combine(seed, std::get<Index>(tuple));
  }
};

template <class Tuple>
class HashValueImpl<Tuple, 0>
{
public:
  static void apply(size_t& seed, const Tuple& tuple) {
    hash_combine(seed, std::get<0>(tuple));
  }
};
    
} //anonymous namespace

template <typename... T_elements>
class hash<std::tuple<T_elements...>>  {
public:
  size_t operator()(const std::tuple<T_elements...>& tt) const {                                              
    size_t seed = 0;                             
    HashValueImpl<std::tuple<T_elements...> >::apply(seed, tt);    
    return seed;                                 
  }                                              
};

} //namespace hash_tuple

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
