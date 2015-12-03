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

#ifndef __MURRAYCDP_TUPLE_CDR_H
#define __MURRAYCDP_TUPLE_CDR_H

#include <tuple>
#include <utility>
#include <type_traits>

namespace murraycdp {
namespace utils{

//Based on this:
//http://stackoverflow.com/a/14854294/1123654

namespace {

template<typename T, typename Seq>
struct tuple_cdr_impl;

template<typename T, std::size_t I0, std::size_t... I>
struct tuple_cdr_impl<T, std::index_sequence<I0, I...>>
{
  using type = std::tuple<typename std::tuple_element<I, T>::type...>;
};

} //anonymous namespace

/**
 * Get the type of a tuple without the first item.
 */
template<typename T>
struct tuple_cdr
: tuple_cdr_impl<T, std::make_index_sequence<std::tuple_size<T>::value>>
{};

template<typename T, std::size_t I0, std::size_t... I>
typename tuple_cdr<T>::type
cdr_impl(const T& t, std::index_sequence<I0, I...>) {
  return std::make_tuple(std::get<I>(t)...);
}

/**
 * Get the a tuple without the first item.
 */
template<typename T>
typename tuple_cdr<T>::type
cdr(const T& t) {
  constexpr auto size = std::tuple_size<T>::value;
  const auto seq = std::make_index_sequence<size>{};
  return cdr_impl(t, seq);
}


} //namespace utils
} //namespace murraycdp

#endif //__MURRAYCDP_TUPLE_CDR_H