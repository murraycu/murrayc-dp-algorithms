#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>

#include <murraycdp/circular_vector.h>

#define MURRAYC_DP_DEBUG_OUTPUT = 1;

/** A base class for a 2D bottom-up dynamic programming algorithm.
 *
 * Override this, implementing calc_cost(), and then call calc() to get the
 * overall cost.
 * @tparam T_COUNT_COSTS_TO_KEEP The number of previous i values that calc_cost() needs to use.
 * @tparam T_cost The type of the cost, such as unsigned int.
 */
template <unsigned int T_COUNT_COSTS_TO_KEEP, typename T_cost>
class DpBase {
public:
  using type_cost = T_cost;
  using type_costs = std::vector<T_cost>;

  /**
   * @param The number of i values to calculate the cost for.
   * @pram The number of j values to calculate the cost for.
   */
  DpBase(unsigned int i_count, unsigned int j_count)
  : costs_(T_COUNT_COSTS_TO_KEEP, type_costs(j_count)),
    i_count_(i_count),
    j_count_(j_count)
  {}
  
  type_cost calc() {
    for (unsigned int i = 0; i < i_count_; ++i) {
      type_costs& costs_i = costs_.get(0);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << "i=" << std::setw(2) << i << ": ";
#endif

      for (unsigned int j = 0; j < j_count_; ++j) {
        costs_i[j] = calc_cost(i, j);

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
        if (j != 0) {
          std::cout << ", ";
        }
        std::cout << std::setw(2) << costs_i[j].cost;
#endif
      }

#if defined(MURRAYC_DP_DEBUG_OUTPUT)
      std::cout << std::endl;
#endif

      //costs_i will then be read as costs_i_minus_1;
      //and costs_i_minus_1 will be filled as costs_i;
      costs_.step(); //Swap costs_i and costs_i_minus_1.
    }

    unsigned int goal_i = 0;
    unsigned int goal_j = 0;
    get_goal_cell(goal_i, goal_j);
    //std::cout << "goal_i=" << goal_i << ", goal_j=" << goal_j << std::endl;

    const type_costs& costs_i = costs_.get_at_offset_from_start(goal_i);
    return costs_i[goal_j];
  }

private:
  virtual type_cost calc_cost(unsigned int i, unsigned int j) const = 0;

  /** Get the cell whose value contains the solution.
   */
  virtual void get_goal_cell(unsigned int& i, unsigned int& j) const = 0;

protected:
  using type_vec_costs = circular_vector<type_costs>;
  type_vec_costs costs_;
  uint i_count_;
  uint j_count_;
};

