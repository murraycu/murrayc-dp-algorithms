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

// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 test.cc -o prog

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>

typedef unsigned int type_value;

typedef std::vector<type_value> type_vec_coins;

/*
static
void indent(int level)
{
  std::cout << "level: " << level;
  for(int l = 0; l < level; ++l) {
    std::cout << "  ";
  }
}
*/

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

typedef type_vec_coins::size_type type_size;

class SubSolution
{
public:
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


typedef std::vector<std::unordered_map<type_value, SubSolution>> type_sub_problems;

const type_value COIN_COUNT_INFINITY = std::numeric_limits<type_value>::max();

const type_value COIN_COUNT_ONE = 1; //1 coin has a value of 1.

SubSolution
calc_optimal_sub_problem(const type_vec_coins& items, type_size item_number,
  type_value needed_value, type_sub_problems& sub_problems,
  int level)
{
  ++level;

  const auto& item_value = items[item_number - 1];

  //indent(level);
  //std::cout << "calc_optimal_sub_problem(): item_number=" << item_number <<
  //  ", item value=" << item_value << ", needed_value=" << needed_value << std::endl;

  if(needed_value == 0)
  {
    return SubSolution(0);
  }

  auto& map = sub_problems[needed_value];

  //Return a cached (memoized) value if one exists:
  const auto iter = map.find(item_number);
  if(iter != map.end())
  {
    auto& result = iter->second;

    //indent(level);
    //std::cout << "(cached) result.coin_count_used=" << result.coin_count_used << std::endl;

    return result;
  }

  if(item_number < 1)
  {
    std::cerr << "Unexpected item_number=0" << std::endl;
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
    SubSolution(COIN_COUNT_INFINITY) :
    calc_optimal_sub_problem(items,
      item_number - 1, needed_value,
      sub_problems,
      level));

/*
  if(item_value == needed_value)
  {
    indent(level);
    std::cout << "Taking item_value." << std::endl;
    result = SubSolution(COIN_COUNT_ONE);
    result.solution.emplace_back(item_value);
  }
*/
  if(item_value > needed_value)
  {
    //indent(level);
    //std::cout << "Taking case_dont_use_this_item because item_value too big." << std::endl;
    result = case_dont_use_this_item;
  }
  else
  {
    //The Knapsack problem would use 0 here, to match its check for a maximum
    //when comparing case 1 and case 2.
    //But we check for a minimum instead, so we use infinity here instead.
    SubSolution case_use_this_item(COIN_COUNT_INFINITY);
    if(item_number != 1)
    {
      case_use_this_item = calc_optimal_sub_problem(items,
        item_number - 1, needed_value - item_value,
        sub_problems,
        level);
      case_use_this_item.coin_count_used += COIN_COUNT_ONE;
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


  //Cache it:
  map[needed_value] = result;

  //indent(level);
  //std::cout << "result.coin_count_used=" << result.coin_count_used << std::endl;

  return result;
}

int main()
{
  //type_vec_coins coins{2, 52, 50};
  type_vec_coins coins{3, 50, 2, 100, 52, 119, 15};

  //Sort largest first:
  /*
  std::sort(coins.begin(), coins.end(),
    [](const type_value& a, const type_value& b)
    {
      return a > b;
    });
  */

  const type_value needed_value = 117;

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
  const auto items_count = coins.size();
  type_sub_problems sub_problems(needed_value + 1);
  const auto solution = calc_optimal_sub_problem(coins, items_count, needed_value,
    sub_problems, 0);
  if(solution.coin_count_used != COIN_COUNT_INFINITY)
  {
    std::cout << "solution: coins count: " << solution.coin_count_used << std::endl <<
      "with solution: ";
    print_vec(solution.solution);
    std::cout << std::endl;

    return EXIT_SUCCESS;
  }

  std::cout << "Cannot make the needed value: " << needed_value << std::endl << std::endl;;

  for(type_value lesser_value = needed_value - 1; lesser_value > 0; --lesser_value)
  {
    std::cout << "Trying: " << lesser_value << std::endl;
    const auto solution_lesser =
      calc_optimal_sub_problem(coins, items_count, lesser_value ,
       sub_problems, 0);
    if(solution_lesser.coin_count_used != COIN_COUNT_INFINITY)
    {
      std::cout << "lesser value_reached: " << lesser_value << std::endl <<
        "with solution: ";
      print_vec(solution_lesser.solution);
      std::cout << std::endl;
      break;
    }
  }

  return EXIT_SUCCESS;
}


