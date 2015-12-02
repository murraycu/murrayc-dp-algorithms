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

#ifndef _MURRAYCDP_DP_TOP_DOWN_BASE_H
#define _MURRAYCDP_DP_TOP_DOWN_BASE_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <murraycdp/tuple_hash.h>
#include <murraycdp/tuple_printer.h>


//#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D top-down (memoization) dynamic programming algorithm.
 *
 * Override this, implementing calc_subproblem(), and then call calc() to get the
 * overall solution.
 * @tparam T_subproblem The type of the subproblem solution, such as unsigned int,
 * or a custom class containing a value and a partial path.
 */
template <typename T_subproblem, typename... T_value_types>
class DpTopDownBase {
public:
  using type_subproblem = T_subproblem;
  using type_level = unsigned int;
  
  using type_values = std::tuple<T_value_types...>;

  /**
   * @param The number of i values to calculate the subproblem for.
   * @pram The number of j values to calculate the subproblem for.
   */
  DpTopDownBase(T_value_types... value_counts)
  : value_counts_(value_counts...)
  {}

  DpTopDownBase(const DpTopDownBase& src) = delete;
  DpTopDownBase& operator=(const DpTopDownBase& src) = delete;

  DpTopDownBase(DpTopDownBase&& src) = delete;
  DpTopDownBase& operator=(DpTopDownBase&& src) = delete;

  type_subproblem calc() {
    clear();

    //We cannot do this to pass the output parameters to get_goal_cell():
    //  T_type_values... goal
    //but we can pass a std::tuple<> based on T_type_values... 
    //and that will then be passed as individual parameters when we unpack it
    //via std::index_sequence.
    type_values goals;
    get_goal_cell_call_with_tuple(goals,
      std::index_sequence_for<T_value_types...>());
    //std::cout << "calc: " << std::get<0>(goals) << std::endl;

    type_level level = 0;
    return get_subproblem_call_with_tuple(level, goals,
      std::index_sequence_for<T_value_types...>());
  }

  unsigned int count_cached_sub_problems() const {
    return subproblems_.size();
  }

  void print_subproblem_sequence() const {
    for (const auto subproblem_access : subproblem_accesses_) {
       std::cout <<
         get_string_for_subproblem_access(subproblem_access.second) << ": ";

       TuplePrinter<type_values, sizeof...(T_value_types)>::print(
         subproblem_access.first);

       std::cout << std::endl;
    }
  }

protected:
  void clear() {
    subproblems_.clear();

    subproblem_accesses_.clear();
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

public:

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
      std::cout << "DpTopDownBase::get_subproblem(): i=" << i << ", j=" << j << std::endl;
#endif //MURRAYC_DP_DEBUG_OUTPUT
      ++level;
      result = calc_subproblem(level, values...);

      const type_values key(values...);
      subproblems_[key] = result;

      subproblem_accesses_.emplace_back(
        type_values(values...), SubproblemAccess::CALCULATED);
    }

    return result;
  }
  

private:
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
  
  /** Gets the already-calculated subproblem solution, if any.
   * @result true if the subproblem solution was in the cache.
   */
  bool get_cached_subproblem(type_subproblem& subproblem, T_value_types... values) const {
    //std::cout << "get_cached_subproblem(): i=" << i << ", j=" << j << std::endl;
    const type_values key(values...);
    const auto iter = subproblems_.find(key);
    if (iter == subproblems_.end()) {
      subproblem = type_subproblem();
      return false;
    }

    //std::cout << "get_cached_subproblem(): returning cache for i=" << i << ", j=" << j << std::endl;
    subproblem = iter->second;
    return true;
  }
  
  /// Call get_goal_cell(a, b, c, d) with std::tuple<a, b, c, d>
  template<std::size_t... Is>
  void get_goal_cell_call_with_tuple(
    type_values& goals,
    std::index_sequence<Is...>) {
    get_goal_cell(std::get<Is>(goals)...);
  }
  
  /// Call get_subproblem(level, a, b, c, d) with std::tuple<a, b, c, d>
  template<std::size_t... Is>
  type_subproblem get_subproblem_call_with_tuple(type_level level,
    const type_values& goals,
    std::index_sequence<Is...>) {
    return get_subproblem(level, std::get<Is>(goals)...);
  }

protected:
  static void indent(type_level level)
  {
    std::cout << "level: " << level;
    for(type_level l = 0; l < level; ++l) {
      std::cout << "  ";
    }
  }

protected:

  //Map of values to subproblems:
  using type_map_subproblems = std::unordered_map<type_values, type_subproblem, hash_tuple::hash<type_values>>;
  mutable type_map_subproblems subproblems_;

  type_values value_counts_;

  //Keep a record of the order in which each subproblem was calculated:
  using type_subproblem_access = std::pair<type_values, SubproblemAccess>;
  mutable std::list<type_subproblem_access> subproblem_accesses_;
};

#endif //MURRAYCDP_DP_BOTTOM_UP_BASE_H
