#ifndef TOY_PARSER_GEN_H_
#define TOY_PARSER_GEN_H_

#include <map>
#include <set>
#include <vector>

#include "grammar.h"

namespace toy {

namespace {
typedef std::set<Symbol> Set;
typedef std::map<Symbol, Set> Sets;
typedef std::map<std::vector<Symbol>, Set> FirstSetsRhs;
typedef std::map<Symbol, std::vector<Symbol>> Graph;
typedef std::map<Symbol, std::map<Symbol, Production>> ParsingTable;
}  // namespace

/**
 * Generates and stores first sets, follow sets and the
 * LL(1) parsing table for the given grammar.
 */
class ParserGenerator {
 public:
  ParserGenerator() = delete;
  ParserGenerator(const Grammar& grammar);

  const Sets& FirstSets() const;
  const Sets& FollowSets() const;
  const FirstSetsRhs& FirstSetsOfRhs() const;
  Production TableAt(Symbol row, Symbol col);
  bool TableValid(Symbol row, Symbol col);
  void PrintSets() const;
  void PrintTable() const;

 private:
  const Grammar& grammar_;
  Sets first_sets_;
  Sets follow_sets_;
  FirstSetsRhs first_sets_rhs_;
  Graph dep_graph_;
  ParsingTable table_;

  void CalculateSets();
  void ConstructTable();
  Set FirstSetOf(Symbol symb);
  Set FirstSetOfRhs(const std::vector<Symbol>& rhs);
  Set FollowSetOf(Symbol symb);
  void ResolveDependencies(Symbol symb);
};

}  // namespace toy

#endif  // TOY_PARSER_GEN_H_