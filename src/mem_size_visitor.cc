#include "mem_size_visitor.h"

#include <fstream>

#include "symbol_table.h"

namespace toy {

MemSizeVisitor::MemSizeVisitor()
    : type_sizes_(), temp_var_counter(1), lit_val_counter(1){};

// tempvars are used to store intermediate results,
// e.g. the result of an add operation
std::string MemSizeVisitor::GetTempVarName() {
  return "temp" + std::to_string(temp_var_counter++);
}

std::string MemSizeVisitor::GetLitValName() {
  return "lit" + std::to_string(lit_val_counter++);
}

void MemSizeVisitor::InitTypeSizes(ProgNode& node) {
  type_sizes_["integer"] = 4;
  type_sizes_["float"] = 8;
  type_sizes_["void"] = 0;

  // Calculate class sizes
  for (std::string class_name : node.sorted_classes()) {
    type_sizes_[class_name] = 0;
    auto classtab = node.symtab->GetEntry("class", class_name)->Link();
    for (auto it = classtab->Begin(); it != classtab->End(); ++it) {
      if (it->second->Kind() == "memberVar") {
        type_sizes_[class_name] += GetEntrySize(*it->second);
      }
    }
  }
}

int MemSizeVisitor::GetEntrySize(const Entry& entry) {
  int size = type_sizes_.at(entry.Type());
  for (auto& dim : entry.Dims()) {
    if (dim.length() > 0) {
      size *= std::stoi(dim);
    }
  }
  return size;
}

void MemSizeVisitor::Visit(ProgNode& node) {
  InitTypeSizes(node);
  for (auto it = node.symtab->Begin(); it != node.symtab->End(); ++it) {
    it->second->size = GetEntrySize(*it->second);
    node.symtab->SetScopeSize(node.symtab->ScopeSize() - it->second->size);
  }
  DFS(node);
  std::ofstream ofs("../out/outsymboltable");
  ofs << *node.symtab;
}

void MemSizeVisitor::Visit(ClassNode& node) {
  DFS(node);
  auto symtab = node.symtab;
  for (auto it = symtab->Begin(); it != symtab->End(); ++it) {
    if (it->first.first == "inherit") continue;
    it->second->size = GetEntrySize(*it->second);
    it->second->offset = symtab->ScopeSize() - it->second->size;
    symtab->SetScopeSize(symtab->ScopeSize() - it->second->size);
  }
}

void MemSizeVisitor::Visit(FuncDefNode& node) {
  DFS(node);
  // After all the temps and literals were added to the
  // symtab, calculate offsets
  auto symtab = node.symtab;
  // Allocate some space for the return value at the bottom of the stack
  // frame.
  symtab->SetScopeSize(symtab->ScopeSize() -
                       type_sizes_.at(node.ChildAt(3)->Val()));
  // Then right after, make some space to store the return address
  symtab->SetScopeSize(symtab->ScopeSize() - 4);
  // Offsets for everything else
  for (auto it = symtab->Begin(); it != symtab->End(); ++it) {
    auto entry = it->second;
    entry->size = GetEntrySize(*entry);
    entry->offset = symtab->ScopeSize() - entry->size;
    symtab->SetScopeSize(symtab->ScopeSize() - entry->size);
  }
}

void MemSizeVisitor::Visit(MainNode& node) {
  DFS(node);
  // After all the temps and literals were added to the
  // symtab, calculate offsets
  auto symtab = node.symtab;
  for (auto it = symtab->Begin(); it != symtab->End(); ++it) {
    auto entry = it->second;
    entry->size = GetEntrySize(*entry);
    entry->offset = symtab->ScopeSize() - entry->size;
    symtab->SetScopeSize(symtab->ScopeSize() - entry->size);
  }
}

// Create a temp var entry
// The type was assigned to this node in TypeCheckVisitor
void MemSizeVisitor::Visit(AddOpNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetTempVarName(), node.Type(), node.ChildAt(0)->Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

// Create a temp var entry
// The type was assigned to this node in TypeCheckVisitor
void MemSizeVisitor::Visit(MultOpNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetTempVarName(), node.Type(), node.ChildAt(0)->Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

// Create a temp var entry for the return value of the function
// The type was assigned to this node in TypeCheckVisitor
void MemSizeVisitor::Visit(FuncCallNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetTempVarName(), node.Type(), node.ChildAt(0)->Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

// Create tempvar for the result of the relExpr (the result is an integer)
// The type was assigned to this node in TypeCheckVisitor
void MemSizeVisitor::Visit(RelExprNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetTempVarName(), node.Type(), node.ChildAt(1)->Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

void MemSizeVisitor::Visit(NotNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetTempVarName(), node.Type(), node.ChildAt(0)->Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

// Create a literal entry, type was set during TypeCheckVisitor
void MemSizeVisitor::Visit(IntNumNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetLitValName(), node.Type(), node.Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

// Create a literal entry, type was set during TypeCheckVisitor
void MemSizeVisitor::Visit(FloatNumNode& node) {
  DFS(node);
  auto entry = std::shared_ptr<LocalVarEntry>(new LocalVarEntry(
      GetLitValName(), node.Type(), node.Line(), nullptr, {}));
  node.symtab->AddEntry(entry);
  node.symtab_entry = entry;
}

void MemSizeVisitor::Visit(VarNode& node) {
  DFS(node);
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
}

void MemSizeVisitor::Visit(DataMemberNode& node) {
  DFS(node);
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
}

void MemSizeVisitor::Visit(ArithExprNode& node) {
  DFS(node);
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
}

void MemSizeVisitor::Visit(SignNode& node) {
  DFS(node);
  node.ChildAt(0)->SetVal(node.Val() + node.ChildAt(0)->Val());
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
}

void MemSizeVisitor::Visit(ReturnNode& node) {
  DFS(node);
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
}

void MemSizeVisitor::Visit(IdNode& node) {
  DFS(node);
  auto entry = node.symtab->GetEntry(node.Val());
  if (entry) {
    node.symtab_entry = entry;
  }
}

// Just dfs
void MemSizeVisitor::Visit(AParamsNode& node) { DFS(node); }
void MemSizeVisitor::Visit(AssignNode& node) { DFS(node); }
void MemSizeVisitor::Visit(ClassListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(DimListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(DimNode& node) { DFS(node); }
void MemSizeVisitor::Visit(FParamsListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(FParamsNode& node) { DFS(node); }
void MemSizeVisitor::Visit(FuncBodyNode& node) { DFS(node); }
void MemSizeVisitor::Visit(FuncDefListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(IfStatNode& node) { DFS(node); }
void MemSizeVisitor::Visit(IndiceList& node) { DFS(node); }
void MemSizeVisitor::Visit(InheritListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(MemberFuncDeclNode& node) { DFS(node); }
void MemSizeVisitor::Visit(MemberListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(MemberVarDeclNode& node) { DFS(node); }
void MemSizeVisitor::Visit(ReadNode& node) { DFS(node); }
void MemSizeVisitor::Visit(RelOpNode& node) { DFS(node); }
void MemSizeVisitor::Visit(ScopeResNode& node) { DFS(node); }
void MemSizeVisitor::Visit(StatListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(TypeNode& node) { DFS(node); }
void MemSizeVisitor::Visit(VarDeclListNode& node) { DFS(node); }
void MemSizeVisitor::Visit(VarDeclNode& node) { DFS(node); }
void MemSizeVisitor::Visit(WhileNode& node) { DFS(node); }
void MemSizeVisitor::Visit(WriteNode& node) { DFS(node); }

}  // namespace toy