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

#ifndef _MURRAYCDP_DP__BASE_H
#define _MURRAYCDP_DP__BASE_H

#include <iostream>
#include <tuple>
#include <utility>


//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for dynamic programming algorithms.
 *
 * See DpBottomUpBase and DpTopDownBase.
 *
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <typename T_subproblem, typename... T_value_types>
class DpBase {
public:
  using type_subproblem = T_subproblem;
  using type_level = unsigned int;
  using type_values = std::tuple<T_value_types...>;

  /**
   * @param i The number of i values to calculate the subproblem for.
   * @param j The number of j values to calculate the subproblem for.
   */
  DpBase()
  {}

  DpBase(const DpBase& src) = delete;
  DpBase& operator=(const DpBase& src) = delete;

  DpBase(DpBase&& src) = delete;
  DpBase& operator=(DpBase&& src) = delete;

  virtual type_subproblem calc() = 0;

protected:
  /** Calculate the subproblem solution.
   * This will only be called when the subproblem solution has not yet been calculated.
   * The implementation dos not need to check any cache first,
   * and does not need to store the result in any cache.
   *
   * Implementations should call get_subproblem() to get related sub-problem results.
   */
  virtual type_subproblem calc_subproblem(type_level level, T_value_types... values) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(T_value_types&... values) const = 0;

  /// Call get_goal_cell(a, b, c, d) with std::tuple<a, b, c, d>
  template<std::size_t... Is>
  void get_goal_cell_call_with_tuple(
    type_values& goals,
    std::index_sequence<Is...>) {
    get_goal_cell(std::get<Is>(goals)...);
  }
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
