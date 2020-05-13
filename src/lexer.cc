#include "lexer.h"

#include "logger.h"
#include "token.h"

namespace toy {

static const std::string OUT_TOKENS_FILEPATH = "../out/outlextokens";

Lexer::Lexer(std::istream& is)
    : is_(is), line_(1), tokens_ofs_(OUT_TOKENS_FILEPATH) {}

bool Lexer::HasNext() {
  SkipWs();
  return is_.tellg() != -1;
}

Token Lexer::NextToken() {
  SkipWs();
  is_.get(c_);
  Token tk;
  std::string s(1, c_);
  if (is_.tellg() == -1) {  // Reached end of source, return '$'.
    tk = Token(TokenType::EOS, "$", line_);
  } else if (isalpha(c_) || c_ == '_') {  // Find id token.
    tk = HandleId(s);
  } else if (isdigit(c_)) {  // Handle intnum or floatnum tokens.
    tk = HandleNum(s);
  } else if (c_ == '=') {  // Handle '=' or '==' tokens.
    c_ = is_.get();
    if (c_ == '=') {
      tk = Token(TokenType::EQ, s + c_, line_);
    } else {
      is_.unget();
      tk = Token(TokenType::ASSGN, s, line_);
    }
  } else if (c_ == '<') {  // Handle '<', '<=' or '<>' tokens.
    c_ = is_.get();
    if (c_ == '=') {
      tk = Token(TokenType::LEQ, s + c_, line_);
    } else if (c_ == '>') {
      tk = Token(TokenType::NEQ, s + c_, line_);
    } else {
      is_.unget();
      tk = Token(TokenType::LT, s, line_);
    }
  } else if (c_ == '>') {  // Handle '>' and '>=' tokens.
    c_ = is_.get();
    if (c_ == '=') {
      tk = Token(TokenType::GEQ, s + c_, line_);
    } else {
      is_.unget();
      tk = Token(TokenType::GT, s, line_);
    }
  } else if (c_ == '+') {  // Find '+' token.
    tk = Token(TokenType::PLUS, s, line_);
  } else if (c_ == '-') {  // Find '-' token.
    tk = Token(TokenType::MINUS, s, line_);
  } else if (c_ == '*') {  // 'Find *' token.
    tk = Token(TokenType::MULT, s, line_);
  } else if (c_ == '/') {  // Handle '/', // comments and /* comments */
    tk = HandleDivOrComment(s);
  } else if (c_ == '(') {  // Find '(' token.
    tk = Token(TokenType::OPEN_PAR, s, line_);
  } else if (c_ == ')') {  // Find ')' token.
    tk = Token(TokenType::CLOSE_PAR, s, line_);
  } else if (c_ == '{') {  // Find '{' token.
    tk = Token(TokenType::OPEN_CBR, s, line_);
  } else if (c_ == '}') {  // Find '}' token.
    tk = Token(TokenType::CLOSE_CBR, s, line_);
  } else if (c_ == '[') {  // Find '[' token.
    tk = Token(TokenType::OPEN_SQBR, s, line_);
  } else if (c_ == ']') {  // Find ']' token.
    tk = Token(TokenType::CLOSE_SQBR, s, line_);
  } else if (c_ == ';') {  // Find ';' token.
    tk = Token(TokenType::SEMICOLON, s, line_);
  } else if (c_ == ',') {  // Find ',' token.
    tk = Token(TokenType::COMMA, s, line_);
  } else if (c_ == '.') {  // Find '.' token.
    tk = Token(TokenType::DOT, s, line_);
  } else if (c_ == ':') {  // Handle ':' and '::' tokens.
    c_ = is_.get();
    if (c_ == ':') {
      tk = Token(TokenType::SCOPE_RES, s + c_, line_);
    } else {
      is_.unget();
      tk = Token(TokenType::COLON, s, line_);
    }
  } else {
    tk = Token(TokenType::INVALID_CHAR, s, line_);  // Unexpected token.
  }
  LogToken(tk);
  return tk;
}

Token Lexer::HandleId(std::string& s) {
  Token tk;
  if (isalpha(c_)) {
    c_ = is_.get();
    while (is_.tellg() != -1 && (isalnum(c_) || c_ == '_')) {
      s += c_;
      c_ = is_.get();
    }
    is_.unget();
    tk = Token(GetIdOrReservedWordType(s), s, line_);
  } else if (c_ == '_') {
    c_ = is_.get();
    while (is_.tellg() != -1 && !isspace(c_)) {
      s += c_;
      c_ = is_.get();
    }
    is_.unget();
    tk = Token(TokenType::INVALID_ID, s, line_);
  }
  return tk;
}

Token Lexer::HandleNum(std::string& s) {
  Token tk;
  if (c_ == '0') {
    c_ = is_.get();
    if (is_.tellg() != -1 && !IsNumTerminatingChar(c_) && c_ != '.') {
      s += c_;
      c_ = is_.get();
      while (is_.tellg() != -1 && !IsNumTerminatingChar(c_)) {
        s += c_;
        c_ = is_.get();
      }
      is_.unget();
      tk = Token(TokenType::INVALID_NUM, s, line_);
    } else {
      if (c_ == '.') {
        tk = HandleFloatNum(s);
      } else {
        is_.unget();
        tk = Token(TokenType::INTNUM, s, line_);
      }
    }
  } else {
    c_ = is_.get();
    while (is_.tellg() != 1 && isdigit(c_)) {
      s += c_;
      c_ = is_.get();
    }
    if (c_ == '.') {
      tk = HandleFloatNum(s);
    } else if (is_.tellg() != -1 && !IsNumTerminatingChar(c_)) {
      s += c_;
      c_ = is_.get();
      while (is_.tellg() != -1 && !IsNumTerminatingChar(c_)) {
        s += c_;
        c_ = is_.get();
      }
      is_.unget();
      tk = Token(TokenType::INVALID_NUM, s, line_);
    } else {
      is_.unget();
      tk = Token(TokenType::INTNUM, s, line_);
    }
  }
  return tk;
}

Token Lexer::HandleFloatNum(std::string& s) {
  Token tk;
  s += c_;
  c_ = is_.get();
  if (isdigit(c_)) {
    s += c_;
    c_ = is_.get();
    bool ends_with_zero = false;
    while (isdigit(c_)) {
      ends_with_zero = (c_ == '0') ? true : false;
      s += c_;
      c_ = is_.get();
      while (c_ == '0') {
        ends_with_zero = true;
        s += c_;
        c_ = is_.get();
      }
    }
    if (!ends_with_zero) {
      if (c_ == 'e') {
        s += c_;
        c_ = is_.get();
        if (c_ == '+' || c_ == '-') {
          s += c_;
          c_ = is_.get();
        }
        if (isdigit(c_)) {
          s += c_;
          if (c_ == '0') {
            c_ = is_.get();
            if (is_.tellg() != -1 && !IsNumTerminatingChar(c_)) {
              s += c_;
              c_ = is_.get();
              while (is_.tellg() != -1 && !IsNumTerminatingChar(c_)) {
                s += c_;
                c_ = is_.get();
              }
              is_.unget();
              tk = Token(TokenType::INVALID_NUM, s, line_);
            } else {
              is_.unget();
              tk = Token(TokenType::FLOATNUM, s, line_);
            }
          } else if (isdigit(c_)) {
            c_ = is_.get();
            while (isdigit(c_)) {
              s += c_;
              c_ = is_.get();
            }
            is_.unget();
            tk = Token(TokenType::FLOATNUM, s, line_);
          } else {
            is_.unget();
            tk = Token(TokenType::INVALID_NUM, s, line_);
          }
        } else {
          is_.unget();
          tk = Token(TokenType::INVALID_NUM, s, line_);
        }

      } else {
        is_.unget();
        tk = Token(TokenType::FLOATNUM, s, line_);
      }
    } else {
      is_.unget();
      tk = Token(TokenType::INVALID_NUM, s, line_);
    }
  } else {
    is_.unget();
    tk = Token(TokenType::INVALID_NUM, s, line_);
  }
  return tk;
}

Token Lexer::HandleDivOrComment(std::string& s) {
  Token tk;
  c_ = is_.get();
  if (c_ == '/') {
    s += c_;
    c_ = is_.get();
    while (is_.tellg() != -1 && c_ != '\n') {
      s += c_;
      c_ = is_.get();
    }
    is_.unget();
    tk = Token(TokenType::INLINE_CMT, s, line_);
  } else if (c_ == '*') {
    s += c_;
    uint block_cmt_line = line_;
    while (is_.tellg() != -1) {
      while (is_.tellg() != -1 && is_.get(c_) && c_ != '*') {
        if (c_ == '\n') {
          line_++;
          s += "\\n";
        } else {
          s += c_;
        }
      }
      if (is_.tellg() == -1) {
        tk = Token(TokenType::UNTERMINATED_CMT, s, block_cmt_line);
        break;
      } else {
        s += c_;
      }
      c_ = is_.get();
      if (is_.tellg() == -1) {
        tk = Token(TokenType::UNTERMINATED_CMT, s, block_cmt_line);
        break;
      } else if (c_ == '/') {
        s += c_;
        tk = Token(TokenType::BLOCK_CMT, s, block_cmt_line);
        break;
      }
    }
  } else {
    is_.unget();
    tk = Token(TokenType::DIV, s, line_);
  }
  return tk;
}

// Return the correct TokenType given a string that is either
// an id or a reserved word.
TokenType Lexer::GetIdOrReservedWordType(const std::string& str) {
  if (str == "if") {
    return TokenType::IF;
  } else if (str == "then") {
    return TokenType::THEN;
  } else if (str == "else") {
    return TokenType::ELSE;
  } else if (str == "while") {
    return TokenType::WHILE;
  } else if (str == "class") {
    return TokenType::CLASS;
  } else if (str == "integer") {
    return TokenType::INTEGER;
  } else if (str == "float") {
    return TokenType::FLOAT;
  } else if (str == "do") {
    return TokenType::DO;
  } else if (str == "end") {
    return TokenType::END;
  } else if (str == "public") {
    return TokenType::PUBLIC;
  } else if (str == "private") {
    return TokenType::PRIVATE;
  } else if (str == "or") {
    return TokenType::OR;
  } else if (str == "and") {
    return TokenType::AND;
  } else if (str == "not") {
    return TokenType::NOT;
  } else if (str == "read") {
    return TokenType::READ;
  } else if (str == "write") {
    return TokenType::WRITE;
  } else if (str == "return") {
    return TokenType::RETURN;
  } else if (str == "main") {
    return TokenType::MAIN;
  } else if (str == "inherits") {
    return TokenType::INHERITS;
  } else if (str == "local") {
    return TokenType::LOCAL;
  } else if (str == "void") {
    return TokenType::VOID;
  } else {
    return TokenType::ID;
  }
}

// Returns true if the given string is a reserved word.
bool Lexer::IsReserved(const std::string& word) {
  return word == "if" || word == "then" || word == "else" || word == "while" ||
         word == "class" || word == "integer" || word == "float" ||
         word == "do" || word == "end" || word == "public" ||
         word == "private" || word == "or" || word == "and" || word == "not" ||
         word == "read" || word == "write" || word == "return" ||
         word == "main" || word == "inherits" || word == "local" ||
         word == "void";
}

// Returns true if the given char can terminate a num token.
bool Lexer::IsNumTerminatingChar(const char& c) {
  return isspace(c) || c == '=' || c == '+' || c == '-' || c == '/' ||
         c == '*' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';' ||
         c == ']' || c == ',';
}

// Sets the stream position to before the next non-whitespace character.
void Lexer::SkipWs() {
  while (is_.get(c_)) {
    if (!isspace(c_)) {
      is_.unget();
      break;
    } else if (c_ == '\n') {
      line_++;
    }
  }
}

void Lexer::LogToken(const Token& tk) {
  tokens_ofs_ << tk << std::endl;
  switch (tk.Type()) {
    case TokenType::INVALID_ID:
      Logger::Err("Invalid identifier '" + tk.Lexeme() + "'", tk.Line(),
                  ErrorType::LEXING);
      break;
    case TokenType::INVALID_CHAR:
      Logger::Err("Invalid character '" + tk.Lexeme() + "'", tk.Line(),
                  ErrorType::LEXING);
      break;
    case TokenType::INVALID_NUM:
      Logger::Err("Invalid number '" + tk.Lexeme() + "'", tk.Line(),
                  ErrorType::LEXING);
      break;
    case TokenType::UNTERMINATED_CMT:
      Logger::Err("Unterminated comment '" + tk.Lexeme() + "'", tk.Line(),
                  ErrorType::LEXING);
      break;
    default:
      break;
  }
}

}  // namespace toy
