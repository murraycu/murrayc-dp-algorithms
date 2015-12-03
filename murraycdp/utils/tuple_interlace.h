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
//http://stackoverflow.com/a/28182793/1123654

namespace {
template<class T, class... T_tuples>
struct tuple_interlace_ {
  using type = T;
};

template<class... T_result_types, template<class...> class T, class T_first, class... T_tail_types, class... T_tuples>
struct tuple_interlace_<std::tuple<T_result_types...>, T<T_first, T_tail_types...>, T_tuples...>
  : tuple_interlace_<std::tuple<T_result_types..., T_first>, T_tuples..., T<T_tail_types...>>
{};

template<class... T_result_types, template<class...> class T, class T_first, class... T_tuples>
struct tuple_interlace_<std::tuple<T_result_types...>, T<T_first>, T_tuples...>
: tuple_interlace_<std::tuple<T_result_types..., T_first>, T_tuples...>
{};

} //anonymous namespace

template<class... T_tuples>
using tuple_interlace = tuple_interlace_<std::tuple<>, T_tuples...>;

} //namespace utils
} //namespace murraycdp

#endif //__MURRAYCDP_TUPLE_CDR_H
