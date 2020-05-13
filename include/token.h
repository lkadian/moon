#ifndef TOY_TOKEN_H_
#define TOY_TOKEN_H_

#include <iostream>

namespace toy {

enum class TokenType;

/**
 * Represents a token generated by the lexer.
 */
class Token {
 public:
  Token() = default;
  Token(TokenType type, std::string lexeme, int line);
  TokenType Type() const;
  std::string Lexeme() const;
  int Line() const;
  bool IsComment() const;

  friend std::ostream& operator<<(std::ostream& os, const Token& tk);
  friend bool operator==(const Token& tk1, const Token& tk2);

 private:
  TokenType type_;
  std::string lexeme_;
  int line_;  // first line the token appears in the source code.
};
bool operator==(const Token& tk1, const Token& tk2);
std::ostream& operator<<(std::ostream& os, const Token& tk);

std::string TokenTypeToString(const TokenType& type);

enum class TokenType {
  ID,
  INTNUM,
  FLOATNUM,
  EQ,
  NEQ,
  LT,
  GT,
  LEQ,
  GEQ,
  PLUS,
  MINUS,
  MULT,
  DIV,
  ASSGN,
  OPEN_PAR,
  CLOSE_PAR,
  OPEN_CBR,
  CLOSE_CBR,
  OPEN_SQBR,
  CLOSE_SQBR,
  SEMICOLON,
  COMMA,
  DOT,
  COLON,
  SCOPE_RES,
  // Reserved words.
  IF,
  THEN,
  ELSE,
  WHILE,
  CLASS,
  INTEGER,
  FLOAT,
  DO,
  END,
  PUBLIC,
  PRIVATE,
  OR,
  AND,
  NOT,
  READ,
  WRITE,
  RETURN,
  MAIN,
  INHERITS,
  LOCAL,
  VOID,
  // Comments.
  BLOCK_CMT,
  INLINE_CMT,
  // Lexical errors.
  INVALID_ID,
  INVALID_CHAR,
  INVALID_NUM,
  UNTERMINATED_CMT,
  // End of source, returned by NextToken() if there are no more tokens to scan.
  EOS
};

}  // namespace toy

#endif  // TOY_TOKEN_H_