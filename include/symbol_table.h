#ifndef TOY_SYMBOL_TABLE_H_
#define TOY_SYMBOL_TABLE_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace toy {

// Forward declarations
class Entry;
class LocalVarEntry;
class MemberVarEntry;
class FreeFuncEntry;
class MemberFuncEntry;
class ClassEntry;
class InheritEntry;

typedef std::map<std::pair<std::string, std::string>,
                 std::shared_ptr<toy::Entry>>::iterator SymTabIt;

class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
 public:
  SymbolTable() = delete;
  SymbolTable(std::string name, std::shared_ptr<SymbolTable> parent, int level);

  std::string Name() const;
  int Level() const;
  std::shared_ptr<SymbolTable> Parent() const;
  int ScopeSize() const;

  void SetName(std::string name);
  void SetLevel(int level);
  void SetParent(std::shared_ptr<SymbolTable> parent);
  void SetScopeSize(int mem_size);
  void IncrementLevel();
  // Remove member functions
  void RemoveMemberFunctionDefinitions();

  SymTabIt Begin();
  SymTabIt End();

  std::shared_ptr<toy::Entry> GetEntry(const std::string& kind,
                                       const std::string& name) const;
  std::shared_ptr<toy::Entry> GetEntry(const std::string& name) const;
  void AddEntry(std::shared_ptr<LocalVarEntry> entry);
  void AddEntry(std::shared_ptr<MemberVarEntry> entry);
  void AddEntry(std::shared_ptr<FreeFuncEntry> entry);
  void AddEntry(std::shared_ptr<MemberFuncEntry> entry);
  void AddEntry(std::shared_ptr<ClassEntry> entry);
  void AddEntry(std::shared_ptr<InheritEntry> entry);

  static std::string GetFuncSignature(std::string cls, std::string name,
                                      std::vector<std::string> fparams);
  friend std::ostream& operator<<(std::ostream& os, const SymbolTable& symbtab);

 private:
  std::string name_;
  int level_;
  std::shared_ptr<SymbolTable> parent_;
  int mem_size_ = 0;
  // The key is (entryKind, entryId), the value is the entry.
  std::map<std::pair<std::string, std::string>, std::shared_ptr<toy::Entry>>
      table_;
  // Helper set to check for overloaded functions.
  std::set<std::string> funcs_;
};
std::ostream& operator<<(std::ostream& os, const SymbolTable& symbtab);

/**
 * Base class for entries in the SymbolTable.
 */
class Entry {
 public:
  Entry() = delete;
  Entry(std::string kind, std::string name, std::string type, int line,
        std::shared_ptr<SymbolTable> link);
  std::string Kind() const;
  std::string Name() const;
  std::string Type() const;
  int Line() const;
  std::shared_ptr<SymbolTable> Link() const;
  virtual std::vector<std::string> Dims() const;

  // memory size & offset, for code generation
  int size = 0;
  int offset = 0;

  void SetKind(std::string kind);
  void SetName(std::string name);
  void SetType(std::string type);
  void SetLink(std::shared_ptr<SymbolTable> link);

  friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
  virtual void Print(std::ostream& os) const;

 protected:
  std::string kind_;  // Kind of entry, e.g. var, class, func, memberFunc...
  std::string name_;
  std::string type_;  // Data type, e.g. integer
  int line_;
  std::shared_ptr<SymbolTable> link_;
};
std::ostream& operator<<(std::ostream& os, const Entry& entry);

class LocalVarEntry : public Entry {
 public:
  LocalVarEntry(std::string name, std::string type, int line,
                std::shared_ptr<SymbolTable> link,
                std::vector<std::string> dims);
  std::vector<std::string> Dims() const;
  void SetDims(std::vector<std::string> dims);
  void Print(std::ostream& os) const override;

 private:
  std::vector<std::string> dims_;
};
std::ostream& operator<<(std::ostream& os, const LocalVarEntry& entry);

class MemberVarEntry : public Entry {
 public:
  MemberVarEntry(std::string name, std::string type, int line,
                 std::shared_ptr<SymbolTable> link,
                 std::vector<std::string> dims, std::string cls,
                 std::string visibility);
  std::vector<std::string> Dims() const;
  std::string Class();
  std::string Visibility();
  void SetDims(std::vector<std::string> dims);
  void SetClass(std::string cls);
  void SetVisibility(std::string visibility);
  void Print(std::ostream& os) const override;

 private:
  std::vector<std::string> dims_;
  std::string cls_;
  std::string visibility_;
};
std::ostream& operator<<(std::ostream& os, const MemberVarEntry& entry);

class FreeFuncEntry : public Entry {
 public:
  FreeFuncEntry(std::string name, std::string type, int line,
                std::shared_ptr<SymbolTable> link,
                std::vector<std::pair<std::string, std::string>> params,
                std::string scope);
  std::vector<std::pair<std::string, std::string>> Params();
  std::string Scope();
  std::string Signature();
  void SetParams(std::vector<std::pair<std::string, std::string>> params);
  void SetScope(std::string scope);
  void Print(std::ostream& os) const override;

 private:
  std::vector<std::pair<std::string, std::string>>
      params_;  // vector of (param_name, param_type)
  std::string scope_;
  std::string signature_;
};
std::ostream& operator<<(std::ostream& os, const FreeFuncEntry& entry);

class MemberFuncEntry : public Entry {
 public:
  MemberFuncEntry(std::string name, std::string type, int line,
                  std::shared_ptr<SymbolTable> link,
                  std::vector<std::pair<std::string, std::string>> params,
                  std::string cls, std::string visibility);
  std::vector<std::pair<std::string, std::string>> Params();
  std::string Class();
  std::string Visibility();
  std::string Signature();
  void SetParams(std::vector<std::pair<std::string, std::string>> params);
  void SetClass(std::string cls);
  void SetVisibility(std::string visibility);
  void Print(std::ostream& os) const override;

 private:
  std::vector<std::pair<std::string, std::string>>
      params_;  // vector of (param name, type)
  std::string cls_;
  std::string visibility_;
  std::string signature_;
};
std::ostream& operator<<(std::ostream& os, const MemberFuncEntry& entry);

class ClassEntry : public Entry {
 public:
  ClassEntry(std::string name, int line, std::shared_ptr<SymbolTable> link);
  void Print(std::ostream& os) const override;
};
std::ostream& operator<<(std::ostream& os, const ClassEntry& entry);

class InheritEntry : public Entry {
 public:
  InheritEntry(std::vector<std::string> inherit_list);
  std::vector<std::string> InheritList();
  void Print(std::ostream& os) const override;

 private:
  std::vector<std::string> inherit_list_;
};
std::ostream& operator<<(std::ostream& os, const InheritEntry& entry);

}  // namespace toy

#endif  // TOY_SYMBOL_TABLE_H_