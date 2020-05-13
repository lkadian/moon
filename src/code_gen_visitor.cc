#include "code_gen_visitor.h"

#include <algorithm>
#include <fstream>

#include "ast.h"
#include "symbol_table.h"
#include "util.h"

namespace toy {

CodeGenVisitor::CodeGenVisitor()
    : exec_code_(),
      data_code_(),
      code_indent_("      "),
      ifstat_num_(1),
      not_num_(1),
      and_num_(0),
      or_num_(0) {
  // General purpose registers (r1-r9)
  for (int i = 9; i >= 1; i--) {
    register_pool_.push("r" + std::to_string(i));
  }
  // Registers reserved for runtime offset calculation storage
  for (int i = 12; i >= 9; i--) {
    offset_register_pool_.push("r" + std::to_string(i));
  }
  procedure_map_ = CodeGenVisitor::InitProcedureMap();
};

int CodeGenVisitor::GetIfStatNum() { return ifstat_num_++; }
int CodeGenVisitor::GetNotNum() { return not_num_++; }
int CodeGenVisitor::GetAndNum() { return and_num_++; }
int CodeGenVisitor::GetOrNum() { return or_num_++; }

void CodeGenVisitor::AddExecLine(const std::string& line) {
  exec_code_ += code_indent_ + line + "\n";
}

void CodeGenVisitor::AddComment(const std::string& comment) {
  exec_code_ += "% " + comment + "\n";
}

void CodeGenVisitor::AddHeaderComment(const std::string& comment) {
  AddComment("--------------------------------------------------------------%");
  exec_code_ += "% " + comment + "\n";
  AddComment("--------------------------------------------------------------%");
}

// start offset if the offset stored at the node is not r0
void CodeGenVisitor::StartOffsetIf(const ASTNode& node) {
  if (node.regist != "r0") {
    AddComment("Start array offsetting");
    AddExecLine("add r14, r14, " + node.regist);
  }
}

void CodeGenVisitor::EndOffsetIf(ASTNode& node) {
  if (node.regist != "r0") {
    AddComment("End array offsetting");
    AddExecLine("sub r14, r14, " + node.regist);
    offset_register_pool_.push(node.regist);
  }
}

void CodeGenVisitor::Visit(ProgNode& node) {
  DFS(node);
  std::ofstream output_file("../out/outcode.m");
  output_file << exec_code_;
  output_file << data_code_;
  output_file << procedures_;
}

void CodeGenVisitor::Visit(MainNode& node) {
  AddHeaderComment("Start of main");
  AddExecLine("entry");
  AddExecLine("addi r14, r0, topaddr\n");
  DFS(node);
  AddExecLine("hlt");
  AddHeaderComment("End of main");
}

// Stores the intnum (litval) at the offset address
// calculated in MemSizeVisitor
void CodeGenVisitor::Visit(IntNumNode& node) {
  DFS(node);
  int litval_offset = node.symtab_entry->offset;
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  AddComment("storing intnum " + node.Val());
  AddExecLine("addi " + r1 + ", r0, " + node.Val());
  AddExecLine("sw " + std::to_string(litval_offset) + "(r14), " + r1 + "\n");
  register_pool_.push(r1);
}

void CodeGenVisitor::HandleAnd(MultOpNode& node) {
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string r2 = register_pool_.top();
  register_pool_.pop();
  std::string r3 = register_pool_.top();
  register_pool_.pop();
  std::string r4 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(1)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;
  int tmp_offset = node.symtab_entry->offset;

  AddComment(lhs_name + " and " + rhs_name);

  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r2 + ", " + std::to_string(lhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));
  // RHS
  StartOffsetIf(*node.ChildAt(1));
  AddExecLine("lw " + r3 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(1));

  std::string tagnum = std::to_string(GetAndNum());
  std::string endandtag = "endand" + tagnum;
  std::string zerotag = "andzero" + tagnum;
  AddExecLine("bz " + r2 + ", " + zerotag);
  AddExecLine("bz " + r3 + ", " + zerotag);
  AddExecLine("addi " + r4 + ", r0, 1");
  AddExecLine("j " + endandtag);
  AddExecLine(zerotag + " addi " + r4 + ", r0, 0");
  AddExecLine(endandtag + " sw " + std::to_string(tmp_offset) + "(r14), " + r4);

  register_pool_.push(r4);
  register_pool_.push(r3);
  register_pool_.push(r2);
  register_pool_.push(r1);
}

void CodeGenVisitor::HandleOr(AddOpNode& node) {
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string r2 = register_pool_.top();
  register_pool_.pop();
  std::string r3 = register_pool_.top();
  register_pool_.pop();
  std::string r4 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(1)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;
  int tmp_offset = node.symtab_entry->offset;

  AddComment(lhs_name + " or " + rhs_name);

  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r2 + ", " + std::to_string(lhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));
  // RHS
  StartOffsetIf(*node.ChildAt(1));
  AddExecLine("lw " + r3 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(1));

  std::string tagnum = std::to_string(GetOrNum());
  std::string endortag = "endor" + tagnum;
  std::string nonzerotag = "ornonzero" + tagnum;
  // if both is zero branch off.
  AddExecLine("bnz " + r2 + ", " + nonzerotag);
  AddExecLine("bnz " + r3 + ", " + nonzerotag);
  AddExecLine("addi " + r4 + ", r0, 0");
  AddExecLine("j " + endortag);
  AddExecLine(nonzerotag + " addi " + r4 + ", r0, 1");
  AddExecLine(endortag + " sw " + std::to_string(tmp_offset) + "(r14), " + r4);

  register_pool_.push(r4);
  register_pool_.push(r3);
  register_pool_.push(r2);
  register_pool_.push(r1);
}

// Perform the add operation, and store the temp result
// at the memory address offset calculated in MemSizeVisitor
void CodeGenVisitor::Visit(AddOpNode& node) {
  DFS(node);
  std::string op = node.Val();
  if (op == "or") {
    HandleOr(node);
    return;
  }
  std::string instruction = GetInstructionFromOp(op);
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string r2 = register_pool_.top();
  register_pool_.pop();
  std::string r3 = register_pool_.top();
  register_pool_.pop();
  std::string r4 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(1)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;
  int tmp_offset = node.symtab_entry->offset;

  AddComment(lhs_name + " " + op + " " + rhs_name);

  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r2 + ", " + std::to_string(lhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));
  // RHS
  StartOffsetIf(*node.ChildAt(1));
  AddExecLine("lw " + r3 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(1));

  AddExecLine(instruction + r4 + ", " + r2 + ", " + r3);
  AddExecLine("sw " + std::to_string(tmp_offset) + "(r14), " + r4 + "\n");

  register_pool_.push(r4);
  register_pool_.push(r3);
  register_pool_.push(r2);
  register_pool_.push(r1);
}

// Perform the mult operation, and store the temp result
// at the memory address offset calculated in MemSizeVisitor
void CodeGenVisitor::Visit(MultOpNode& node) {
  DFS(node);
  std::string op = node.Val();
  if (op == "and") {
    HandleAnd(node);
    return;
  }
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string r2 = register_pool_.top();
  register_pool_.pop();
  std::string r3 = register_pool_.top();
  register_pool_.pop();
  std::string r4 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(1)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;
  int tmp_offset = node.symtab_entry->offset;
  std::string instruction = GetInstructionFromOp(op);

  AddComment(lhs_name + " " + op + " " + rhs_name);
  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r2 + ", " + std::to_string(lhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));
  // RHS
  StartOffsetIf(*node.ChildAt(1));
  AddExecLine("lw " + r3 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(1));
  AddExecLine(instruction + r4 + ", " + r2 + ", " + r3);
  AddExecLine("sw " + std::to_string(tmp_offset) + "(r14), " + r4 + "\n");

  register_pool_.push(r4);
  register_pool_.push(r3);
  register_pool_.push(r2);
  register_pool_.push(r1);
}

// Perform the relational operation, and store the temp result
// at the memory address offset calculated in MemSizeVisitor
void CodeGenVisitor::Visit(RelExprNode& node) {
  DFS(node);
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string r2 = register_pool_.top();
  register_pool_.pop();
  std::string r3 = register_pool_.top();
  register_pool_.pop();
  std::string r4 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(2)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;
  int tmp_offset = node.symtab_entry->offset;
  std::string op = node.ChildAt(1)->Val();
  std::string instruction = GetInstructionFromOp(op);

  AddComment(lhs_name + " " + op + " " + rhs_name);
  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r2 + ", " + std::to_string(lhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));
  // RHS
  StartOffsetIf(*node.ChildAt(2));
  AddExecLine("lw " + r3 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(2));
  AddExecLine(instruction + r4 + ", " + r2 + ", " + r3);
  AddExecLine("sw " + std::to_string(tmp_offset) + "(r14), " + r4 + "\n");

  register_pool_.push(r4);
  register_pool_.push(r3);
  register_pool_.push(r2);
  register_pool_.push(r1);
}

void CodeGenVisitor::Visit(VarNode& node) {
  // For access to class variables
  if (node.Children().size() > 1) {
    std::string entry_name;
    std::string type;
    int offset = 0;
    std::string sep = "";
    for (auto& child : node.Children()) {
      child->Accept(*this);
      type = child->Type();
      entry_name += sep + child->ChildAt(0)->Val();
      if (child->symtab_entry->Kind() == "memberVar") {
        offset -= child->symtab_entry->offset + child->symtab_entry->size;
      } else {
        offset += child->symtab_entry->offset;
      }
      sep = ".";
    }
    auto tmpentry = std::shared_ptr<LocalVarEntry>(
        new LocalVarEntry(entry_name, type, 0, nullptr, {}));
    tmpentry->offset = offset;
    node.symtab->AddEntry(tmpentry);
    node.symtab_entry = tmpentry;
  } else {
    DFS(node);
    node.symtab_entry = node.ChildAt(0)->symtab_entry;
  }
  node.regist = node.ChildAt(0)->regist;
}

void CodeGenVisitor::Visit(DataMemberNode& node) {
  DFS(node);

  auto entry = node.symtab->GetEntry("local", node.ChildAt(0)->Val());
  if (!entry) {
    entry = node.symtab->GetEntry("class", node.cls);
    if (entry) {
      entry = entry->Link()->GetEntry("memberVar", node.ChildAt(0)->Val());
    }
    // Inside a member function
    else {
      entry = node.symtab->GetEntry("memberVar", node.ChildAt(0)->Val());
    }
  }
  node.symtab_entry = std::shared_ptr<Entry>(entry);
  auto dims = entry->Dims();
  // To calculate offset
  // a[0][0] = r14 + 0*sizeof(type)*col + 0*sizeof(int)
  // where col is the "next" size, e.g. x[3][5] is 5 for col
  if (node.ChildAt(1)->Children().size() > 0) {
    node.regist = offset_register_pool_.top();
    offset_register_pool_.pop();
    AddExecLine("add " + node.regist + ", r0, r0");
    auto vec = node.ChildAt(1)->Children();
    uint64_t i = 1;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      auto indice = *it;
      AddComment("calculating offset");
      AddExecLine("lw r1, " + std::to_string(indice->symtab_entry->offset) +
                  "(r14)");
      // 4 is hardcoded here, change for type size
      AddExecLine("muli r2, r1, 4");
      if (i < dims.size()) {
        AddExecLine("muli r2, r2, " + dims.at(i));
      }
      AddExecLine("add " + node.regist + ", " + node.regist + ", r2");
      i++;
    }
  }
}

void CodeGenVisitor::Visit(AssignNode& node) {
  DFS(node);
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  auto lhs_entry = node.ChildAt(0)->symtab_entry;
  std::string lhs_name = lhs_entry->Name();
  int lhs_offset = lhs_entry->offset;
  auto rhs_entry = node.ChildAt(1)->symtab_entry;
  std::string rhs_name = rhs_entry->Name();
  int rhs_offset = rhs_entry->offset;

  AddComment(lhs_name + " = " + rhs_name);
  // RHS
  StartOffsetIf(*node.ChildAt(1));
  AddExecLine("lw " + r1 + ", " + std::to_string(rhs_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(1));
  // LHS
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("sw " + std::to_string(lhs_offset) + "(r14), " + r1 + "\n");
  EndOffsetIf(*node.ChildAt(0));

  register_pool_.push(r1);
}

void CodeGenVisitor::Visit(IfStatNode& node) {
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string tagnum = std::to_string(GetIfStatNum());
  std::string elsetag = "else" + tagnum;
  std::string endiftag = "endif" + tagnum;
  // Process the rel expr
  node.ChildAt(0)->Accept(*this);
  auto left_entry = node.ChildAt(0)->symtab_entry;
  std::string left_name = left_entry->Name();
  int left_offset = left_entry->offset;
  AddComment("if");
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r1 + ", " + std::to_string(left_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));

  AddExecLine("bz " + r1 + ", " + elsetag);
  register_pool_.push(r1);
  // Process the first statement
  AddComment("then");
  node.ChildAt(1)->Accept(*this);
  AddExecLine("j " + endiftag);
  // Process the second statement
  AddComment("else");
  AddExecLine(elsetag);
  node.ChildAt(2)->Accept(*this);
  AddExecLine(endiftag);
}

void CodeGenVisitor::Visit(WhileNode& node) {
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string tagnum = std::to_string(GetIfStatNum());
  std::string gowhiletag = "gowhile" + tagnum;
  std::string endwhiletag = "endwhile" + tagnum;
  // Process the rel expr
  AddComment("while loop");
  AddExecLine(gowhiletag);
  node.ChildAt(0)->Accept(*this);
  auto left_entry = node.ChildAt(0)->symtab_entry;
  std::string left_name = left_entry->Name();
  int left_offset = left_entry->offset;
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r1 + ", " + std::to_string(left_offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));

  AddExecLine("bz " + r1 + ", " + endwhiletag);
  register_pool_.push(r1);
  // Process the stat block
  AddComment("inside the while loop");
  node.ChildAt(1)->Accept(*this);
  AddExecLine("j " + gowhiletag);
  AddExecLine(endwhiletag);
}

void CodeGenVisitor::Visit(WriteNode& node) {
  DFS(node);
  AddLibProcedure("putint");
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  int offset = node.ChildAt(0)->symtab_entry->offset;
  AddComment("writing to stdout");

  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("lw " + r1 + ", " + std::to_string(offset) + "(r14)");
  EndOffsetIf(*node.ChildAt(0));

  AddExecLine("jl r15, putint\n");
  register_pool_.push(r1);
}

void CodeGenVisitor::Visit(ReadNode& node) {
  DFS(node);
  AddLibProcedure("getint");
  int offset = node.ChildAt(0)->symtab_entry->offset;
  AddComment("reading from stdin");
  AddExecLine("jl r15, getint\n");
  StartOffsetIf(*node.ChildAt(0));
  AddExecLine("sw " + std::to_string(offset) + "(r14), r1\n");
  EndOffsetIf(*node.ChildAt(0));
}

void CodeGenVisitor::Visit(FuncDefNode& node) {
  std::string func_tag;
  if (node.ChildAt(1)->Children().size() == 0) {
    func_tag = node.symtab_entry->Name();
  } else {
    func_tag = node.ChildAt(1)->ChildAt(0)->Val() + node.symtab_entry->Name();
  }

  for (auto param : node.ChildAt(2)->Children()) {
    func_tag += param->ChildAt(0)->Val();
  }
  func_tag.erase(
      std::remove_if(func_tag.begin(), func_tag.end(),
                     [](auto const& c) -> bool { return !std::isalnum(c); }),
      func_tag.end());
  AddHeaderComment("Start of function definition: " + func_tag);
  // Create the tag to jump onto after function is done and also
  // copy the jumping-back address as the second thing on function call stack
  // (after the return value), at and offset which depends on the return size of
  // the return value, i.e. if the first thing on the stack is the return value
  // which is an integer, then this will be at offset -4, void -0, etc.
  auto offset = -node.ChildAt(3)->symtab_entry->size;
  AddExecLine(func_tag + " sw " + std::to_string(offset) + "(r14), r15");
  DFS(node);
  // Load that jumping-back address stored earlier and jump to it, ending the
  // function
  AddExecLine("lw r15, " + std::to_string(offset) + "(r14)");
  AddExecLine("jr r15");
  AddHeaderComment("End of function definition: " + func_tag);
}

void CodeGenVisitor::Visit(ReturnNode& node) {
  DFS(node);
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  AddComment("return");
  std::string return_offset = std::to_string(node.symtab_entry->offset);
  // Store the return value at beginning of function call stack
  // The offset of this value was calculated in MemSizeVisitor
  AddExecLine("lw " + r1 + ", " + return_offset + "(r14)");
  AddExecLine("sw 0(r14)," + r1);
  register_pool_.push(r1);
}

void CodeGenVisitor::Visit(FuncCallNode& node) {
  DFS(node);
  // TODO: Refactor to have func_signature as a member of fcallNode.
  std::string class_name = node.cls;
  std::string func_name = node.ChildAt(0)->Val();
  std::string func_tag = class_name + func_name;
  std::vector<std::string> fparams;
  for (auto& param : node.ChildAt(1)->Children()) {
    fparams.push_back(param->Type());
    func_tag += param->Type();
  }
  std::string func_signature = SymbolTable::GetFuncSignature(
      class_name, node.ChildAt(0)->Val(), fparams);
  std::shared_ptr<Entry> func_entry =
      node.symtab->GetEntry("func", func_signature);
  if (!func_entry) {
    func_entry = node.symtab->GetEntry("class", class_name)
                     ->Link()
                     ->GetEntry("memberFunc", func_signature);
  }
  // Erase non-alphanum from functag, else error in the assembly
  func_tag.erase(
      std::remove_if(func_tag.begin(), func_tag.end(),
                     [](auto const& c) -> bool { return !std::isalnum(c); }),
      func_tag.end());
  auto func_symtab = func_entry->Link();

  std::string r1 = register_pool_.top();
  register_pool_.pop();
  AddComment("Function call to " + func_tag);
  // Parameter passing
  int idx_of_param = 0;
  for (auto param : node.ChildAt(1)->Children()) {
    int passed_param_offset = param->symtab_entry->offset;
    int passed_param_size = param->symtab_entry->size;
    if (param->regist != "r0") {
      for (auto dim : param->symtab_entry->Dims()) {
        passed_param_size /= std::stoi(dim);
      }
    }
    // copy word by word
    for (int i = 0; i < passed_param_size; i += 4) {
      // Loading the PASSED value into r1
      StartOffsetIf(*param);
      AddExecLine("lw " + r1 + ", " + std::to_string(passed_param_offset + i) +
                  "(r14)");
      EndOffsetIf(*param);
      // Storing that passed value at the calculated offset residing inside the
      // function's call stack

      int offsetofparam;
      if (func_entry->Kind() == "func") {
        offsetofparam =
            node.symtab->ScopeSize() +
            func_symtab
                ->GetEntry("local",
                           std::dynamic_pointer_cast<FreeFuncEntry>(func_entry)
                               ->Params()
                               .at(idx_of_param)
                               .first)
                ->offset;
      } else {
        offsetofparam =
            node.symtab->ScopeSize() +
            func_symtab
                ->GetEntry("local", std::dynamic_pointer_cast<MemberFuncEntry>(
                                        func_entry)
                                        ->Params()
                                        .at(idx_of_param)
                                        .first)
                ->offset;
      }
      AddExecLine("sw " + std::to_string(offsetofparam + i) + "(r14), " + r1);
    }

    idx_of_param++;
  }
  // Increment stack frame and jump to function, here node.symtab is the caller
  // scope, e.g. main
  AddExecLine("addi r14, r14, " + std::to_string(node.symtab->ScopeSize()));
  AddExecLine("jl r15, " + func_tag);
  // Function done, decrement stack frame
  AddExecLine("subi r14, r14, " + std::to_string(node.symtab->ScopeSize()));

  // Copy the return value (which was the first thing in the function's call
  // stack) and store it on the current stack frame
  AddExecLine("lw " + r1 + ", " + std::to_string(node.symtab->ScopeSize()) +
              "(r14)");
  // Store in the space allocated for a temp var
  AddExecLine(
      "sw " +
      std::to_string(node.symtab->GetEntry(node.symtab_entry->Name())->offset) +
      "(r14), " + r1);
  register_pool_.push(r1);
}

// Here the assumption is that 0 is false and
// anything else is true
void CodeGenVisitor::Visit(NotNode& node) {
  DFS(node);
  std::string r1 = register_pool_.top();
  register_pool_.pop();
  std::string tagnum = std::to_string(GetNotNum());
  std::string nonzerotag = "notnonzero" + tagnum;
  std::string endnottag = "endnot" + tagnum;
  AddComment("not");
  std::string tmp_offset = std::to_string(node.symtab_entry->offset);
  std::string child_offset =
      std::to_string(node.ChildAt(0)->symtab_entry->offset);
  AddExecLine("lw " + r1 + ", " + child_offset + "(r14)");
  AddExecLine("bnz " + r1 + ", " + nonzerotag);
  AddExecLine("addi " + r1 + ", r0, 1");
  AddExecLine("sw " + tmp_offset + "(r14), " + r1);
  AddExecLine("j " + endnottag);
  AddExecLine(nonzerotag + " sw " + tmp_offset + "(r14), r0");
  AddExecLine(endnottag);
  register_pool_.push(r1);
  node.regist = node.ChildAt(0)->regist;
}

void CodeGenVisitor::Visit(MemberFuncDeclNode& node) {
  DFS(node);
  node.symtab = node.symtab_entry->Link();
}

void CodeGenVisitor::Visit(ArithExprNode& node) {
  DFS(node);
  node.symtab_entry = node.ChildAt(0)->symtab_entry;
  node.regist = node.ChildAt(0)->regist;
}

// Just dfs
void CodeGenVisitor::Visit(AParamsNode& node) { DFS(node); }
void CodeGenVisitor::Visit(ClassListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(ClassNode& node) { DFS(node); }
void CodeGenVisitor::Visit(DimListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(DimNode& node) { DFS(node); }
void CodeGenVisitor::Visit(FParamsListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(FParamsNode& node) { DFS(node); }
void CodeGenVisitor::Visit(FloatNumNode& node) { DFS(node); }
void CodeGenVisitor::Visit(FuncBodyNode& node) { DFS(node); }
void CodeGenVisitor::Visit(FuncDefListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(IdNode& node) { DFS(node); }
void CodeGenVisitor::Visit(IndiceList& node) { DFS(node); }
void CodeGenVisitor::Visit(InheritListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(MemberListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(MemberVarDeclNode& node) { DFS(node); }
void CodeGenVisitor::Visit(RelOpNode& node) { DFS(node); }
void CodeGenVisitor::Visit(ScopeResNode& node) { DFS(node); }
void CodeGenVisitor::Visit(SignNode& node) { DFS(node); }
void CodeGenVisitor::Visit(StatListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(TypeNode& node) { DFS(node); }
void CodeGenVisitor::Visit(VarDeclListNode& node) { DFS(node); }
void CodeGenVisitor::Visit(VarDeclNode& node) { DFS(node); }

// Gets the appropriate instruction given the operator
std::string CodeGenVisitor::GetInstructionFromOp(const std::string& op) {
  std::string instruction;
  // Add operators
  if (op == "+") {
    instruction = "add ";
  } else if (op == "-") {
    instruction = "sub ";
  } else if (op == "or") {
    instruction = "or ";
  }
  // Mult operators
  else if (op == "*") {
    instruction = "mul ";
  } else if (op == "/") {
    instruction = "div ";
  } else if (op == "and") {
    instruction = "and ";
  }
  // Relational operators
  if (op == "==") {
    instruction = "ceq ";
  } else if (op == "<>") {
    instruction = "cne ";
  } else if (op == "<") {
    instruction = "clt ";
  } else if (op == ">") {
    instruction = "cgt ";
  } else if (op == "<=") {
    instruction = "cle ";
  } else if (op == ">=") {
    instruction = "cge ";
  }
  return instruction;
}

// Add procedure if we didn't already
void CodeGenVisitor::AddLibProcedure(const std::string& name) {
  if (procedure_map_.find(name) != procedure_map_.end()) {
    procedures_ += procedure_map_.at(name) + "\n";
    procedure_map_.erase(name);
  }
}

std::unordered_map<std::string, std::string>
CodeGenVisitor::InitProcedureMap() {
  std::unordered_map<std::string, std::string> m;
  m["putint"] =
      "%\n"
      "%\n"
      "% --------------------------------------------------------------%\n"
      "% putint                                                        %\n"
      "% --------------------------------------------------------------%\n"
      "% Write an integer number to stdout. Transform the number into\n"
      "% ASCII string taking the sign into account.\n"
      "% Entry : integer number -> r1\n"
      "% Exit : none.\n"
      "%\n"
      "putint	align\n"
      "	add	r2,r0,r0		% Initialize buffer's index i \n"
      "	cge	r3,r1,r0		% True if N >= 0\n"
      "	bnz	r3,putint1		% Branch if True (N >= 0)\n"
      "	sub	r1,r0,r1		% N = -N\n"
      "putint1	modi	r4,r1,10		% Rightmost digit\n"
      "	addi	r4,r4,48		% Convert to ch\n"
      "	divi	r1,r1,10		% Truncate rightmost digit\n"
      "	sb	putint9(r2),r4		% Store ch in buffer\n"
      "	addi	r2,r2,1			% i++\n"
      "	bnz	r1,putint1		% Loop if not finished\n"
      "	bnz	r3,putint2		% Branch if True (N >= 0)\n"
      "	addi	r3,r0,45\n"
      "	sb	putint9(r2),r3		% Store '-' in buffer\n"
      "	addi	r2,r2,1			% i++\n"
      "	add	r1,r0,r0		% Initialize output register (r1 = 0)\n"
      "putint2	subi	r2,r2,1			% i--\n"
      "	lb	r1,putint9(r2)		% Load ch from buffer\n"
      "	putc	r1			% Output ch\n"
      "	bnz	r2,putint2		% Loop if not finished\n"
      "	addi r2,r0,10     % print a newline\n"
      "	putc r2           % print a newline\n"
      "	jr	r15			% return to the caller\n"
      "putint9	res	12			% loacl buffer (12 bytes)\n"
      "	align\n";
  m["getint"] =
      "%\n"
      "%\n"
      "% --------------------------------------------------------------%\n"
      "% getint                                                        %\n"
      "% --------------------------------------------------------------%\n"
      "% Read an integer number from stdin. Read until a non digit char\n"
      "% is entered. Allowes (+) & (-) signs only as first char. The\n"
      "% digits are read in ASCII and transformed to numbers.\n"
      "% Entry : none.\n"
      "% Exit : result -> r1\n"
      "%\n"
      "getint	align\n"
      "add	r1,r0,r0		% Initialize input register (r1 = 0)\n"
      "add	r2,r0,r0		% Initialize buffer's index i\n"
      "add	r4,r0,r0		% Initialize sign flag\n"
      "getint1	getc	r1			% Input ch\n"
      "ceqi	r3,r1,43		% ch = '+' ?\n"
      "bnz	r3,getint1		% Branch if true (ch = '+')\n"
      "ceqi	r3,r1,45		% ch = '-' ?\n"
      "bz	r3,getint2		% Branch if false (ch != '-')\n"
      "addi	r4,r0,1			% Set sign flag to true\n"
      "j	getint1			% Loop to get next ch\n"
      "getint2	clti	r3,r1,48		% ch < '0' ?\n"
      "bnz	r3,getint3		% Branch if true (ch < '0')\n"
      "cgti	r3,r1,57		% ch > '9' ?\n"
      "bnz	r3,getint3		% Branch if true (ch > '9')\n"
      "sb	getint9(r2),r1		% Store ch in buffer\n"
      "addi	r2,r2,1			% i++\n"
      "j	getint1			% Loop if not finished\n"
      "getint3	sb	getint9(r2),r0		% Store end of string in "
      "buffer (ch = '\\0')\n"
      "add	r2,r0,r0		% i = 0\n"
      "add	r1,r0,r0		% N = 0\n"
      "add	r3,r0,r0		% Initialize r3\n"
      "getint4	lb	r3,getint9(r2)		% Load ch from buffer\n"
      "bz	r3,getint5		% Branch if end of string (ch = "
      "'\\0')\n"
      "subi	r3,r3,48		% Convert to number (M)\n"
      "muli	r1,r1,10		% N *= 10\n"
      "add	r1,r1,r3		% N += M\n"
      "addi	r2,r2,1			% i++\n"
      "j	getint4			% Loop if not finished\n"
      "getint5	bz	r4,getint6		% Branch if sign flag false\n"
      "sub	r1,r0,r1		% N = -N\n"
      "getint6	jr	r15			% Return to the caller\n"
      "getint9	res	12			% Local buffer (12 bytes)\n"
      "align\n"
      "%\n"
      "%\n";
  return m;
}

}  // namespace toy