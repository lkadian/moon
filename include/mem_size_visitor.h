#ifndef TOY_MEM_SIZE_VISITOR_H_
#define TOY_MEM_SIZE_VISITOR_H_

#include <unordered_map>

#include "ast_visitor.h"

namespace toy {

/**
 * Visitor to traverse the AST and calculates the memory to be allocated.
 */
class MemSizeVisitor : public ASTVisitor {
 public:
  MemSizeVisitor();
  void Visit(AParamsNode& node) override;
  void Visit(AddOpNode& node) override;
  void Visit(ArithExprNode& node) override;
  void Visit(AssignNode& node) override;
  void Visit(ClassListNode& node) override;
  void Visit(ClassNode& node) override;
  void Visit(DataMemberNode& node) override;
  void Visit(DimListNode& node) override;
  void Visit(DimNode& node) override;
  void Visit(FParamsListNode& node) override;
  void Visit(FParamsNode& node) override;
  void Visit(FloatNumNode& node) override;
  void Visit(FuncBodyNode& node) override;
  void Visit(FuncCallNode& node) override;
  void Visit(FuncDefListNode& node) override;
  void Visit(FuncDefNode& node) override;
  void Visit(IdNode& node) override;
  void Visit(IfStatNode& node) override;
  void Visit(IndiceList& node) override;
  void Visit(InheritListNode& node) override;
  void Visit(IntNumNode& node) override;
  void Visit(MainNode& node) override;
  void Visit(MemberFuncDeclNode& node) override;
  void Visit(MemberListNode& node) override;
  void Visit(MemberVarDeclNode& node) override;
  void Visit(MultOpNode& node) override;
  void Visit(NotNode& node) override;
  void Visit(ProgNode& node) override;
  void Visit(ReadNode& node) override;
  void Visit(RelExprNode& node) override;
  void Visit(RelOpNode& node) override;
  void Visit(ReturnNode& node) override;
  void Visit(ScopeResNode& node) override;
  void Visit(SignNode& node) override;
  void Visit(StatListNode& node) override;
  void Visit(TypeNode& node) override;
  void Visit(VarDeclListNode& node) override;
  void Visit(VarDeclNode& node) override;
  void Visit(VarNode& node) override;
  void Visit(WhileNode& node) override;
  void Visit(WriteNode& node) override;

 private:
  std::unordered_map<std::string, int> type_sizes_;
  void InitTypeSizes(ProgNode& node);
  int GetEntrySize(const Entry& entry);

  // Generate a unique tempvar name
  std::string GetTempVarName();
  // Generate a unique literal value name
  std::string GetLitValName();
  int temp_var_counter;
  int lit_val_counter;
};

}  // namespace toy

#endif  // TOY_MEM_SIZE_VISITOR_H_