#include "ast_visitor.h"

namespace toy {

void ASTVisitor::DFS(ASTNode& node) {
  for (auto& child : node.Children()) {
    child->Accept(*this);
  }
}

}  // namespace toy