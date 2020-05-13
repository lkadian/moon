#ifndef TOY_AST_H_
#define TOY_AST_H_

#include <memory>

#include "symbol_table.h"

namespace toy {

class ASTVisitor;

/**
 * Base class for all AST nodes.
 */
class ASTNode : public std::enable_shared_from_this<ASTNode> {
 public:
  // Non-leaf node ctor
  ASTNode();
  // Leaf node ctor
  ASTNode(std::string val, int line);

  std::vector<std::shared_ptr<ASTNode>> Children() const;
  std::shared_ptr<ASTNode> ChildAt(int idx) const;
  void AddChild(const std::shared_ptr<ASTNode>& child);

  std::string Type() const;
  std::string Val() const;
  int Line() const;

  void SetType(std::string type);
  void SetVal(std::string val);
  void SetLine(int line);

  // Introduced for semantic analysis and code generation
  std::shared_ptr<SymbolTable> symtab;
  std::shared_ptr<Entry> symtab_entry;
  // Register used for array offset calculations.
  // By default this is r0, meaning no offset.
  std::string regist = "r0";

  // Print this AST to a dot file to view with graphviz
  void ToDotFile(std::string filepath);
  virtual std::string ToStr() const = 0;

  virtual void Accept(ASTVisitor& v) = 0;

  // Factory method for non-leaf AST nodes
  static std::shared_ptr<ASTNode> MakeNode(const std::string& kind);
  // Factory method for leaf AST nodes
  static std::shared_ptr<ASTNode> MakeNode(const std::string& kind,
                                           const std::string& val, int line);

  friend std::ostream& operator<<(std::ostream& os, const ASTNode& node);

 private:
  std::string val_;   // Lexical value (for leaf nodes)
  int line_;          // Source code location (for leaf nhdes)
  std::string type_;  // The type of the node (for type checking of expressions)
  std::vector<std::shared_ptr<ASTNode>> children_;
};
std::ostream& operator<<(std::ostream& os, const ASTNode& node);

// Add operation, one of '+', '-', 'or'.
class AddOpNode : public ASTNode {
 public:
  AddOpNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Actual parameters, aka the input supplied at function call.
class AParamsNode : public ASTNode {
 public:
  AParamsNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class ArithExprNode : public ASTNode {
 public:
  ArithExprNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Indice list, used when trying to access elements in an array.
class IndiceList : public ASTNode {
 public:
  IndiceList();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class AssignNode : public ASTNode {
 public:
  AssignNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Class declaration.
class ClassNode : public ASTNode {
 public:
  ClassNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Class declaration list.
class ClassListNode : public ASTNode {
 public:
  ClassListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class DataMemberNode : public ASTNode {
 public:
  DataMemberNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
  std::string cls;
};

// Variable (data member) or function call
// Children are DataMemberNode and FuncCall
class VarNode : public ASTNode {
 public:
  VarNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
  int mem_loc;
};

// Dimension list, used when declaring a new array.
class DimListNode : public ASTNode {
 public:
  DimListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Dimension of an array, used when declaring a new array.
class DimNode : public ASTNode {
 public:
  DimNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Float number, e.g. (floatNum, 2.0e10)
class FloatNumNode : public ASTNode {
 public:
  FloatNumNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Formal arguments, used to specify the parameters that
// a function takes in the function declaration.
class FParamsNode : public ASTNode {
 public:
  FParamsNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Formal arguments list.
class FParamsListNode : public ASTNode {
 public:
  FParamsListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class FuncBodyNode : public ASTNode {
 public:
  FuncBodyNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class FuncCallNode : public ASTNode {
 public:
  FuncCallNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
  std::string cls;
};

class FuncDefNode : public ASTNode {
 public:
  FuncDefNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class FuncDefListNode : public ASTNode {
 public:
  FuncDefListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class IdNode : public ASTNode {
 public:
  IdNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class IfStatNode : public ASTNode {
 public:
  IfStatNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class InheritListNode : public ASTNode {
 public:
  InheritListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class IntNumNode : public ASTNode {
 public:
  IntNumNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class MainNode : public ASTNode {
 public:
  MainNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class MemberFuncDeclNode : public ASTNode {
 public:
  MemberFuncDeclNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class MemberListNode : public ASTNode {
 public:
  MemberListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class MemberVarDeclNode : public ASTNode {
 public:
  MemberVarDeclNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Multiplication operation, one of '*', '/', 'and'.
class MultOpNode : public ASTNode {
 public:
  MultOpNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class NotNode : public ASTNode {
 public:
  NotNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class ProgNode : public ASTNode {
 public:
  ProgNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
  void set_sorted_classes(std::vector<std::string> sorted_classes);
  std::vector<std::string> sorted_classes();

 private:
  std::vector<std::string> sorted_classes_;
};

class ReadNode : public ASTNode {
 public:
  ReadNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class RelExprNode : public ASTNode {
 public:
  RelExprNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Relational operation, one of 'eq', 'neq', 'lt', 'gt', 'leq', 'geq'.
class RelOpNode : public ASTNode {
 public:
  RelOpNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class ReturnNode : public ASTNode {
 public:
  ReturnNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class ScopeResNode : public ASTNode {
 public:
  ScopeResNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Sign, '+' or '-'
class SignNode : public ASTNode {
 public:
  SignNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class StatListNode : public ASTNode {
 public:
  StatListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Type, one of 'integer', 'float', 'id'
class TypeNode : public ASTNode {
 public:
  TypeNode(std::string val, int line);
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Local variable declaration
class VarDeclNode : public ASTNode {
 public:
  VarDeclNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

// Local variable declaration list
class VarDeclListNode : public ASTNode {
 public:
  VarDeclListNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class WhileNode : public ASTNode {
 public:
  WhileNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

class WriteNode : public ASTNode {
 public:
  WriteNode();
  void Accept(ASTVisitor& v) override;
  std::string ToStr() const override;
};

}  // namespace toy

#endif  // TOY_AST_H_