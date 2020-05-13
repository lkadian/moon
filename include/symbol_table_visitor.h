#ifndef TOY_SYMBOL_TABLE_VISITOR_H_
#define TOY_SYMBOL_TABLE_VISITOR_H_

#include <unordered_map>
#include <vector>

#include "ast_visitor.h"

namespace toy {

/**
 * Visitor to traverse the AST and create the program's symbol table.
 */
class SymbolTableVisitor : public ASTVisitor {
 public:
  void Visit(AParamsNode& node) override;
  void Visit(AddOpNode& node) override;
  void Visit(ArithExprNode& node) override;
  void Visit(AssignNode& node) override;
  void Visit(ClassListNode& node) override;
  void Visit(ClassNode& node) override;
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
  void Visit(DataMemberNode& node) override;
  void Visit(VarNode& node) override;
  void Visit(WhileNode& node) override;
  void Visit(WriteNode& node) override;

 private:
  virtual void DFS(ASTNode& node) override;

  std::unordered_map<std::string, std::vector<std::string>> dep_graph_;

  // Performs a topological sort of the class dependancy graph
  std::vector<std::string> TSort();
  bool TSortUtil(std::string node, std::map<std::string, bool>& visited,
                 std::vector<std::string>& sorted,
                 std::map<std::string, bool>& recur_stack);

  // Check for declared member functions without a definition
  void CheckUndefinedMemberFunctions(SymbolTable& symtab,
                                     std::vector<std::string> sorted_classes);
  // Link member function definitions to their declarations
  void LinkMemberFunctionDefsToDecl(SymbolTable& symtab,
                                    std::vector<std::string> sorted_classes);

  void GetInheritedEntries(SymbolTable& symtab,
                           std::vector<std::string> classes);

  bool return_found_;
};

}  // namespace toy

#endif  // TOY_SYMBOL_TABLE_VISITOR_H_