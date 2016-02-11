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

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <murraycdp/dp_top_down_base.h>

class Item {
public:
  using type_value = long long;
  using type_weight = long long;

  Item() : value(0), weight(0) {}

  Item(type_value value_in, type_weight weight_in)
  : value(value_in), weight(weight_in){};

  Item(const Item& src) = default;
  Item&
  operator=(const Item& src) = default;

  Item(Item&& src) = default;
  Item&
  operator=(Item&& src) = default;

  type_value value;
  type_weight weight;
};

class SubSolution {
public:
  using type_vec_items = std::vector<Item>;
  using type_value = Item::type_value;

  // static constexpr type_value COIN_COUNT_INFINITY =
  // std::numeric_limits<type_value>::max();

  SubSolution() : value(0) {}

  explicit SubSolution(type_value value_in) : value(value_in) {}

  SubSolution(const SubSolution& src) = default;
  SubSolution&
  operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) = default;
  SubSolution&
  operator=(SubSolution&& src) = default;

  type_value value;
  type_vec_items solution;
};

class DpKnapsack
  : public murraycdp::DpTopDownBase<SubSolution,
      SubSolution::type_vec_items::size_type, Item::type_weight> {
public:
  using type_value = Item::type_value;
  using type_weight = Item::type_weight;
  using type_vec_items = SubSolution::type_vec_items;
  using type_size = type_vec_items::size_type;

  DpKnapsack(const type_vec_items& items, type_weight weight_capacity)
  : items_(items), weight_capacity_(weight_capacity) {}

private:
  type_subproblem
  calc_subproblem(type_level level, type_size items_count,
    type_weight weight_capacity) const override {
    if (items_count == 0) {
      return type_subproblem(0); // 0 items means 0 value for any maximum
                                 // weight.
    }

    if (weight_capacity == 0) {
      return type_subproblem(0); // 0 max weight means 0 weight.
    }

    // std::cout << "  calc: i=" << items_count << ", w=" << weight_capacity <<
    // std::endl;

    const auto& item = items_[items_count];

    // If this item's weight alone is too much,
    // try the previously-calculated lesser number of items,
    // and don't bother trying any other alternative:
    if (item.weight > weight_capacity) {
      return get_subproblem(level, items_count - 1, weight_capacity);
    }

    // Case 1: This item is not in the optimal solution,
    // so the optimal solution would be unchanged by ignoring this item (not
    // including it in the optional solution)
    //
    // The value for same max weight with 1 less item.
    // This recurses, so it really tells us the max possible value across all of
    // the earlier items, for the same weight capacity.
    const auto subproblem_1_less_item =
      get_subproblem(level, items_count - 1, weight_capacity);

    // Case 2: This item is in the optimal solution (and is the last item in
    // it),
    // so the optimal solution's value is equal to the solution for 1 less item,
    // with less capacity,
    // plus the item's value.
    //
    // The value for the max weight minus the current item's weight, with 1 less
    // item, plus the current item's value.
    auto subproblem_1_less_item_less_weight =
      get_subproblem(level, items_count - 1, weight_capacity - item.weight);
    subproblem_1_less_item_less_weight.value += item.value;

    if (subproblem_1_less_item.value >
        subproblem_1_less_item_less_weight.value) {
      return subproblem_1_less_item;
    } else {
      subproblem_1_less_item_less_weight.solution.emplace_back(item);
      return subproblem_1_less_item_less_weight;
    }
  }

  void
  get_goal_cell(type_size& items_count, type_weight& weight) const override {
    // The answer is in the last-calculated cell:
    items_count = items_.size();
    weight = weight_capacity_;
  }

  const type_vec_items items_;
  const type_weight weight_capacity_;
};

void
print_vec(const std::vector<Item>& vec) {
  for (auto item : vec) {
    std::cout << "{" << item.value << ", " << item.weight << "}, ";
  }
}

int
main() {
  DpKnapsack::type_vec_items items{{13, 15}, {12, 14}, {2, 13}, {10, 12},
    {1, 11}, {7, 10}, {6, 9}, {5, 8}, {11, 7}, {14, 6}, {9, 5}, {4, 4}, {3, 3},
    {22, 2}, {8, 1}};

  const DpKnapsack::type_value weight_capacity = 45;

  std::cout << "Problem: weight capacity: " << weight_capacity << std::endl
            << "with items: ";
  print_vec(items);
  std::cout << std::endl;

  DpKnapsack dp(items, weight_capacity);
  const auto result = dp.calc();

  std::cout << "solution: value: " << result.value << std::endl
            << "with solution: ";
  print_vec(result.solution);
  std::cout << std::endl;

  std::cout << "Count of sub-problems calculated: "
            << dp.count_cached_sub_problems() << std::endl;

  // Uncomment to show the sequence: dp.print_subproblem_sequence();

  assert(result.value == 83);

  return EXIT_SUCCESS;
}
