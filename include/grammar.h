#ifndef TOY_GRAMMAR_H_
#define TOY_GRAMMAR_H_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "token.h"

namespace toy {

// The grammar format with added semantic actions is as follows:
// - Terminal symbols are surrounded by ' '
// - Non-terminals are surrounded by < >
// - Action symbols are sourrounded by ! !
//
// An action symbol represents a semantic action to
// be executed during parsing in order to construct the AST.
enum class SymbolType { TERM, NON_TERM, ACTION, EPSILON, START, END };

std::string SymbolTypeToString(const SymbolType& type);
SymbolType StringToSymbolType(const std::string& str);

/**
 * Grammar symbol.
 */
class Symbol {
 public:
  Symbol() = default;
  Symbol(std::string s);
  Symbol(SymbolType type);
  Symbol(Token tk);
  SymbolType Type() const;
  std::string Str() const;
  std::string RawStr() const;
  bool IsTerm() const;
  bool operator<(const Symbol& rhs) const;
  friend bool operator==(const Symbol& symb1, const Symbol& symb2);
  friend bool operator!=(const Symbol& symb1, const Symbol& symb2);
  // Terminal symbols can be compared with tokens.
  friend bool operator==(const Symbol& symb, const Token& tk);
  friend bool operator!=(const Symbol& symb, const Token& tk);
  friend std::ostream& operator<<(std::ostream& os, const Symbol& symb);

 private:
  std::string s_;
  SymbolType type_;
};

namespace {
typedef std::vector<Symbol> Rhs;
typedef std::pair<Symbol, Rhs> Production;
typedef std::map<Symbol, std::vector<Rhs>>::const_iterator GrammarIt;
}  // namespace

/**
 * Context-free grammar.
 */
class Grammar {
 public:
  Grammar();
  Grammar(std::string filepath);
  GrammarIt Begin() const;
  GrammarIt End() const;
  // Get productions containing this symbol on the lhs.
  std::vector<Production> GetProductionsForSymbol(Symbol symb) const;
  // Get productions containing this symbol on the rhs.
  std::vector<Production> GetProductionsWithSymbol(Symbol symb) const;

  friend std::ostream& operator<<(std::ostream& os, const Grammar& grm);

 private:
  std::map<Symbol, std::vector<Rhs>> grm_;
  void ReadGrammar(const std::string& filepath);
};
std::ostream& operator<<(std::ostream& os, const Grammar& grm);

}  // namespace toy

#endif  // TOY_GRAMMAR_H_