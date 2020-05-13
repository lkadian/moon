#include "symbol_table.h"

#include <iomanip>

#include "logger.h"

namespace toy {

SymbolTable::SymbolTable(std::string name, std::shared_ptr<SymbolTable> parent,
                         int level)
    : name_(name), level_(level), parent_(parent), table_({}){};

std::string SymbolTable::Name() const { return name_; }
int SymbolTable::Level() const { return level_; }
std::shared_ptr<SymbolTable> SymbolTable::Parent() const { return parent_; }
int SymbolTable::ScopeSize() const { return mem_size_; }

void SymbolTable::SetName(std::string name) { name_ = name; }
void SymbolTable::SetParent(std::shared_ptr<SymbolTable> parent) {
  parent_ = parent;
}
void SymbolTable::SetScopeSize(int mem_size) { mem_size_ = mem_size; }
void SymbolTable::IncrementLevel() { level_++; }

SymTabIt SymbolTable::Begin() { return table_.begin(); }
SymTabIt SymbolTable::End() { return table_.end(); }

void SymbolTable::RemoveMemberFunctionDefinitions() {
  for (auto it = table_.begin(); it != table_.end();) {
    auto entry = it->second;
    if (entry->Kind() == "func" &&
        std::static_pointer_cast<FreeFuncEntry>(entry)->Scope().length() > 0) {
      it = table_.erase(it);
    } else {
      it++;
    }
  }
}

// Get an entry for the given identifier by searching up
// the chain of tables, starting from the current table
std::shared_ptr<toy::Entry> SymbolTable::GetEntry(
    const std::string& kind, const std::string& name) const {
  auto id = std::make_pair(kind, name);
  for (auto symtab = shared_from_this(); symtab != nullptr;
       symtab = symtab->parent_) {
    if (symtab->table_.find(id) != symtab->table_.end()) {
      return symtab->table_.at(id);
    }
  }
  return nullptr;
}

// Get an entry for the given identifier by searching up
// the chain of tables, starting from the current table
std::shared_ptr<toy::Entry> SymbolTable::GetEntry(
    const std::string& name) const {
  for (auto symtab = shared_from_this(); symtab != nullptr;
       symtab = symtab->parent_) {
    for (auto p : symtab->table_) {
      if (p.first.second == name) {
        return p.second;
      }
    }
  }
  return nullptr;
}

void SymbolTable::AddEntry(std::shared_ptr<LocalVarEntry> entry) {
  auto id = std::make_pair(entry->Kind(), entry->Name());
  if (table_.find(id) != table_.end()) {
    Logger::Err("Variable '" + id.second + "' already declared in scope '" +
                    name_ + "'",
                entry->Line(), ErrorType::SEMANTIC);
  } else {
    table_.insert({id, entry});
  }
}

// Here we need to check two thigs: that it is not declared in the local scope,
// but also that it doesn't shadow an inherited member
void SymbolTable::AddEntry(std::shared_ptr<MemberVarEntry> entry) {
  auto id = std::make_pair(entry->Kind(), entry->Name());
  if (table_.find(id) != table_.end()) {
    auto found_entry = table_.at(id);
    std::string class_name =
        std::dynamic_pointer_cast<MemberVarEntry>(entry)->Class();
    if (name_ != class_name) {
      Logger::Warn("Shadowed member variable '" + id.second + "' in '" + name_ +
                       "' already declared in '" + class_name + "'",
                   entry->Line(), WarningType::SEMANTIC);
      table_.insert({id, entry});
    } else {
      Logger::Err("Member variable '" + id.second + "' already declared in '" +
                      name_ + "'",
                  entry->Line(), ErrorType::SEMANTIC);
    }
  } else {
    table_.insert({id, entry});
  }
}

void SymbolTable::AddEntry(std::shared_ptr<FreeFuncEntry> entry) {
  std::string func_name = entry->Scope() + "::" + entry->Name();
  std::string func_signature = entry->Signature();
  auto id = std::make_pair(entry->Kind(), func_signature);
  if (funcs_.find(func_name) != funcs_.end()) {
    // Found function with same signature
    if (table_.find(id) != table_.end()) {
      Logger::Err("Free function '" + func_signature +
                      "' already declared in '" + name_ + "'",
                  entry->Line(), ErrorType::SEMANTIC);
    }
    // Found function with same name, but diff params
    else {
      Logger::Warn(
          "Overloaded function '" + func_signature + "' in '" + name_ + "'",
          entry->Line(), WarningType::SEMANTIC);
    }
  }
  if (table_.find(id) == table_.end()) {
    funcs_.insert(func_name);
    table_.insert({id, entry});
  }
};

void SymbolTable::AddEntry(std::shared_ptr<MemberFuncEntry> entry) {
  std::string func_name = entry->Class() + "::" + entry->Name();
  std::string func_signature = entry->Signature();

  auto id = std::make_pair(entry->Kind(), func_signature);
  if (funcs_.find(func_name) != funcs_.end()) {
    if (table_.find(id) != table_.end()) {
      auto found_entry = table_.at(id);
      std::string class_name =
          std::dynamic_pointer_cast<MemberFuncEntry>(entry)->Class();
      if (name_ != class_name) {
        Logger::Warn("Shadowed member function '" + id.second + "' in '" +
                         name_ + "' already declared in '" + class_name + "'",
                     entry->Line(), WarningType::SEMANTIC);
        table_.insert({id, entry});
      } else {
        Logger::Err("Member function '" + id.second +
                        "' already declared in '" + name_ + "'",
                    entry->Line(), ErrorType::SEMANTIC);
      }
    } else {
      Logger::Warn("Overloaded member function '" + func_signature + "' in '" +
                       name_ + "'",
                   entry->Line(), WarningType::SEMANTIC);
      table_.insert({id, entry});
    }

  } else {
    funcs_.insert(func_name);
    table_.insert({id, entry});
  }
};

void SymbolTable::AddEntry(std::shared_ptr<ClassEntry> entry) {
  auto id = std::make_pair(entry->Kind(), entry->Name());
  if (table_.find(id) != table_.end()) {
    Logger::Err("Class '" + id.second + "' already declared in '" + name_ + "'",
                entry->Line(), ErrorType::SEMANTIC);
  } else {
    table_.insert({id, entry});
  }
};

void SymbolTable::AddEntry(std::shared_ptr<InheritEntry> entry) {
  auto id = std::make_pair(entry->Kind(), entry->Name());
  table_.insert({id, entry});
};

// A function's signature includes the function's name and the number,
// order and type of its formal parameters
std::string SymbolTable::GetFuncSignature(std::string cls, std::string name,
                                          std::vector<std::string> fparams) {
  std::string func_signature = cls + "::" + name + "(";
  auto separator = ", ";
  auto sep = "";
  for (auto& param : fparams) {
    func_signature += sep + param;
    sep = separator;
  }
  func_signature += ")";
  return func_signature;
}

std::ostream& operator<<(std::ostream& os, const SymbolTable& symtab) {
  std::string prelinespacing;
  std::string header =
      "======================================================================";
  int total_width = header.length();  // 70
  for (int i = 0; i < symtab.level_; i++) {
    prelinespacing += "|    ";
  }

  os << "\n" << prelinespacing + header + "\n";
  os << prelinespacing;
  std::string s1 = "| table: " + symtab.name_;
  std::string s2 = "scope size: " + std::to_string(symtab.ScopeSize());
  os << std::left << std::setw(total_width - 19) << s1;
  os << std::right << std::setw(16) << s2 << std::right << std::setw(3) << "|";
  os << "\n" << prelinespacing + header + "\n";

  for (auto it = symtab.table_.begin(); it != symtab.table_.end(); it++) {
    os << prelinespacing << *it->second << std::endl;
  }
  os << prelinespacing + header;
  return os;
}

Entry::Entry(std::string kind, std::string name, std::string type, int line,
             std::shared_ptr<SymbolTable> link)
    : kind_(kind), name_(name), type_(type), line_(line), link_(link){};

std::string Entry::Kind() const { return kind_; }
std::string Entry::Name() const { return name_; }
std::string Entry::Type() const { return type_; }
int Entry::Line() const { return line_; }
std::vector<std::string> Entry::Dims() const { return {}; }
std::shared_ptr<SymbolTable> Entry::Link() const { return link_; }
void Entry::SetKind(std::string kind) { kind_ = kind; }
void Entry::SetName(std::string name) { name_ = name; }
void Entry::SetType(std::string type) { type_ = type; }
void Entry::SetLink(std::shared_ptr<SymbolTable> link) { link_ = link; }

std::ostream& operator<<(std::ostream& os, const Entry& entry) {
  entry.Print(os);
  return os;
}

void Entry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(20) << s;
  s = "| " + name_;
  os << std::left << std::setw(20) << s;
  s = "| " + type_;
  os << std::left << std::setw(20) << s;
  s = "| " + std::to_string(size);
  os << std::left << std::setw(5) << s;
  s = "| " + std::to_string(offset) + " |";
  os << std::setw(5) << s;
  if (link_ != nullptr) {
    os << *link_;
  }
}

LocalVarEntry::LocalVarEntry(std::string name, std::string type, int line,
                             std::shared_ptr<SymbolTable> link,
                             std::vector<std::string> dims)
    : Entry("local", name, type, line, link), dims_(dims) {}

std::vector<std::string> LocalVarEntry::Dims() const { return dims_; }
void LocalVarEntry::SetDims(std::vector<std::string> dims) { dims_ = dims; }

void LocalVarEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(11) << s;
  s = "| " + name_;
  os << std::left << std::setw(22) << s;
  s = "| " + type_;
  for (auto dim : dims_) {
    s += "[]";
  }
  os << std::left << std::setw(22) << s;
  s = "| " + std::to_string(size);
  os << std::left << std::setw(7) << s;
  s = "| " + std::to_string(offset);
  os << std::setw(7) << s;
  os << "|";
}

MemberVarEntry::MemberVarEntry(std::string name, std::string type, int line,
                               std::shared_ptr<SymbolTable> link,
                               std::vector<std::string> dims, std::string cls,
                               std::string visibility)
    : Entry("memberVar", name, type, line, link),
      dims_(dims),
      cls_(cls),
      visibility_(visibility) {}

std::vector<std::string> MemberVarEntry::Dims() const { return dims_; }
std::string MemberVarEntry::Class() { return cls_; }
std::string MemberVarEntry::Visibility() { return visibility_; }
void MemberVarEntry::SetDims(std::vector<std::string> dims) { dims_ = dims; }
void MemberVarEntry::SetClass(std::string cls) { cls_ = cls; }
void MemberVarEntry::SetVisibility(std::string visibility) {
  visibility_ = visibility;
}

void MemberVarEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(13) << s;
  s = "| " + name_;
  os << std::left << std::setw(15) << s;
  s = "| " + type_;
  for (auto dim : dims_) {
    s += "[]";
  }
  os << std::left << std::setw(15) << s;
  s = "| " + visibility_;
  os << std::left << std::setw(12) << s;
  s = "| " + std::to_string(size);
  os << std::left << std::setw(7) << s;
  s = "| " + std::to_string(offset);
  os << std::setw(7) << s;
  os << "|";
}

FreeFuncEntry::FreeFuncEntry(
    std::string name, std::string type, int line,
    std::shared_ptr<SymbolTable> link,
    std::vector<std::pair<std::string, std::string>> params, std::string scope)
    : Entry("func", name, type, line, link), params_(params), scope_(scope) {
  std::vector<std::string> param_types;
  for (auto param : params) {
    param_types.push_back(param.second);
  }
  signature_ = SymbolTable::GetFuncSignature(scope, name, param_types);
};

std::vector<std::pair<std::string, std::string>> FreeFuncEntry::Params() {
  return params_;
}
std::string FreeFuncEntry::Scope() { return scope_; }
std::string FreeFuncEntry::Signature() { return signature_; }

void FreeFuncEntry::SetParams(
    std::vector<std::pair<std::string, std::string>> params) {
  params_ = params;
}
void FreeFuncEntry::SetScope(std::string scope) { scope_ = scope; }

void FreeFuncEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(8) << s;
  s = "| " + signature_ + " -> " + type_;
  os << std::left << std::setw(61) << s << "|";
  if (link_ != nullptr) {
    os << *link_;
  }
}

MemberFuncEntry::MemberFuncEntry(
    std::string name, std::string type, int line,
    std::shared_ptr<SymbolTable> link,
    std::vector<std::pair<std::string, std::string>> params, std::string cls,
    std::string visibility)
    : Entry("memberFunc", name, type, line, link),
      params_(params),
      cls_(cls),
      visibility_(visibility) {
  std::vector<std::string> param_types;
  for (auto param : params) {
    param_types.push_back(param.second);
  }
  signature_ = SymbolTable::GetFuncSignature(cls, name, param_types);
};

std::vector<std::pair<std::string, std::string>> MemberFuncEntry::Params() {
  return params_;
}
std::string MemberFuncEntry::Class() { return cls_; }
std::string MemberFuncEntry::Visibility() { return visibility_; }
std::string MemberFuncEntry::Signature() { return signature_; }
void MemberFuncEntry::SetParams(
    std::vector<std::pair<std::string, std::string>> params) {
  params_ = params;
}
void MemberFuncEntry::SetClass(std::string cls) { cls_ = cls; }
void MemberFuncEntry::SetVisibility(std::string visibility) {
  visibility_ = visibility;
}

void MemberFuncEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(15) << s;
  s = "| " + visibility_;
  os << std::left << std::setw(10) << s;
  s = "| " + signature_ + " -> " + type_;
  os << std::left << std::setw(44) << s;
  os << std::right << std::setw(1) << "|";
  if (link_ != nullptr) {
    os << *link_;
  }
}

ClassEntry::ClassEntry(std::string name, int line,
                       std::shared_ptr<SymbolTable> link)
    : Entry("class", name, name, line, link){};

void ClassEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(8) << s;
  s = "| " + name_;
  os << std::left << std::setw(22) << s;
  os << std::right << std::setw(40) << "|";

  if (link_ != nullptr) {
    os << *link_;
  }
}

InheritEntry::InheritEntry(std::vector<std::string> inherit_list)
    : Entry("inherit", "inherit", "", 0, nullptr),
      inherit_list_(inherit_list){};

std::vector<std::string> InheritEntry::InheritList() { return inherit_list_; }

void InheritEntry::Print(std::ostream& os) const {
  std::string s = "| " + kind_;
  os << std::left << std::setw(15) << s;
  s = "";
  if (inherit_list_.size() == 0) {
    s += "none ";
  } else {
    for (auto entry_name : inherit_list_) {
      s += entry_name + " ";
    }
  }
  s = "| " + s;
  os << std::left << std::setw(15) << s;
  os << std::right << std::setw(40) << "|";
}

}  // namespace toy