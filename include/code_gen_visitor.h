#ifndef TOY_CODE_GEN_VISITOR_H_
#define TOY_CODE_GEN_VISITOR_H_

#include <stack>
#include <unordered_map>

#include "ast_visitor.h"

namespace toy {

/**
 * Visitor to traverse the AST and generate the moon code.
 */
class CodeGenVisitor : public ASTVisitor {
 public:
  CodeGenVisitor();
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
  std::stack<std::string> register_pool_;
  std::stack<std::string> offset_register_pool_;
  std::string exec_code_;
  std::string data_code_;
  std::string code_indent_;
  std::string procedures_;
  std::unordered_map<std::string, std::string> procedure_map_;
  std::unordered_map<std::string, std::string> InitProcedureMap();
  void AddExecLine(const std::string& line);
  void AddComment(const std::string& comment);
  void AddHeaderComment(const std::string& comment);
  void AddLibProcedure(const std::string& name);
  void StartOffsetIf(const ASTNode& node);
  void EndOffsetIf(ASTNode& node);
  std::string GetInstructionFromOp(const std::string& tk);
  void HandleOr(AddOpNode& node);
  void HandleAnd(MultOpNode& node);

  int ifstat_num_;
  int not_num_;
  int and_num_;
  int or_num_;
  int GetIfStatNum();
  int GetNotNum();
  int GetAndNum();
  int GetOrNum();
};

}  // namespace toy

#endif  // TOY_CODE_GEN_VISITOR_H_
