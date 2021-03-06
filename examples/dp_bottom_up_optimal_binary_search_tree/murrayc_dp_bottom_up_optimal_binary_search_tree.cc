/* Copyright (C) 2015, 2016 Murray Cumming
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
#include <numeric>
#include <string>
#include <vector>

#include <murraycdp/dp_bottom_up_base.h>

class ItemAndFrequency {
public:
  char item;
  unsigned int percentage; // 0 to 100.
};

class SubSolution {
public:
  using type_items = std::vector<ItemAndFrequency>;
  using type_value = unsigned int;

  SubSolution() : value(0), root(0) {}

  explicit SubSolution(type_value value_in, unsigned int root_in)
  : value(value_in), root(root_in) {}

  SubSolution(const SubSolution& src) = default;
  SubSolution&
  operator=(const SubSolution& src) = default;

  SubSolution(SubSolution&& src) noexcept = default;
  SubSolution&
  operator=(SubSolution&& src) noexcept = default;

  type_value value;
  unsigned int root;
};

/**
 * Find the optimal binary search tree, by
 * choosing the best roots for the tree and its sub-trees.
 *
 * This uses O(n^3) time, because it nests loops over s (size of range),
 * i (start of range), and r (root inside range).
 */
class DpOptimalBinarySearchTree
  : public murraycdp::DpBottomUpBase<0, /* keep all subproblems */
      SubSolution, SubSolution::type_items::size_type,
      SubSolution::type_items::size_type> {
public:
  using type_value = unsigned int;
  using type_items = SubSolution::type_items;
  using type_size = type_items::size_type;

  DpOptimalBinarySearchTree(const type_items& items)
  : DpBottomUpBase(items.size() + 1, items.size()), items_(items) {}

private:
  /**
   * Calculate the optimal root for the @a s items starting with item @a i.
   * by calculating the optimal roots for every possible pair of subtrees.
   */
  type_subproblem
  calc_subproblem(type_level level, type_size s, type_size i) const override {
    // std::cout << "calc_subproblem: s=" << s << ", i=" << i << std::endl;

    constexpr auto INFINITE_COST = std::numeric_limits<type_value>::max();

    // Base case:
    if (s == 0) {
      return type_subproblem(0, 0);
    }

    const auto size = items_.size();
    if (i + s > size) {
      return type_subproblem(0, INFINITE_COST);
    }

    // Sum of all costs in this range:
    //
    // Note: If the item ends up being x levels deep in a sub-tree,
    // its cost will have been added together x times. And that's what
    // we want because the cost of each item in the optimal solution is
    // its frequency * its depth.
    // See https://youtu.be/u5eSBQQ4qVc?t=4m22s for Tim Roughgarden's more detailed explanation,
    // though it seems to hand-wave past this part.
    const auto b = std::begin(items_) + i;
    const auto freq_sum = std::accumulate(b, b + s, 0,
      [](auto sum, const auto& item) { return sum + item.percentage; });

    // Get the min of possible subproblems: For every possible root (try i to
    // r-1 and r+1 to i+s)).
    auto min = INFINITE_COST;
    type_size r_for_min = 0;
    const auto end = std::min(i + s, size);

    // TODO: This should reduce the overall time to O(n^2):
    // https://stackoverflow.com/questions/16987670/dynamic-programming-why-knuths-improvement-to-optimal-binary-search-tree-on2
    // but that needs us to index by the start and end, not by the start and length.
    //const auto r_left = get_subproblem(level, i, end - 1).root;
    //const auto r_right = get_subproblem(level, i + 1, end).root;
    //for (type_size r = r_left; r < r_right; ++r) {
    for (type_size r = i; r < end; ++r) {
      const auto left_size = r - i;
      const auto left = get_subproblem(level, left_size, i);
      // std::cout << "  r=" << r << ", (" << left_size << ", " << i << ")
      // left=" << left.value << std::endl;

      // TODO: Simplify this?
      type_value right_value = 0;
      const auto right_size = s - left_size - 1;
      const auto right_start = r + 1;
      if (right_start < end && (right_start + right_size) <= end) {
        const auto right = get_subproblem(level, right_size, right_start);
        // std::cout << "  r=" << r << ", (" << right_size << ", " <<
        // right_start << ") right=" << right.value << std::endl;
        right_value = right.value;
      }

      const auto cost = left.value + right_value;
      // std::cout << "  r=" << r << ", cost=" << cost << std::endl;
      if (cost < min) {
        min = cost;
        r_for_min = r;
      }
    }

    return type_subproblem(min + freq_sum, r_for_min);
  }

  void
  get_goal_cell(type_size& s, type_size& i) const override {
    // The answer is in the last-calculated cell:
    s = items_.size();
    i = 0;
  }

  const type_items items_;
};

int
main() {
  // Note that these must be sorted,
  // so we can choose a root r and know that the items
  // before it will be in its left sub-tree, and the items
  // after it will be in the right sub-tree.
  const DpOptimalBinarySearchTree::type_items items = {
    {'a', 11}, {'b', 10}, {'c', 12}, {'d', 22}, {'e', 18}};

  std::cout << "Problem:" << std::endl << "  items: ";
  for (const auto& item : items) {
    std::cout << item.item << ": " << item.percentage << ", ";
  }
  std::cout << std::endl;

  DpOptimalBinarySearchTree dp(items);
  const auto result = dp.calc();

  std::cout << "solution: value: " << result.value << std::endl;
  std::cout << "  root index: " << result.root << std::endl;
  std::cout << "  root item: " << items[result.root].item << std::endl;
  //  << "with solution: " << std::endl
  //  << "  a: [" << result.solution << "]" << std::endl

  // Uncomment to show the sequence: dp.print_subproblem_sequence();

  assert(result.value == 147);
  assert(result.root == 3);

  return EXIT_SUCCESS;
}
