#include "grammar.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>

#include "util.h"

namespace toy {

static const std::string GRAMMAR_FILEPATH = "../etc/toy-ll1.g";

std::string SymbolTypeToString(const SymbolType& type) {
  switch (type) {
    case SymbolType::EPSILON:
      return "EPSILON";
    case SymbolType::END:
      return "'$'";
    case SymbolType::START:
      return "<START>";
    default:
      throw std::invalid_argument(
          "Default strings only avaialble for EPSILON, END and START symbols");
  }
}

SymbolType StringToSymbolType(const std::string& str) {
  if (str == "EPSILON") {
    return SymbolType::EPSILON;
  } else if (str == "'$'") {
    return SymbolType::END;
  } else if (str == "<START>") {
    return SymbolType::START;
  } else if (str.at(0) == '!' && str.at(str.length() - 1) == '!') {
    return SymbolType::ACTION;
  } else if (str.at(0) == '\'' && str.at(str.length() - 1) == '\'') {
    return SymbolType::TERM;
  } else {
    return SymbolType::NON_TERM;
  }
}

Symbol::Symbol(std::string s)
    : s_(util::TrimString(s)), type_(StringToSymbolType(s_)) {}

Symbol::Symbol(SymbolType type) : s_(SymbolTypeToString(type)), type_(type) {}

Symbol::Symbol(Token tk)
    : s_("'" + TokenTypeToString(tk.Type()) + "'"),
      type_(StringToSymbolType(s_)) {}

SymbolType Symbol::Type() const { return type_; }

std::string Symbol::Str() const { return s_; }

std::string Symbol::RawStr() const { return s_.substr(1, s_.size() - 2); }

bool Symbol::IsTerm() const {
  return type_ == SymbolType::TERM || type_ == SymbolType::EPSILON;
}

bool Symbol::operator<(const Symbol& rhs) const { return s_ < rhs.s_; }

std::ostream& operator<<(std::ostream& os, const Symbol& symb) {
  os << symb.s_;
  return os;
}

bool operator==(const Symbol& symb1, const Symbol& symb2) {
  return symb1.s_ == symb2.s_;
}

bool operator!=(const Symbol& symb1, const Symbol& symb2) {
  return symb1.s_ != symb2.s_;
}

bool operator==(const Symbol& symb, const Token& tk) {
  return symb.RawStr() == TokenTypeToString(tk.Type());
}

bool operator!=(const Symbol& symb, const Token& tk) {
  return symb.RawStr() != TokenTypeToString(tk.Type());
}

Grammar::Grammar() { ReadGrammar(GRAMMAR_FILEPATH); }

Grammar::Grammar(std::string filepath) { ReadGrammar(filepath); }

void Grammar::ReadGrammar(const std::string& filepath) {
  std::ifstream file(filepath);
  std::string line;
  while (std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }
    // <indice> ::= '[' <arithExpr> ']'
    std::string delimiter = "::=";
    // Extract the symbol from the LHS, e.g. <indice>
    Symbol lhs(line.substr(0, line.find(delimiter)));
    // Extract the symbols from the RHS, e.g. '[', <arithExpr>, ']'
    std::istringstream iss(
        line.substr(line.find(delimiter) + delimiter.length()));
    std::vector<Symbol> rhs{std::istream_iterator<std::string>{iss},
                            std::istream_iterator<std::string>{}};
    grm_[lhs].push_back(rhs);
  }
}

GrammarIt Grammar::Begin() const { return grm_.begin(); }

GrammarIt Grammar::End() const { return grm_.end(); }

std::vector<Production> Grammar::GetProductionsForSymbol(Symbol symb) const {
  std::vector<Production> productions;
  for (Rhs rhs : grm_.at(symb)) {
    Rhs rhs_no_actions;
    std::copy_if(rhs.begin(), rhs.end(), std::back_inserter(rhs_no_actions),
                 [](Symbol s) { return s.Type() != SymbolType::ACTION; });
    productions.emplace_back(symb, rhs_no_actions);
  }
  return productions;
}

std::vector<Production> Grammar::GetProductionsWithSymbol(Symbol symb) const {
  std::vector<Production> productions;
  for (auto it = grm_.begin(); it != grm_.end(); ++it) {
    Symbol lhs = it->first;
    for (auto& rhs : it->second) {
      if (std::find(rhs.begin(), rhs.end(), symb) != rhs.end()) {
        Rhs rhs_no_actions;
        std::copy_if(rhs.begin(), rhs.end(), std::back_inserter(rhs_no_actions),
                     [](Symbol s) { return s.Type() != SymbolType::ACTION; });
        productions.emplace_back(lhs, rhs_no_actions);
      }
    }
  }
  return productions;
}

std::ostream& operator<<(std::ostream& os, const Grammar& grm) {
  for (auto it = grm.grm_.begin(); it != grm.grm_.end(); ++it) {
    Symbol lhs = it->first;
    for (std::vector<Symbol> rhs : it->second) {
      std::cout << lhs << " ::= ";
      for (Symbol symbol : rhs) {
        std::cout << symbol << " ";
      }
      std::cout << std::endl;
    }
  }
  return os;
}

}  // namespace toy
