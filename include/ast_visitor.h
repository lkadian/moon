#ifndef TOY_AST_VISITOR_H_
#define TOY_AST_VISITOR_H_

#include "ast.h"

namespace toy {

class ASTVisitor {
 public:
  ASTVisitor() = default;
  virtual void Visit(AParamsNode& node) = 0;
  virtual void Visit(AddOpNode& node) = 0;
  virtual void Visit(ArithExprNode& node) = 0;
  virtual void Visit(IndiceList& node) = 0;
  virtual void Visit(AssignNode& node) = 0;
  virtual void Visit(ClassListNode& node) = 0;
  virtual void Visit(ClassNode& node) = 0;
  virtual void Visit(DimListNode& node) = 0;
  virtual void Visit(DimNode& node) = 0;
  virtual void Visit(FParamsListNode& node) = 0;
  virtual void Visit(FParamsNode& node) = 0;
  virtual void Visit(FloatNumNode& node) = 0;
  virtual void Visit(FuncBodyNode& node) = 0;
  virtual void Visit(FuncCallNode& node) = 0;
  virtual void Visit(FuncDefListNode& node) = 0;
  virtual void Visit(FuncDefNode& node) = 0;
  virtual void Visit(IdNode& node) = 0;
  virtual void Visit(IfStatNode& node) = 0;
  virtual void Visit(InheritListNode& node) = 0;
  virtual void Visit(IntNumNode& node) = 0;
  virtual void Visit(MainNode& node) = 0;
  virtual void Visit(MemberFuncDeclNode& node) = 0;
  virtual void Visit(MemberListNode& node) = 0;
  virtual void Visit(MemberVarDeclNode& node) = 0;
  virtual void Visit(MultOpNode& node) = 0;
  virtual void Visit(NotNode& node) = 0;
  virtual void Visit(ProgNode& node) = 0;
  virtual void Visit(ReadNode& node) = 0;
  virtual void Visit(RelExprNode& node) = 0;
  virtual void Visit(RelOpNode& node) = 0;
  virtual void Visit(ReturnNode& node) = 0;
  virtual void Visit(ScopeResNode& node) = 0;
  virtual void Visit(SignNode& node) = 0;
  virtual void Visit(StatListNode& node) = 0;
  virtual void Visit(TypeNode& node) = 0;
  virtual void Visit(VarDeclListNode& node) = 0;
  virtual void Visit(VarDeclNode& node) = 0;
  virtual void Visit(DataMemberNode& node) = 0;
  virtual void Visit(VarNode& node) = 0;
  virtual void Visit(WhileNode& node) = 0;
  virtual void Visit(WriteNode& node) = 0;

 protected:
  virtual void DFS(ASTNode& node);
};

}  // namespace toy

#endif  // TOY_AST_VISITOR_H_