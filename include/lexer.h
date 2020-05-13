#ifndef TOY_LEXER_H_
#define TOY_LEXER_H_

#include <fstream>
#include <memory>

#include "token.h"

namespace toy {

/**
 * Extracts tokens from an input stream.
 */
class Lexer {
 public:
  Lexer() = delete;
  Lexer(std::istream& is);

  // Finds and returns the next token from the input stream.
  Token NextToken();
  // Returns true if the lexer has another token in its input.
  bool HasNext();
  // Returns true if the word is a reserved word.
  static bool IsReserved(const std::string& word);

 private:
  std::istream& is_;
  char c_;  // Holds the current char being lexed
  int line_;
  std::ofstream tokens_ofs_;

  Token HandleId(std::string& s);
  Token HandleNum(std::string& s);
  Token HandleFloatNum(std::string& s);
  Token HandleDivOrComment(std::string& s);
  TokenType GetIdOrReservedWordType(const std::string& str);
  bool IsNumTerminatingChar(const char& c);
  void SkipWs();
  void LogToken(const Token& tk);
};

}  // namespace toy

#endif  // TOY_LEXER_H_