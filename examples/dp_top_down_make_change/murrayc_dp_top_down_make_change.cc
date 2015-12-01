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

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <murraycdp/dp_top_down_base.h>

class SubSolution
{
public:
  using type_value = unsigned int;
  using type_vec_coins = std::vector<type_value>;
  using type_size = type_vec_coins::size_type;
  
  static constexpr type_value COIN_COUNT_INFINITY = std::numeric_limits<type_value>::max();
  static constexpr type_value COIN_COUNT_ONE = 1; //1 coin has a value of 1.
 
  SubSolution()
  : coin_count_used(0)
  {
  }

  explicit SubSolution(type_size coin_count_used_in)
  : coin_count_used(coin_count_used_in)
  {}

  SubSolution(const SubSolution& src) = default;
  SubSolution& operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) = default;
  SubSolution& operator=(SubSolution&& src) = default;

  type_size coin_count_used;
  type_vec_coins solution;
};

class DpMakeChange
  : public DpTopDownBase<SubSolution, SubSolution::type_size, SubSolution::type_value> {
public:
  using type_value = SubSolution::type_value;
  using type_vec_coins = SubSolution::type_vec_coins;
  using type_size = SubSolution::type_size;

  DpMakeChange(const type_vec_coins& items, type_value needed_value)
  : DpTopDownBase(items.size() + 1, needed_value),
    items_(items),
    needed_value_(needed_value)
  {}

private:

  type_subproblem calc_subproblem(type_size item_number, type_value needed_value, type_level level) const override {
    //indent(level);
    //std::cout << "calc_subproblem(): item_number = " << item_number << ", needed_value=" << needed_value << std::endl;
    //const auto& item_value = items_[item_number - 1];
    //indent(level);
    //std::cout << "item value=" << item_value << std::endl;

    if(item_number < 1)
    {
      std::cerr << "Unexpected item_number=" << item_number << std::endl;
      return SubSolution();
    } else if (item_number > items_.size() + 1) {
      std::cerr << "Unexpected item_number=" << item_number << std::endl;
      return SubSolution();
    }

    SubSolution result;

    //Otherwise calculate it:
    //
    //The Knapsack problem would use 0 here, to match its check for a maximum
    //when comparing case 1 and case 2.
    //But we check for a minimum instead, so we use infinity here instead.
    auto case_dont_use_this_item =
      (item_number == 1 ?
      SubSolution(SubSolution::COIN_COUNT_INFINITY) :
      get_subproblem(item_number - 1, needed_value, level));

    const auto& item_value = items_[item_number - 1];
    //indent(level);
    //std::cout << "item_value=" << item_value << std::endl;

    if(item_value == needed_value)
    {
      //indent(level);
      //std::cout << "Taking item_value:" << item_value << std::endl;
      result = SubSolution(SubSolution::COIN_COUNT_ONE);
      result.solution.emplace_back(item_value);
    }
    else if(item_value > needed_value)
    {
      //indent(level);
      //std::cout << "Taking case_dont_use_this_item because item_value too big: " << item_value << std::endl;
      result = case_dont_use_this_item;
    }
    else
    {
      //The Knapsack problem would use 0 here, to match its check for a maximum
      //when comparing case 1 and case 2.
      //But we check for a minimum instead, so we use infinity here instead.
      SubSolution case_use_this_item(SubSolution::COIN_COUNT_INFINITY);
      if(item_number != 1)
      {
        case_use_this_item =
          get_subproblem(item_number - 1, needed_value - item_value, level);
        case_use_this_item.coin_count_used += SubSolution::COIN_COUNT_ONE;
      }

      //indent(level);
      //std::cout << "case_use_this_item=" << case_use_this_item.coin_count_used << 
      //    ", case_dont_use_this_item=" << case_dont_use_this_item.coin_count_used << std::endl;

      //We check for a minimum here, to minimize coins count.
      //The normal knapsack problem would need us to check for a maximum instead,
      //to maximize value.
      if(case_use_this_item.coin_count_used < case_dont_use_this_item.coin_count_used)
      {
        //indent(level);
        //std::cout << "Taking case_use_this_item because it is smaller: " << case_use_this_item.coin_count_used << std::endl;

        result = case_use_this_item;
        result.solution.emplace_back(item_value);
      }
      else
      {
        //indent(level);
        //std::cout << "Taking case_dont_use_this_item because it is smaller:" << case_dont_use_this_item.coin_count_used << std::endl;

        result = case_dont_use_this_item;
      }
    }

    //indent(level);
    //std::cout << "item_value=" << item_value << ", returning result: coin_count_used=" << result.coin_count_used << std::endl;
    return result;
  }

  void get_goal_cell(type_i& i, type_j& j) const override {
     //The answer is in the last-calculated cell:
     i = i_count_;
     j = j_count_;
  }

  static uint match(const char ch_str, const char ch_pattern) {
    if (ch_str == ch_pattern) {
      return 0;
    } else {
      return 1;
    }
  }

  static uint indel(char /* ch */) {
    return 1;
  }
  
  const type_vec_coins items_;
  const type_value needed_value_;
};

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

int main() {
  DpMakeChange::type_vec_coins coins{3, 50, 2, 100, 52, 119, 15};
  const DpMakeChange::type_value needed_value = 117;

  //Sort largest first:
  /*
  std::sort(coins.begin(), coins.end(),
    [](const type_value& a, const type_value& b)
    {
      return a > b;
    });
  */

  std::cout << "Problem: needed value: " << needed_value << std::endl <<
    "with coins: ";
  print_vec(coins);
  std::cout << std::endl;

  //Use a dynamic programming (optimal sub structure) algorithm,
  //because a greedy algorithm would only work with a "canonical" coin system:
  //http://cs.stackexchange.com/a/6625/40929
  //This is like the knapsack problem, but where:
  //- 1 (coin used) is like the knapsack problems' item value.
  //- coin used count (minimized) is like the knapsack problem's total item value (maximized).
  //- coin value is lke the knapsack problem's item weight.
  //- value_needed is like the knapsack problems' weight capacity
  //  We try to get under the limit, hoping to hit the exact limit.
  //
  //The main way to identify the analogue seems to be to identify what is being maximised or minimised.
  //
  //Unlike with the knapsack problem, we want the minimum coin count for the needed value,
  //instead of the maximum value for the available (or less) weigtht.
  //
  //Use a recursive memoization technique, instead of filling a count*needed_value
  //array, because we shouldn't need to calculate all possibilities.
  DpMakeChange dp(coins, needed_value);
  const auto result = dp.calc();
  
  if(result.coin_count_used != SubSolution::COIN_COUNT_INFINITY)
  {
    std::cout << "solution: coins count: " << result.coin_count_used << std::endl <<
      "with solution: ";
    print_vec(result.solution);
    std::cout << std::endl;
  } else {
    std::cout << "Cannot make the needed value: " << needed_value << std::endl;
  }

  std::cout << "Count of sub-problems calculated: " <<
    dp.count_cached_sub_problems() << std::endl;

  assert(result.coin_count_used == 3);

  return EXIT_SUCCESS;
}
