#include "parser.h"

#include <algorithm>

#include "logger.h"

namespace toy {

Parser::Parser(Lexer& lexer, const ParserGenerator& pgen)
    : lexer_(lexer), pgen_(pgen){};

// LL(1) table-driven parsing
std::shared_ptr<ASTNode> Parser::Parse() {
  std::ofstream derivations("../out/outderivation");
  Token tk = lexer_.NextToken();
  bool error = false;

  symbol_stack_.emplace(SymbolType::END);
  symbol_stack_.emplace(SymbolType::START);
  while (symbol_stack_.top().Type() != SymbolType::END) {
    // Skip over comment tokens
    while (tk.IsComment()) {
      tk = lexer_.NextToken();
    }
    Symbol top = symbol_stack_.top();
    derivations << top << std::endl;
    if (top.Type() == SymbolType::ACTION) {
      ExecuteSemanticAction(top.RawStr());
      symbol_stack_.pop();
    } else if (top.IsTerm()) {
      if (top == tk) {
        symbol_stack_.pop();
        prev_token_ = tk;
        tk = lexer_.NextToken();
      } else {
        SkipErrors(top, tk);
        error = true;
      }
    }
    // Push the correct inverse of the production on the stack
    else {
      if (pgen_.TableValid(top, tk)) {
        symbol_stack_.pop();
        InverseRhsMultiplePush(top, tk);
      } else {
        SkipErrors(top, tk);
        error = true;
      }
    }
  }
  if (lexer_.HasNext() || error == true) {
    derivations << "# Parsing failed" << std::endl;
    return semantic_stack_.top();
  } else {
    derivations << "# Parsing ok" << std::endl;
    return semantic_stack_.top();
  }
};

// This syntax error recovery is very primitive - just pops from the symbol
// stack and attempts to continue the parse.
// TODO: implement a better syntax error recovery mechanism (i.e. token
// synchronization)
void Parser::SkipErrors(Symbol& top, Token& tk) {
  Logger::Err(
      "Expected '" + top.Str() + "', but next token was '" + tk.Lexeme() + "'",
      tk.Line(), ErrorType::SYNTAX);
  symbol_stack_.pop();
}

void Parser::InverseRhsMultiplePush(Symbol top, Symbol tk) {
  Production prod = pgen_.TableAt(top, tk);
  // Push the reverse of the rhs on the stack
  std::vector<Symbol> rhs = prod.second;
  std::reverse(rhs.begin(), rhs.end());
  for (Symbol symb : rhs) {
    if (symb.Type() != SymbolType::EPSILON) symbol_stack_.push(symb);
  }
}

// In the grammar, actions are encoded in the form:
// !sem_<action-to-execute>_<node-type>!
// Where <action-to-execute> is one of:
// - push: create a new node with info from the previous token and push it on
// the semantic stack. Essentially creates leaf nodes.
// - start: push a nullptr on the stack to signify the start of a siblings list.
// - end: adopts all siblings until the marker node.
// - op: special case for addOp, assignOp, multOp.
void Parser::ExecuteSemanticAction(const std::string& action) {
  if (action.find("end_sign") != std::string::npos) {
    EndSignAction();
  } else if (action.find("end_scoperes") != std::string::npos) {
    EndScopeResAction(action);
  } else if (action.find("start") != std::string::npos) {
    StartAction();
  } else if (action.find("end") != std::string::npos) {
    EndAction(action);
  } else if (action.find("push") != std::string::npos) {
    PushAction(action);
  } else if (action.find("op") != std::string::npos) {
    OpAction();
  }
}

void Parser::StartAction() { semantic_stack_.push(nullptr); }

void Parser::EndAction(std::string action) {
  std::string node_kind = action.substr(8);
  // In this case we had nothing on the stack, just pop it
  if (semantic_stack_.top() == nullptr) {
    semantic_stack_.pop();
    auto node = ASTNode::MakeNode(node_kind);
    semantic_stack_.push(node);
  } else {
    auto parent = ASTNode::MakeNode(node_kind);

    std::stack<std::shared_ptr<ASTNode>> reverse;
    while (semantic_stack_.top() != nullptr) {
      reverse.push(semantic_stack_.top());
      semantic_stack_.pop();
    }
    while (reverse.size() > 0) {
      parent->AddChild(reverse.top());
      reverse.pop();
    }
    semantic_stack_.pop();  // pop the NULL OUT
    semantic_stack_.push(parent);
  }
}

void Parser::PushAction(const std::string& action) {
  std::shared_ptr<ASTNode> node = nullptr;
  if (action.find("type") != std::string::npos) {
    node = ASTNode::MakeNode("type", prev_token_.Lexeme(), prev_token_.Line());
  } else if (action.find("dim") != std::string::npos) {
    std::string val;
    if (prev_token_.Type() == TokenType::INTNUM) {
      val = prev_token_.Lexeme();
    } else {
      val = "";
    }
    node = ASTNode::MakeNode("dim", val, prev_token_.Line());
  } else {
    node = ASTNode::MakeNode(TokenTypeToString(prev_token_.Type()),
                             prev_token_.Lexeme(), prev_token_.Line());
  }
  semantic_stack_.push(node);
}

void Parser::OpAction() {
  auto n1 = semantic_stack_.top();
  semantic_stack_.pop();
  // n2 is the parent, e.g. addOp, multOp
  auto n2 = semantic_stack_.top();
  semantic_stack_.pop();
  auto n3 = semantic_stack_.top();
  semantic_stack_.pop();
  n2->AddChild(n3);
  n2->AddChild(n1);
  semantic_stack_.push(n2);
}

void Parser::EndSignAction() {
  std::shared_ptr<ASTNode> factor = semantic_stack_.top();
  semantic_stack_.pop();
  std::shared_ptr<ASTNode> child_node = semantic_stack_.top();
  semantic_stack_.pop();
  semantic_stack_.pop();  // pop the NULL out
  auto node = ASTNode::MakeNode("sign", child_node->Val(), child_node->Line());
  node->AddChild(factor);
  semantic_stack_.push(node);
}

void Parser::EndScopeResAction(std::string action) {
  std::shared_ptr<ASTNode> func_name_node;
  std::shared_ptr<ASTNode> scope_res_node;
  if (semantic_stack_.top() != nullptr) {
    func_name_node = semantic_stack_.top();
    semantic_stack_.pop();
    semantic_stack_.pop();
    scope_res_node = semantic_stack_.top();
    semantic_stack_.pop();
    semantic_stack_.push(func_name_node);
    semantic_stack_.push(nullptr);
    semantic_stack_.push(scope_res_node);
    EndAction(action);
  } else {
    EndAction(action);
  }
}

}  // namespace toy