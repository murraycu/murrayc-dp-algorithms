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
#include <regex>
#include <string>
#include <vector>

#include <murraycdp/dp_top_down_base.h>

using uint = unsigned int;

//** Map of rule names to rules that the rule can produce:
using MapRules = std::unordered_map<std::string, std::vector<std::string>>;

using Range = std::pair<uint, uint>;

/** This is the cost of the "substring" i:j.
 */
class SubProblem {
public:
  bool can_produce = false;

  // To be really useful, this would be a tree, not just a description of the
  // solution tree:
  std::string solution;
};

/** Based on section 4.6 "Parsing Context-Free Grammars" from
 * The Algorithm Design Manual by Steven S. Skiena.
 *
 * Each subproblem is whether (bool) a nonterminal rule is generated by the
 * substring i:j of S.
 */
class DpContextFreeGrammarParser
  : public murraycdp::DpTopDownBase<SubProblem,
      uint,              // i
      uint,              // j
      const std::string& // non-terminal rule name
      > {
public:
  DpContextFreeGrammarParser(const MapRules& non_terminal_rules,
    const MapRules& terminals, const std::string& sentence)
  : non_terminal_rules_(non_terminal_rules),
    terminals_(terminals),
    words_(parse_words(sentence)) {}

private:
  static std::vector<std::string>
  parse_words(const std::string& sentence) {
    std::vector<std::string> result;

    std::regex ws_re("\\s+"); // whitespace
    std::sregex_token_iterator iter(
      sentence.begin(), sentence.end(), ws_re, -1);
    std::sregex_token_iterator end;
    while (iter != end) {
      result.emplace_back(*iter);
      ++iter;
    }

    return result;
  }

  type_subproblem
  calc_subproblem(type_level level, uint i, uint j,
    const std::string& nonterminal_rule_name) const override {
    // indent(level);
    // std::cout << "calc_subproblem: i=" << i << ", j=" << j << ", rule: " <<
    // nonterminal_rule_name << std::endl;
    SubProblem result;
    result.can_produce = false;

    if (i == j) {
      // Check terminals:
      const auto iter_terminal = terminals_.find(nonterminal_rule_name);
      if (iter_terminal != terminals_.end()) {
        const auto substr = words_[i];
        // indent(level);
        // std::cout << "substr: " << substr << std::endl;
        const auto produces = iter_terminal->second;
        for (const auto& terminal : produces) {
          if (terminal == substr) {
            result.can_produce = true;
            result.solution =
              describe_production(nonterminal_rule_name, terminal);
            // indent(level);
            // std::cout << "result: TERMINAL: " << terminal << std::endl;
            return result;
          }
        }
      }
    }

    // Check non-terminals:
    // TODO: Avoid the need for the runtime size check:
    const auto iter_nonterminal =
      non_terminal_rules_.find(nonterminal_rule_name);
    if (iter_nonterminal == non_terminal_rules_.end()) {
      return result;
    }

    const auto produces = iter_nonterminal->second;
    if (produces.size() != 2) {
      std::cerr << "Incorrect number of produced rules for rule: "
                << nonterminal_rule_name << std::endl;
      return result;
    }
    const auto X_name = produces[0];
    const auto Y_name = produces[1];

    for (uint k = i + 1; k <= j; ++k) {
      const auto sub_i_to_k = get_subproblem(level, i, k - 1, X_name);
      const auto sub_k_to_j = get_subproblem(level, k, j, Y_name);
      if (sub_i_to_k.can_produce && sub_k_to_j.can_produce) {
        result.can_produce = true;
        result.solution = describe_production(nonterminal_rule_name,
          sub_i_to_k.solution + " " + sub_k_to_j.solution);
        // indent(level);
        // std::cout << "result: non-terminals: " << X_name << " " << Y_name <<
        // std::endl;
        return result;
      }
    }

    // indent(level);
    // std::cout << "result: no" << std::endl;
    return result;
  }

  static std::string
  describe_production(const std::string a, const std::string b) {
    return "(" + a + " ::= " + b + ")";
  }

  void
  get_goal_cell(unsigned int& i, unsigned int& j,
    std::string& nonterminal_rule_name) const override {
    i = 0;
    j = words_.size() - 1;
    nonterminal_rule_name = "sentence";
  }

  MapRules non_terminal_rules_;
  MapRules terminals_;
  std::vector<std::string> words_;
};

int
main() {
  const auto sentence = "the cat drank the milk";

  const MapRules rules_nonterminals(
    {{"sentence", {"noun-phrase", "verb-phrase"}},
      {"noun-phrase", {"article", "noun"}},
      {"verb-phrase", {"verb", "noun-phrase"}}});

  const MapRules rules_terminals({{"article", {"the", "a"}},
    {"noun", {"cat", "milk"}}, {"verb", {"drank"}}});

  std::cout << "String: " << sentence << std::endl;
  DpContextFreeGrammarParser dp(rules_nonterminals, rules_terminals, sentence);
  const auto result = dp.calc();
  std::cout << "result: " << (result.can_produce ? "Can parse" : "Cannot parse")
            << std::endl;
  if (result.can_produce) {
    std::cout << "solution: " << std::endl << result.solution << std::endl;
  }

  assert(result.can_produce);
  assert(result.solution == "(sentence ::= (noun-phrase ::= (article ::= the) "
                            "(noun ::= cat)) (verb-phrase ::= (verb ::= drank) "
                            "(noun-phrase ::= (article ::= the) (noun ::= "
                            "milk))))");

  // This shows that we calculate all sub-problems, missing none,
  // so this top-down version has no advantage over the bottom-up version.
  // However, the bottom-up version can discard older (> i-2) results along the
  // way.
  std::cout << "Count of sub-problems calculated: "
            << dp.count_cached_sub_problems() << std::endl;

  return EXIT_SUCCESS;
}
