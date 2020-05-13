#include "type_check_visitor.h"

#include "logger.h"
#include "symbol_table.h"

namespace toy {

bool IsClassType(const std::string& type) {
  return type != "integer" && type != "float" && type != "typeerror";
}

void TypeCheckVisitor::Visit(AddOpNode& node) {
  DFS(node);
  auto left_type = node.Children().at(0)->Type();
  if (IsClassType(left_type)) {
    Logger::Err("Add operators not allowed on objects ('" + left_type + "')",
                node.Line(), ErrorType::SEMANTIC);
  }
  auto right_type = node.Children().at(1)->Type();
  if (IsClassType(right_type)) {
    Logger::Err("Add operators not allowed on objects ('" + right_type + "')",
                node.Line(), ErrorType::SEMANTIC);
  }
  if (left_type == right_type) {
    node.SetType(left_type);
  } else {
    node.SetType("typeerror");
    Logger::Err("Mismatching types '" + left_type + "' and '" + right_type +
                    "' in add operation",
                node.Line(), ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(MultOpNode& node) {
  DFS(node);
  auto left_type = node.Children().at(0)->Type();
  if (IsClassType(left_type)) {
    Logger::Err("Mult operators not allowed on objects ('" + left_type + "')",
                node.Line(), ErrorType::SEMANTIC);
  }
  auto right_type = node.Children().at(1)->Type();
  if (IsClassType(right_type)) {
    Logger::Err("Mult operators not allowed on objects ('" + right_type + "')",
                node.Line(), ErrorType::SEMANTIC);
  }
  if (left_type == right_type) {
    node.SetType(left_type);
  } else {
    node.SetType("typeerror");
    Logger::Err("Mismatching types '" + left_type + "' and '" + right_type +
                    "' in mult operation",
                node.Line(), ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(AssignNode& node) {
  DFS(node);
  auto left_type = node.Children().at(0)->Type();
  auto right_type = node.Children().at(1)->Type();
  if (left_type == right_type) {
    node.SetType(left_type);
  } else {
    node.SetType("typeerror");
    Logger::Err("Mismatching types '" + left_type + "' and '" + right_type +
                    "' in assign operation",
                node.Line(), ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(RelExprNode& node) {
  DFS(node);
  auto left_type = node.ChildAt(0)->Type();
  if (IsClassType(left_type)) {
    Logger::Err("Rel operators not allowed on objects ('" + left_type + "')",
                node.ChildAt(1)->Line(), ErrorType::SEMANTIC);
  }
  auto right_type = node.Children().at(2)->Type();
  if (IsClassType(right_type)) {
    Logger::Err("Rel operators not allowed on objects ('" + right_type + "')",
                node.ChildAt(1)->Line(), ErrorType::SEMANTIC);
  }
  if (left_type == right_type) {
    node.SetType(left_type);
  } else {
    node.SetType("typeerror");
    Logger::Err("Mismatching types '" + left_type + "' and '" + right_type +
                    "' in rel expression",
                node.ChildAt(1)->Line(), ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(DataMemberNode& node) {
  DFS(node);
  std::string var_name = node.ChildAt(0)->Val();
  int line = node.ChildAt(0)->Line();

  // dot operator should only be used for class types
  std::string class_name = node.Type();
  if (!IsClassType(class_name)) {
    Logger::Err("dot operator can only be used on variables of class type",
                line, ErrorType::SEMANTIC);
    node.SetType("typeerror");
    return;
  }
  node.cls = class_name;

  std::shared_ptr<Entry> entry = node.symtab->GetEntry("memberVar", var_name);
  if (!entry) {
    if (class_name == "") {
      entry = node.symtab->GetEntry("local", var_name);
    } else {
      auto classentry = node.symtab->GetEntry("class", class_name);
      if (classentry) {
        entry = classentry->Link()->GetEntry("memberVar", var_name);
        if (entry) {
          if (std::dynamic_pointer_cast<MemberVarEntry>(entry)->Visibility() ==
              "private") {
            Logger::Err("Can't access private variables from outside the class",
                        line, ErrorType::SEMANTIC);
            return;
          }
        } else {
          Logger::Err("Undeclared data member '" + var_name +
                          "' not found in '" + class_name + "'",
                      line, ErrorType::SEMANTIC);
          node.SetType("typerror");
          return;
        }
      } else {
        Logger::Err("Undefined class " + class_name, line, ErrorType::SEMANTIC);
        node.SetType("typerror");
        return;
      }
    }
  }

  if (entry) {
    std::string type = entry->Type();
    auto dimlist = entry->Dims();
    auto indicelist = node.ChildAt(1)->Children();
    for (auto indice : indicelist) {
      if (indice->Type() != "integer") {
        Logger::Err("Expresions used as an index must be of integer type", line,
                    ErrorType::SEMANTIC);
      }
    }
    int resulting_dims = dimlist.size() - indicelist.size();
    if (resulting_dims >= 0) {
      for (int i = 0; i < resulting_dims; i++) {
        type += "[]";
      }
      node.SetType(type);
      node.symtab_entry = entry;
    } else {
      Logger::Err("Cannot refer to an array of dimension " +
                      std::to_string(dimlist.size()) + " with " +
                      std::to_string(indicelist.size()) + " indices",
                  line, ErrorType::SEMANTIC);
      node.SetType("typeerror");
    }
  } else {
    node.SetType("typeerror");
    Logger::Err("Use of undeclared variable '" + var_name + "'", line,
                ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(FuncCallNode& node) {
  DFS(node);
  // We check for the type given from our lhs when dot operator is used
  std::string class_name = node.Type();
  std::string func_name = node.ChildAt(0)->Val();
  int line = node.ChildAt(0)->Line();
  std::vector<std::string> fparams;
  for (auto& param : node.ChildAt(1)->Children()) {
    fparams.push_back(param->Type());
  }
  std::string func_signature = SymbolTable::GetFuncSignature(
      class_name, node.ChildAt(0)->Val(), fparams);

  // dot operator should only be used for class types
  if (!IsClassType(class_name)) {
    Logger::Err("dot operator can only be used on variables of class type",
                line, ErrorType::SEMANTIC);
    node.SetType("typeerror");
    return;
  }
  node.cls = class_name;

  std::shared_ptr<Entry> entry =
      node.symtab->GetEntry("memberFunc", func_signature);
  if (!entry) {
    if (class_name == "") {
      entry = node.symtab->GetEntry("func", func_signature);
    } else {
      auto classentry = node.symtab->GetEntry("class", class_name);
      if (classentry) {
        entry = classentry->Link()->GetEntry("memberFunc", func_signature);
        if (entry) {
          if (std::dynamic_pointer_cast<MemberFuncEntry>(entry)->Visibility() ==
              "private") {
            Logger::Err("Can't access private functions from outside the class",
                        line, ErrorType::SEMANTIC);
            return;
          }
        } else {
          Logger::Err("Undeclared member function '" + func_signature +
                          "' not found in '" + class_name + "'",
                      line, ErrorType::SEMANTIC);
          node.SetType("typerror");
          return;
        }
      } else {
        Logger::Err("Undefined class " + class_name, line, ErrorType::SEMANTIC);
        node.SetType("typerror");
        return;
      }
    }
  }

  if (entry) {
    std::string type = entry->Type();
    node.SetType(type);
  } else {
    Logger::Err("Use of undeclared free function '" + func_signature + "'",
                line, ErrorType::SEMANTIC);
    node.SetType("typeerror");
  }
}

void TypeCheckVisitor::Visit(VarNode& node) {
  // For chaining using dot operator, propagate
  // the type information from left to right
  auto num_children = node.Children().size();
  for (uint i = 0; i < num_children; i++) {
    node.ChildAt(i)->Accept(*this);
    if (i < num_children - 1) {
      node.ChildAt(i + 1)->SetType(node.ChildAt(i)->Type());
    }
  }
  node.SetType(node.ChildAt(num_children - 1)->Type());
}

void TypeCheckVisitor::Visit(ReturnNode& node) {
  DFS(node);
  std::string expected_return = node.symtab_entry->Type();
  std::string actual_return = node.ChildAt(0)->Type();
  if (expected_return != actual_return) {
    Logger::Err("Function '" + node.symtab->Name() + "' has return type '" +
                    expected_return + "' but returns '" + actual_return + "'",
                node.ChildAt(0)->Line(), ErrorType::SEMANTIC);
  }
}

void TypeCheckVisitor::Visit(SignNode& node) {
  DFS(node);
  node.SetType(node.ChildAt(0)->Type());
}
void TypeCheckVisitor::Visit(NotNode& node) {
  DFS(node);
  node.SetType(node.ChildAt(0)->Type());
}

void TypeCheckVisitor::Visit(ArithExprNode& node) {
  DFS(node);
  node.SetLine(node.ChildAt(0)->Line());
  node.SetType(node.ChildAt(0)->Type());
}

void TypeCheckVisitor::Visit(MemberVarDeclNode& node) {
  DFS(node);
  std::string type = node.ChildAt(1)->Val();
  if (type != "integer" && type != "float") {
    if (!node.symtab->GetEntry("class", type)) {
      Logger::Err("Undefined type " + type, node.ChildAt(0)->Line(),
                  ErrorType::SEMANTIC);
    }
  }
}

void TypeCheckVisitor::Visit(VarDeclNode& node) {
  DFS(node);
  std::string type = node.ChildAt(0)->Val();
  if (type != "integer" && type != "float") {
    if (!node.symtab->GetEntry("class", type)) {
      Logger::Err("Undefined type " + type, node.ChildAt(0)->Line(),
                  ErrorType::SEMANTIC);
    }
  }
}

void TypeCheckVisitor::Visit(IntNumNode& node) {
  DFS(node);
  node.SetType("integer");
}

void TypeCheckVisitor::Visit(FloatNumNode& node) {
  DFS(node);
  node.SetType("float");
}

// Just dfs
void TypeCheckVisitor::Visit(AParamsNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(ClassListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(ClassNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(DimListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(DimNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(FParamsListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(FParamsNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(FuncBodyNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(FuncDefListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(FuncDefNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(IdNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(IfStatNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(IndiceList& node) { DFS(node); }
void TypeCheckVisitor::Visit(InheritListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(MainNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(MemberFuncDeclNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(MemberListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(ProgNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(ReadNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(RelOpNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(ScopeResNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(StatListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(TypeNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(VarDeclListNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(WhileNode& node) { DFS(node); }
void TypeCheckVisitor::Visit(WriteNode& node) { DFS(node); }

}  // namespace toy