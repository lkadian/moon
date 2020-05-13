#include "symbol_table_visitor.h"

#include <fstream>

#include "ast.h"
#include "logger.h"
#include "symbol_table.h"

namespace toy {

namespace {
bool IsClassType(const std::string& type) {
  return type != "integer" && type != "float";
}
}  // namespace

// dfs and propagate symbol tables and entries along the way.
void SymbolTableVisitor::DFS(ASTNode& node) {
  for (auto& child : node.Children()) {
    child->symtab = node.symtab;
    child->symtab_entry = node.symtab_entry;
    child->Accept(*this);
  }
}

bool SymbolTableVisitor::TSortUtil(std::string node,
                                   std::map<std::string, bool>& visited,
                                   std::vector<std::string>& sorted,
                                   std::map<std::string, bool>& recur_stack) {
  visited[node] = true;
  recur_stack[node] = true;
  bool has_cycle = false;
  for (auto& neighbor : dep_graph_[node]) {
    if (!visited[neighbor] &&
        TSortUtil(neighbor, visited, sorted, recur_stack)) {
      has_cycle = true;
    } else if (recur_stack[neighbor]) {
      has_cycle = true;
    }
  }
  sorted.push_back(node);
  recur_stack[node] = false;
  return has_cycle;
}

std::vector<std::string> SymbolTableVisitor::TSort() {
  std::map<std::string, bool> Visited;
  for (auto it = dep_graph_.begin(); it != dep_graph_.end(); ++it) {
    Visited[it->first] = false;
  }
  std::map<std::string, bool> recur_stack;
  for (auto it = dep_graph_.begin(); it != dep_graph_.end(); ++it) {
    recur_stack[it->first] = false;
  }
  // Vector used here instead of a stack because in our graph B inherits A is
  // represented as B -> A rather than A -> B
  std::vector<std::string> sorted;
  for (auto it = dep_graph_.begin(); it != dep_graph_.end(); ++it) {
    if (!Visited[it->first] &&
        TSortUtil(it->first, Visited, sorted, recur_stack)) {
      Logger::Err("Cyclic class dependency detected from class " + it->first,
                  ErrorType::SEMANTIC);
    }
  }
  return sorted;
}

void SymbolTableVisitor::CheckUndefinedMemberFunctions(
    SymbolTable& symtab, std::vector<std::string> sorted_classes) {
  for (auto& class_name : sorted_classes) {
    auto class_entry = symtab.GetEntry("class", class_name);
    if (!class_entry) {
      Logger::Err("Inherited class '" + class_name + "' not defined",
                  ErrorType::SEMANTIC);
      continue;
    }
    auto class_tab = class_entry->Link();
    for (auto it = class_tab->Begin(); it != class_tab->End(); ++it) {
      // Link to the corresponding function
      if (it->first.first == "memberFunc") {
        auto entry = std::dynamic_pointer_cast<MemberFuncEntry>(it->second);
        std::string func_signature = entry->Signature();
        auto found_entry = symtab.GetEntry("func", func_signature);
        if (!found_entry) {
          Logger::Err(
              "Declared function '" + func_signature + "' has no definition",
              it->second->Line(), ErrorType::SEMANTIC);
        }
      }
    }
  }
}

void SymbolTableVisitor::LinkMemberFunctionDefsToDecl(
    SymbolTable& symtab, std::vector<std::string> sorted_classes) {
  for (auto it = symtab.Begin(); it != symtab.End(); ++it) {
    auto entry = it->second;
    if (entry->Kind() == "func" && entry->Name() != "main") {
      auto freefunc = std::static_pointer_cast<FreeFuncEntry>(entry);
      std::string scoperes = freefunc->Scope();
      if (scoperes != "") {
        // Attempt to find the class that matches the scoperes.
        auto class_entry = symtab.GetEntry("class", scoperes);
        if (class_entry) {
          auto func_entry = class_entry->Link()->GetEntry(
              "memberFunc", freefunc->Signature());
          if (func_entry) {
            freefunc->Link()->SetParent(class_entry->Link());
            func_entry->SetLink(freefunc->Link());
            func_entry->Link()->IncrementLevel();
            entry = nullptr;
          } else {
            Logger::Err("Definition provided for undeclared member function '" +
                            freefunc->Signature() + "'",
                        freefunc->Line(), ErrorType::SEMANTIC);
          }
        } else {
          Logger::Err("The scope resolution operator '" + scoperes +
                          "' does not match any class",
                      entry->Line(), ErrorType::SEMANTIC);
        }
      }
    }
  }
}

// Get and copy inherited entries from parent classes
void SymbolTableVisitor::GetInheritedEntries(SymbolTable& symtab,
                                             std::vector<std::string> classes) {
  for (auto& class_name : classes) {
    auto class_entry = symtab.GetEntry("class", class_name);
    if (class_entry == nullptr) {
      Logger::Err(
          "Attempt to inherit a class that doesn't exist '" + class_name + "'",
          ErrorType::SEMANTIC);
      continue;
    }
    auto class_symtab = class_entry->Link();
    auto inherited_classes = std::dynamic_pointer_cast<InheritEntry>(
                                 class_symtab->GetEntry("inherit", "inherit"))
                                 ->InheritList();
    for (auto inh_class : inherited_classes) {
      auto inh_entry = symtab.GetEntry("class", inh_class);
      if (!inh_entry) {
        Logger::Err(
            "Attempt to inherit a class that doesn't exist '" + inh_class + "'",
            ErrorType::SEMANTIC);
        continue;
      }
      auto inh_symtab = inh_entry->Link();
      for (auto it = inh_symtab->Begin(); it != inh_symtab->End(); ++it) {
        if (it->first.first == "memberVar") {
          class_symtab->AddEntry(
              std::dynamic_pointer_cast<MemberVarEntry>(it->second));
        }
      }
    }
  }
  symtab.RemoveMemberFunctionDefinitions();
}

void SymbolTableVisitor::Visit(ProgNode& node) {
  node.symtab = std::make_shared<SymbolTable>("global", nullptr, 0);
  DFS(node);

  std::vector<std::string> sorted_classes = TSort();
  node.set_sorted_classes(sorted_classes);
  CheckUndefinedMemberFunctions(*node.symtab, sorted_classes);
  LinkMemberFunctionDefsToDecl(*node.symtab, sorted_classes);
  GetInheritedEntries(*node.symtab, sorted_classes);

  std::ofstream ofs("../out/outsymboltable");
  ofs << *node.symtab;
}

void SymbolTableVisitor::Visit(VarDeclNode& node) {
  DFS(node);
  std::string type = node.ChildAt(0)->Val();
  std::string name = node.ChildAt(1)->Val();
  auto dimlist = node.ChildAt(2);
  std::vector<std::string> dims;
  for (auto& dim : dimlist->Children()) {
    dims.push_back(dim->Val());
  }
  node.symtab_entry = std::make_shared<LocalVarEntry>(
      name, type, node.ChildAt(0)->Line(), nullptr, dims);
  node.symtab->AddEntry(
      std::static_pointer_cast<LocalVarEntry>(node.symtab_entry));
}

void SymbolTableVisitor::Visit(MemberVarDeclNode& node) {
  DFS(node);
  std::string visibility = node.ChildAt(0)->Val();
  std::string type = node.ChildAt(1)->Val();
  std::string name = node.ChildAt(2)->Val();
  auto dimlist = node.ChildAt(3)->Children();
  std::vector<std::string> dims;
  for (auto& dim : dimlist) {
    dims.push_back(dim->Val());
  }
  std::string class_name = node.symtab->Name();
  if (IsClassType(type)) {
    dep_graph_.at(class_name).push_back(type);
  }
  node.symtab_entry =
      std::make_shared<MemberVarEntry>(name, type, node.ChildAt(0)->Line(),
                                       nullptr, dims, class_name, visibility);
  node.symtab->AddEntry(
      std::static_pointer_cast<MemberVarEntry>(node.symtab_entry));
}

void SymbolTableVisitor::Visit(ClassNode& node) {
  std::string class_name = node.ChildAt(0)->Val();
  // Add class to dependency graph
  dep_graph_.insert({class_name, {}});
  auto inherit_list = node.ChildAt(1)->Children();
  for (auto& inherited : inherit_list) {
    dep_graph_.at(class_name).push_back(inherited->Val());
  }
  // Add class entry to global symbol table
  auto class_table = std::make_shared<SymbolTable>(class_name, node.symtab, 1);
  auto entry = std::make_shared<ClassEntry>(class_name, node.ChildAt(0)->Line(),
                                            class_table);
  node.symtab_entry = entry;
  node.symtab->AddEntry(std::static_pointer_cast<ClassEntry>(entry));
  node.symtab = class_table;
  DFS(node);
}

void SymbolTableVisitor::Visit(FuncDefNode& node) {
  std::string name = node.ChildAt(0)->Val();
  auto func_table = std::make_shared<SymbolTable>(name, node.symtab, 1);
  std::vector<std::pair<std::string, std::string>> params;
  for (auto& param : node.ChildAt(2)->Children()) {
    std::string param_type = param->ChildAt(0)->Val();
    std::string param_type_str = param_type;
    std::string param_name = param->ChildAt(1)->Val();
    auto dimlist = param->ChildAt(2)->Children();
    std::vector<std::string> dims;
    for (auto& dim : dimlist) {
      dims.push_back(dim->Val());
      param_type_str += "[]";
    }
    params.emplace_back(param_name, param_type_str);
    auto param_entry = std::shared_ptr<Entry>(new LocalVarEntry(
        param_name, param_type, node.ChildAt(0)->Line(), nullptr, dims));
    func_table->AddEntry(std::static_pointer_cast<LocalVarEntry>(param_entry));
  }
  std::string return_type = node.ChildAt(3)->Val();
  std::string scope;
  auto scope_res = node.ChildAt(1)->Children();
  if (scope_res.size() > 0) {
    scope = scope_res.at(0)->Val();
  }
  node.symtab_entry = std::make_shared<FreeFuncEntry>(
      name, return_type, node.ChildAt(0)->Line(), func_table, params, scope);
  node.symtab->AddEntry(
      std::static_pointer_cast<FreeFuncEntry>(node.symtab_entry));
  node.symtab = func_table;
  return_found_ = false;
  DFS(node);
  // Check if the function is missing a return statement
  if (!return_found_ && node.ChildAt(3)->Val() != "void") {
    std::vector<std::string> param_types;
    for (auto& p : params) {
      param_types.push_back(p.second);
    }
    Logger::Err("Missing return statement in function '" +
                    SymbolTable::GetFuncSignature(scope, name, param_types) +
                    " -> " + return_type + "'",
                node.ChildAt(0)->Line(), ErrorType::SEMANTIC);
  }
  return_found_ = false;
}

// Only creates the entry without the table, linking is done after traversal
void SymbolTableVisitor::Visit(MemberFuncDeclNode& node) {
  std::string visibility = node.ChildAt(0)->Val();
  std::string name = node.ChildAt(1)->Val();
  std::string type = node.ChildAt(3)->Val();
  auto func_params = node.ChildAt(2)->Children();
  std::vector<std::pair<std::string, std::string>> params;
  for (auto& param : func_params) {
    params.emplace_back(param->ChildAt(1)->Val(), param->ChildAt(0)->Val());
  }
  node.symtab_entry = std::make_shared<MemberFuncEntry>(
      name, type, node.ChildAt(0)->Line(), nullptr, params, node.symtab->Name(),
      visibility);
  node.symtab->AddEntry(
      std::static_pointer_cast<MemberFuncEntry>(node.symtab_entry));
  DFS(node);
}

void SymbolTableVisitor::Visit(InheritListNode& node) {
  std::vector<std::string> inherit_list;
  for (auto& child : node.Children()) {
    inherit_list.push_back(child->Val());
    child->symtab = node.symtab;
    child->Accept(*this);
  }
  node.symtab_entry = std::make_shared<InheritEntry>(inherit_list);
  node.symtab->AddEntry(
      std::static_pointer_cast<InheritEntry>(node.symtab_entry));
}

void SymbolTableVisitor::Visit(MainNode& node) {
  auto main_table = std::make_shared<SymbolTable>("main", node.symtab, 1);
  node.symtab_entry = std::shared_ptr<FreeFuncEntry>(new FreeFuncEntry(
      "main", "void", node.ChildAt(0)->ChildAt(0)->Line(), main_table, {}, ""));
  node.symtab->AddEntry(
      std::static_pointer_cast<FreeFuncEntry>(node.symtab_entry));
  node.symtab = main_table;
  DFS(node);
}

void SymbolTableVisitor::Visit(ReturnNode& node) {
  DFS(node);
  return_found_ = true;
}

// Visits below just perform dfs.
void SymbolTableVisitor::Visit(AParamsNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(AddOpNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(ArithExprNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(AssignNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(ClassListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(DataMemberNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(DimListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(DimNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FParamsListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FParamsNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FloatNumNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FuncBodyNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FuncCallNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(FuncDefListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(IdNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(IfStatNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(IndiceList& node) { DFS(node); }
void SymbolTableVisitor::Visit(IntNumNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(MemberListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(MultOpNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(NotNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(ReadNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(RelExprNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(RelOpNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(ScopeResNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(SignNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(StatListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(TypeNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(VarDeclListNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(VarNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(WhileNode& node) { DFS(node); }
void SymbolTableVisitor::Visit(WriteNode& node) { DFS(node); }

}  // namespace toy