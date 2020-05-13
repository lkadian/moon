#include "ast.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "ast_visitor.h"
#include "lexer.h"

namespace toy {

ASTNode::ASTNode()
    : symtab(nullptr),
      symtab_entry(nullptr),
      val_(),
      line_(),
      type_(),
      children_(){};

ASTNode::ASTNode(std::string val, int line)
    : symtab(nullptr),
      symtab_entry(nullptr),
      val_(val),
      line_(line),
      type_(),
      children_(){};

std::vector<std::shared_ptr<ASTNode>> ASTNode::Children() const {
  return children_;
}

std::shared_ptr<ASTNode> ASTNode::ChildAt(int idx) const {
  return children_.at(idx);
}

void ASTNode::AddChild(const std::shared_ptr<ASTNode>& child) {
  children_.push_back(child);
}

std::string ASTNode::Type() const { return type_; };
std::string ASTNode::Val() const { return val_; };
int ASTNode::Line() const { return line_; };

void ASTNode::SetType(std::string type) { type_ = type; }
void ASTNode::SetVal(std::string val) { val_ = val; }
void ASTNode::SetLine(int line) { line_ = line; }

void ASTNode::ToDotFile(std::string filepath) {
  int node_id = 0;
  // lambda function for preorder traversal
  std::function<void(std::shared_ptr<ASTNode>, std::ofstream&)> preorder;
  preorder = [&](std::shared_ptr<ASTNode> node, std::ofstream& ofs) {
    if (node == nullptr) return;
    auto curr_id = node_id;
    ofs << curr_id << "[label=\"" << *node << "\"]" << std::endl;
    for (auto& child : node->Children()) {
      node_id++;
      ofs << curr_id << " -> " << node_id << std::endl;
      preorder(child, ofs);
    }
  };
  std::ofstream ofs(filepath);
  ofs << "digraph ast {" << std::endl;
  ofs << "node [shape=record];\n";
  preorder(shared_from_this(), ofs);
  ofs << "}";
}

std::ostream& operator<<(std::ostream& os, const ASTNode& node) {
  os << node.ToStr();
  return os;
}

std::shared_ptr<ASTNode> ASTNode::MakeNode(const std::string& kind) {
  if (kind == "aparams") {
    return std::make_shared<AParamsNode>();
  } else if (kind == "arithexpr") {
    return std::make_shared<ArithExprNode>();
  } else if (kind == "class") {
    return std::make_shared<ClassNode>();
  } else if (kind == "classlist") {
    return std::make_shared<ClassListNode>();
  } else if (kind == "datamember") {
    return std::make_shared<DataMemberNode>();
  } else if (kind == "dimlist") {
    return std::make_shared<DimListNode>();
  } else if (kind == "fcall") {
    return std::make_shared<FuncCallNode>();
  } else if (kind == "fparams") {
    return std::make_shared<FParamsNode>();
  } else if (kind == "fparamslist") {
    return std::make_shared<FParamsListNode>();
  } else if (kind == "funcbody") {
    return std::make_shared<FuncBodyNode>();
  } else if (kind == "funcdef") {
    return std::make_shared<FuncDefNode>();
  } else if (kind == "funcdeflist") {
    return std::make_shared<FuncDefListNode>();
  } else if (kind == "ifstat") {
    return std::make_shared<IfStatNode>();
  } else if (kind == "indicelist") {
    return std::make_shared<IndiceList>();
  } else if (kind == "inheritlist") {
    return std::make_shared<InheritListNode>();
  } else if (kind == "main") {
    return std::make_shared<MainNode>();
  } else if (kind == "memberfuncdecl") {
    return std::make_shared<MemberFuncDeclNode>();
  } else if (kind == "membervardecl") {
    return std::make_shared<MemberVarDeclNode>();
  } else if (kind == "memblist") {
    return std::make_shared<MemberListNode>();
  } else if (kind == "not") {
    return std::make_shared<NotNode>();
  } else if (kind == "prog") {
    return std::make_shared<ProgNode>();
  } else if (kind == "read") {
    return std::make_shared<ReadNode>();
  } else if (kind == "relexpr") {
    return std::make_shared<RelExprNode>();
  } else if (kind == "return") {
    return std::make_shared<ReturnNode>();
  } else if (kind == "scoperes") {
    return std::make_shared<ScopeResNode>();
  } else if (kind == "statlist") {
    return std::make_shared<StatListNode>();
  } else if (kind == "var") {
    return std::make_shared<VarNode>();
  } else if (kind == "vardecl") {
    return std::make_shared<VarDeclNode>();
  } else if (kind == "vardecl") {
    return std::make_shared<VarDeclNode>();
  } else if (kind == "vardecllist") {
    return std::make_shared<VarDeclListNode>();
  } else if (kind == "while") {
    return std::make_shared<WhileNode>();
  } else if (kind == "write") {
    return std::make_shared<WriteNode>();
  } else {
    throw std::invalid_argument(kind + " is not a valid ASTNode kind.");
  }
}

std::shared_ptr<ASTNode> ASTNode::MakeNode(const std::string& kind,
                                           const std::string& val, int line) {
  if (kind == "sign") {
    return std::make_shared<SignNode>(val, line);
  } else if (kind == "dim") {
    return std::make_shared<DimNode>(val, line);
  } else if (kind == "type") {
    return std::make_shared<TypeNode>(val, line);
  } else if (kind == "intNum") {
    return std::make_shared<IntNumNode>(val, line);
  } else if (kind == "+" || kind == "-" || kind == "or") {
    return std::make_shared<AddOpNode>(val, line);
  } else if (kind == "*" || kind == "/" || kind == "and") {
    return std::make_shared<MultOpNode>(val, line);
  } else if (kind == "floatNum") {
    return std::make_shared<FloatNumNode>(val, line);
  } else if (kind == "id" || Lexer::IsReserved(kind)) {
    return std::make_shared<IdNode>(val, line);
  } else if (kind == "=") {
    return std::make_shared<AssignNode>(val, line);
  } else if (kind == "eq" || kind == "neq" || kind == "lt" || kind == "gt" ||
             kind == "leq" || kind == "geq") {
    return std::make_shared<RelOpNode>(val, line);
  } else {
    throw std::invalid_argument(kind + " is not a valid leaf ASTNode kind.");
  }
}

AddOpNode::AddOpNode(std::string val, int line) : ASTNode(val, line){};
void AddOpNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string AddOpNode::ToStr() const { return "addOp | " + Val(); }

AParamsNode::AParamsNode() : ASTNode(){};
void AParamsNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string AParamsNode::ToStr() const { return "aParams"; }

ArithExprNode::ArithExprNode() : ASTNode(){};
void ArithExprNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ArithExprNode::ToStr() const { return "arithExpr"; }

IndiceList::IndiceList() : ASTNode(){};
void IndiceList::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string IndiceList::ToStr() const { return "indiceList"; }

AssignNode::AssignNode(std::string val, int line) : ASTNode(val, line){};
void AssignNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string AssignNode::ToStr() const { return "assign"; }

ClassNode::ClassNode() : ASTNode(){};
void ClassNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ClassNode::ToStr() const { return "class"; }

ClassListNode::ClassListNode() : ASTNode(){};
void ClassListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ClassListNode::ToStr() const { return "classList"; }

DataMemberNode::DataMemberNode() : ASTNode(){};
void DataMemberNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string DataMemberNode::ToStr() const { return "dataMember"; }

VarNode::VarNode() : ASTNode(){};
void VarNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string VarNode::ToStr() const { return "var"; }

DimListNode::DimListNode() : ASTNode(){};
void DimListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string DimListNode::ToStr() const { return "dimList"; }

DimNode::DimNode(std::string val, int line) : ASTNode(val, line){};
void DimNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string DimNode::ToStr() const { return "dim | " + Val(); }

FloatNumNode::FloatNumNode(std::string val, int line) : ASTNode(val, line){};
void FloatNumNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FloatNumNode::ToStr() const { return "floatNum | " + Val(); }

FParamsNode::FParamsNode() : ASTNode(){};
void FParamsNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FParamsNode::ToStr() const { return "fParams"; }

FParamsListNode::FParamsListNode() : ASTNode(){};
void FParamsListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FParamsListNode::ToStr() const { return "fParamsList"; }

FuncBodyNode::FuncBodyNode() : ASTNode(){};
void FuncBodyNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FuncBodyNode::ToStr() const { return "funcBody"; }

FuncCallNode::FuncCallNode() : ASTNode(){};
void FuncCallNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FuncCallNode::ToStr() const { return "fCall"; }

FuncDefNode::FuncDefNode() : ASTNode(){};
void FuncDefNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FuncDefNode::ToStr() const { return "funcDef"; }

FuncDefListNode::FuncDefListNode() : ASTNode(){};
void FuncDefListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string FuncDefListNode::ToStr() const { return "funcDefList"; }

IdNode::IdNode(std::string val, int line) : ASTNode(val, line){};
void IdNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string IdNode::ToStr() const { return "id | " + Val(); }

IfStatNode::IfStatNode() : ASTNode(){};
void IfStatNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string IfStatNode::ToStr() const { return "ifStat"; }

InheritListNode::InheritListNode() : ASTNode(){};
void InheritListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string InheritListNode::ToStr() const { return "inheritList"; }

IntNumNode::IntNumNode(std::string val, int line) : ASTNode(val, line){};
void IntNumNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string IntNumNode::ToStr() const { return "intNum | " + Val(); }

MainNode::MainNode() : ASTNode(){};
void MainNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string MainNode::ToStr() const { return "main"; }

MemberFuncDeclNode::MemberFuncDeclNode() : ASTNode(){};
void MemberFuncDeclNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string MemberFuncDeclNode::ToStr() const { return "memFuncDecl"; }

MemberListNode::MemberListNode() : ASTNode(){};
void MemberListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string MemberListNode::ToStr() const { return "memberList"; }

MemberVarDeclNode::MemberVarDeclNode() : ASTNode(){};
void MemberVarDeclNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string MemberVarDeclNode::ToStr() const { return "memVarDecl"; }

MultOpNode::MultOpNode(std::string val, int line) : ASTNode(val, line){};
void MultOpNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string MultOpNode::ToStr() const { return "multOp | " + Val(); }

NotNode::NotNode() : ASTNode(){};
void NotNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string NotNode::ToStr() const { return "not"; }

ProgNode::ProgNode() : ASTNode(){};
void ProgNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ProgNode::ToStr() const { return "prog"; }
void ProgNode::set_sorted_classes(std::vector<std::string> sorted_classes) {
  sorted_classes_ = sorted_classes;
}
std::vector<std::string> ProgNode::sorted_classes() { return sorted_classes_; }

ReadNode::ReadNode() : ASTNode(){};
void ReadNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ReadNode::ToStr() const { return "read"; }

RelExprNode::RelExprNode() : ASTNode(){};
void RelExprNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string RelExprNode::ToStr() const { return "relExpr"; }

RelOpNode::RelOpNode(std::string val, int line) : ASTNode(val, line){};
void RelOpNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string RelOpNode::ToStr() const {
  std::string str;
  if (Val() == "<>") {
    str = "\\<\\>";
  } else {
    str = "\\" + Val();
  }
  return "relOp | " + str;
}

ReturnNode::ReturnNode() : ASTNode(){};
void ReturnNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ReturnNode::ToStr() const { return "return"; }

ScopeResNode::ScopeResNode() : ASTNode(){};
void ScopeResNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string ScopeResNode::ToStr() const { return "scopeRes"; }

SignNode::SignNode(std::string val, int line) : ASTNode(val, line){};
void SignNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string SignNode::ToStr() const { return "sign | " + Val(); }

StatListNode::StatListNode() : ASTNode(){};
void StatListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string StatListNode::ToStr() const { return "statList"; }

TypeNode::TypeNode(std::string val, int line) : ASTNode(val, line){};
void TypeNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string TypeNode::ToStr() const { return "type | " + Val(); }

VarDeclNode::VarDeclNode() : ASTNode(){};
void VarDeclNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string VarDeclNode::ToStr() const { return "varDecl"; }

VarDeclListNode::VarDeclListNode() : ASTNode(){};
void VarDeclListNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string VarDeclListNode::ToStr() const { return "varDeclList"; }

WhileNode::WhileNode() : ASTNode(){};
void WhileNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string WhileNode::ToStr() const { return "while"; }

WriteNode::WriteNode() : ASTNode(){};
void WriteNode::Accept(ASTVisitor& v) { v.Visit(*this); }
std::string WriteNode::ToStr() const { return "write"; }

}  // namespace toy
