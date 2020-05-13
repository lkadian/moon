#include "parser_gen.h"

#include <algorithm>
#include <queue>

#include "util.h"

namespace toy {

ParserGenerator::ParserGenerator(const Grammar& grammar) : grammar_(grammar) {
  CalculateSets();
  ConstructTable();
}

const Sets& ParserGenerator::FirstSets() const { return first_sets_; }

const Sets& ParserGenerator::FollowSets() const { return follow_sets_; }

const FirstSetsRhs& ParserGenerator::FirstSetsOfRhs() const {
  return first_sets_rhs_;
}

Production ParserGenerator::TableAt(Symbol row, Symbol col) {
  return table_.at(row).at(col);
}

bool ParserGenerator::TableValid(Symbol row, Symbol col) {
  return (table_.find(row) != table_.end()) &&
         (table_.at(row).find(col) != table_.at(row).end());
}

void ParserGenerator::PrintSets() const {
  for (auto& p : first_sets_) {
    std::cout << "FIRST(" << p.first << ") = ";
    util::PPrintSet(p.second);
  }
  for (auto& p : follow_sets_) {
    std::cout << "FOLLOW(" << p.first << ") = ";
    util::PPrintSet(p.second);
  }
}

void ParserGenerator::PrintTable() const {
  for (auto it = table_.begin(); it != table_.end(); ++it) {
    auto row = it->first;
    std::cout << "Row: " << row << std::endl;
    auto cols = it->second;
    for (auto it2 = cols.begin(); it2 != cols.end(); ++it2) {
      Production prod = it2->second;
      std::cout << "Col: " << it2->first << ", Production: ";
      for (Symbol symb : prod.second) {
        std::cout << symb << " ";
      }
      std::cout << std::endl;
    }
  }
}

void ParserGenerator::CalculateSets() {
  for (auto it = grammar_.Begin(); it != grammar_.End(); ++it) {
    FirstSetOf(it->first);
    FollowSetOf(it->first);
  }
  for (auto it = grammar_.Begin(); it != grammar_.End(); ++it) {
    ResolveDependencies(it->first);
  }
}

void ParserGenerator::ConstructTable() {
  for (auto it = grammar_.Begin(); it != grammar_.End(); ++it) {
    Symbol lhs = it->first;
    for (Rhs rhs : it->second) {
      if (table_.find(lhs) == table_.end()) {
        table_[lhs] = std::map<Symbol, Production>();
      }
      // If terminal is EPSILON do not add it to the table
      Rhs rhs_no_actions;
      std::copy_if(rhs.begin(), rhs.end(), std::back_inserter(rhs_no_actions),
                   [](Symbol s) { return s.Type() != SymbolType::ACTION; });
      auto first_rhs = first_sets_rhs_[rhs_no_actions];
      for (Symbol terminal : first_rhs) {
        if (terminal.Type() != SymbolType::EPSILON) {
          table_[lhs][terminal] = std::make_pair(lhs, rhs);
        }
      }
      if (first_rhs.find(Symbol("EPSILON")) != first_rhs.end()) {
        for (auto& terminal : follow_sets_[lhs]) {
          if (terminal.Type() != SymbolType::EPSILON) {
            table_[lhs][terminal] = std::make_pair(lhs, rhs);
          }
        }
      }
    }
  }
}

// Computes the first set of the given symbol
Set ParserGenerator::FirstSetOf(Symbol symb) {
  if (first_sets_.find(symb) != first_sets_.end()) {
    return first_sets_.at(symb);
  }
  // If it's a terminal, its first set is itself
  if (symb.IsTerm()) {
    first_sets_[symb].insert(symb);
    return first_sets_[symb];
  }

  auto productions_for_symbol = grammar_.GetProductionsForSymbol(symb);
  for (auto& production : productions_for_symbol) {
    std::vector<Symbol> rhs = production.second;
    std::set<Symbol> first_set_rhs = FirstSetOfRhs(rhs);
    // Add FIRST(X1X2...Xk) to first(A)
    first_sets_[symb].insert(first_set_rhs.begin(), first_set_rhs.end());
  }
  return first_sets_[symb];
}

// Computes the first set of a rhs
Set ParserGenerator::FirstSetOfRhs(const std::vector<Symbol>& rhs) {
  if (first_sets_rhs_.find(rhs) != first_sets_rhs_.end()) {
    return first_sets_rhs_.at(rhs);
  }
  std::set<Symbol> first_set_rhs;
  for (Symbol symb : rhs) {
    if (symb.Type() == SymbolType::ACTION) {
      continue;
    }
    if (symb.Type() == SymbolType::EPSILON) {
      first_set_rhs.insert(symb);
      break;
    }

    auto first_set_of_cur = FirstSetOf(symb);
    // Add first_set_of_cur to first_set_rhs, excluding EPSILON.
    Set tmp = first_set_of_cur;
    tmp.erase(Symbol("EPSILON"));
    first_set_rhs.insert(tmp.begin(), tmp.end());

    // If there is no EPSILON, we're done
    if (!std::any_of(
            first_set_of_cur.begin(), first_set_of_cur.end(),
            [](Symbol s) { return s.Type() == SymbolType::EPSILON; })) {
      break;
    }
    // If all symbols were checked, add EPSILON to the set
    else if (symb == rhs.at(rhs.size() - 1)) {
      first_set_rhs.insert(Symbol("EPSILON"));
    }
  }
  first_sets_rhs_[rhs] = first_set_rhs;
  return first_set_rhs;
}

// Computes the follow set of the given symbol
Set ParserGenerator::FollowSetOf(Symbol symb) {
  if (follow_sets_.find(symb) != follow_sets_.end()) {
    return follow_sets_.at(symb);
  }
  follow_sets_[symb] = {};

  // Start symbol always contains "$" in its follow set.
  if (symb.Type() == SymbolType::START) {
    follow_sets_[symb].emplace("'$'");
    return follow_sets_[symb];
  }

  // Check all productions where our symbol appears on the RHS
  std::vector<Production> productions_with_symbol =
      grammar_.GetProductionsWithSymbol(symb);

  for (Production production : productions_with_symbol) {
    std::vector<Symbol> rhs = production.second;
    size_t symbol_index = std::find(rhs.begin(), rhs.end(), symb) - rhs.begin();
    // We get the follow symbols of our symbol, e.g.
    // S -> AaAb, so Follow(A) would be FIRST(aAb) = {a, b}. If it's the last
    // symbol, it should be FOLLOW(S)
    while (symbol_index < rhs.size()) {
      std::vector<Symbol> follow_part =
          std::vector<Symbol>(rhs.begin() + symbol_index + 1, rhs.end());

      if (follow_part.size() > 0) {
        while (follow_part.size() > 0) {
          Symbol production_symbol = follow_part.at(0);
          Set first_of_follow = FirstSetOf(production_symbol);
          // Add first_set_of_cur to follow set, excluding EPSILON
          auto tmp = first_of_follow;
          tmp.erase(Symbol("EPSILON"));
          follow_sets_[symb].insert(tmp.begin(), tmp.end());

          // If there is no EPSILON, we're done, otherwise continue
          if (!std::any_of(
                  first_of_follow.begin(), first_of_follow.end(),
                  [](Symbol s) { return s.Type() == SymbolType::EPSILON; })) {
            break;
          }
          follow_part =
              std::vector<Symbol>(follow_part.begin() + 1, follow_part.end());
        }
      }

      // For productions of the form B -> XA, FOLLOW(A) is FOLLOW(B)
      if (follow_part.size() == 0) {
        auto lhs = production.first;
        // We add B to our dependency graph, and only on a second
        // pass we will add FOLLOW(B) to FOLLOW(A) by traversing
        // the graph to get As dependent FOLLOW sets.
        if (dep_graph_.find(symb) == dep_graph_.end()) {
          dep_graph_[symb] = {lhs};
        }
        dep_graph_.at(symb).push_back(lhs);
      }
      rhs = follow_part;
      symbol_index = std::find(rhs.begin(), rhs.end(), symb) - rhs.begin();
    }
  }
  return follow_sets_[symb];
}

// When we calculate FOLLOW(A) for productions of the form B -> XA,
// we do not add FOLLOW(B) to FOLLOW(A) right away, instead we add it to
// a dependency graph. This function propagates those changes with a bfs on a
// second pass, by adding all of As dependencies follow sets to its own follow
// set.
void ParserGenerator::ResolveDependencies(Symbol symb) {
  std::set<Symbol> visited = {symb};
  std::queue<Symbol> queue;
  queue.push(symb);

  while (!queue.empty()) {
    Symbol tmp = queue.front();
    queue.pop();

    for (auto& neighbor : dep_graph_[tmp]) {
      if (visited.find(neighbor) == visited.end()) {
        visited.insert(neighbor);
        queue.push(neighbor);

        Set nei_follow = follow_sets_[neighbor];
        follow_sets_[symb].insert(nei_follow.begin(), nei_follow.end());
      }
    }
  }
}

}  // namespace toy