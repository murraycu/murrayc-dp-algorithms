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
#include <list>
#include <tuple>
#include <utility>
#include <murraycdp/utils/tuple_printer.h>

namespace murraycdp {

//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for dynamic programming algorithms.
 *
 * See DpBottomUpBase and DpTopDownBase.
 *
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 * @tparam T_value_types The types of the parameters for the calc_subproblem() method.
 */
template <typename T_subproblem, typename... T_value_types>
class DpBase {
public:
  using type_subproblem = T_subproblem;
  using type_level = unsigned int;
  using type_values = std::tuple<typename std::decay<T_value_types>::type...>;

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

  void print_subproblem_sequence() const {
    std::size_t i = 0;
    for (const auto subproblem_access : subproblem_accesses_) {
       std::cout << i << ", " <<
         get_string_for_subproblem_access(subproblem_access.second) << ", ";

       utils::TuplePrinter<type_values, sizeof...(T_value_types)>::print(
         subproblem_access.first);

       std::cout << std::endl;
       ++i;
    }
  }

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
  virtual void get_goal_cell(typename std::decay<T_value_types>::type&... values) const = 0;

  virtual bool get_cached_subproblem(type_subproblem& subproblem, T_value_types... values) const = 0;
  virtual void set_subproblem(const type_subproblem& subproblem, T_value_types... values) const = 0;

  virtual void clear() {
    subproblem_accesses_.clear();
  }

  /** Get the subproblem solution from the cache if it is in the cache,
   * or call calc_subproblem() and put it in the cache, then return it.
   *
   * See calc_subproblem().
   */
  type_subproblem get_subproblem(type_level level, T_value_types... values) const {
    type_subproblem result;
    if (get_cached_subproblem(result, values...)) {
      subproblem_accesses_.emplace_back(
        type_values(values...), SubproblemAccess::FROM_CACHE);
    } else {
#if defined MURRAYC_DP_DEBUG_OUTPUT
      indent(level);
      std::cout << "DpBase::get_subproblem(): i=" << i << ", j=" << j << std::endl;
#endif //MURRAYC_DP_DEBUG_OUTPUT
      ++level;
      result = this->calc_subproblem(level, values...);

      set_subproblem(result, values...);

      subproblem_accesses_.emplace_back(
        type_values(values...), SubproblemAccess::CALCULATED);
    }

    return result;
  }

  /// Call get_goal_cell(a, b, c, d) with std::tuple<a, b, c, d>
  template<std::size_t... Is>
  void get_goal_cell_call_with_tuple(
    type_values& goals,
    std::index_sequence<Is...>) {
    get_goal_cell(std::get<Is>(goals)...);
  }

private:
 enum class SubproblemAccess {
    CALCULATED,
    FROM_CACHE
  };

  static std::string get_string_for_subproblem_access(SubproblemAccess enumVal) {
    switch (enumVal) {
      case SubproblemAccess::CALCULATED:
       return "calculated";
      case SubproblemAccess::FROM_CACHE:
       return "from-cache";
      default:
       return "unknown";
    }
  }

  //Keep a record of the order in which each subproblem was calculated:
  using type_subproblem_access = std::pair<type_values, SubproblemAccess>;
  mutable std::list<type_subproblem_access> subproblem_accesses_;
};

} //namespace murraycdp

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
