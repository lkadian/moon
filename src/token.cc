#include "token.h"

namespace toy {

Token::Token(TokenType type, std::string lexeme, int line)
    : type_(type), lexeme_(lexeme), line_(line) {}

TokenType Token::Type() const { return type_; }

std::string Token::Lexeme() const { return lexeme_; }

int Token::Line() const { return line_; }

bool Token::IsComment() const {
  return type_ == TokenType::INLINE_CMT || type_ == TokenType::BLOCK_CMT;
}

std::ostream& operator<<(std::ostream& os, const Token& tk) {
  os << "[" << TokenTypeToString(tk.type_) << ", " << tk.lexeme_ << ", "
     << tk.line_ << "]";
  return os;
}

bool operator==(const Token& tk1, const Token& tk2) {
  return tk1.type_ == tk2.type_ && tk1.lexeme_ == tk2.lexeme_ &&
         tk1.line_ == tk2.line_;
}

inline std::string TokenTypeToString(const TokenType& type) {
  switch (type) {
    case TokenType::ID:
      return "id";
    case TokenType::INTNUM:
      return "intNum";
    case TokenType::FLOATNUM:
      return "floatNum";
    case TokenType::EQ:
      return "eq";
    case TokenType::NEQ:
      return "neq";
    case TokenType::LT:
      return "lt";
    case TokenType::GT:
      return "gt";
    case TokenType::LEQ:
      return "leq";
    case TokenType::GEQ:
      return "geq";
    case TokenType::PLUS:
      return "+";
    case TokenType::MINUS:
      return "-";
    case TokenType::MULT:
      return "*";
    case TokenType::DIV:
      return "/";
    case TokenType::ASSGN:
      return "=";
    case TokenType::OPEN_PAR:
      return "(";
    case TokenType::CLOSE_PAR:
      return ")";
    case TokenType::OPEN_CBR:
      return "{";
    case TokenType::CLOSE_CBR:
      return "}";
    case TokenType::OPEN_SQBR:
      return "[";
    case TokenType::CLOSE_SQBR:
      return "]";
    case TokenType::SEMICOLON:
      return ";";
    case TokenType::COMMA:
      return ",";
    case TokenType::DOT:
      return ".";
    case TokenType::COLON:
      return ":";
    case TokenType::SCOPE_RES:
      return "sr";
    case TokenType::IF:
      return "if";
    case TokenType::THEN:
      return "then";
    case TokenType::ELSE:
      return "else";
    case TokenType::WHILE:
      return "while";
    case TokenType::CLASS:
      return "class";
    case TokenType::INTEGER:
      return "integer";
    case TokenType::FLOAT:
      return "float";
    case TokenType::DO:
      return "do";
    case TokenType::END:
      return "end";
    case TokenType::PUBLIC:
      return "public";
    case TokenType::PRIVATE:
      return "private";
    case TokenType::OR:
      return "or";
    case TokenType::AND:
      return "and";
    case TokenType::NOT:
      return "not";
    case TokenType::READ:
      return "read";
    case TokenType::WRITE:
      return "write";
    case TokenType::RETURN:
      return "return";
    case TokenType::MAIN:
      return "main";
    case TokenType::INHERITS:
      return "inherits";
    case TokenType::LOCAL:
      return "local";
    case TokenType::VOID:
      return "void";
    case TokenType::BLOCK_CMT:
      return "block_cmt";
    case TokenType::INLINE_CMT:
      return "inline_cmt";
    case TokenType::INVALID_ID:
      return "invalid_id";
    case TokenType::INVALID_NUM:
      return "invalid_num";
    case TokenType::INVALID_CHAR:
      return "invalid_char";
    case TokenType::UNTERMINATED_CMT:
      return "unterminated_cmt";
    case TokenType::EOS:
      return "$";
    default:
      throw std::invalid_argument("Unknown token type.");
  }
};

}  // namespace toy
