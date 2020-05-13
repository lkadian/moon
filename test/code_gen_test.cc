#include "ast.h"
#include "code_gen_visitor.h"
#include "gtest/gtest.h"
#include "logger.h"
#include "mem_size_visitor.h"
#include "parser.h"
#include "symbol_table_visitor.h"
#include "type_check_visitor.h"
#include "util.h"

namespace codegentest {

class CodeGenTest : public ::testing::Test {
 protected:
  CodeGenTest() {}
  virtual ~CodeGenTest() {}
  virtual void SetUp() { toy::Logger::Clear(); }
  virtual void TearDown() {}
};

std::vector<std::string> CompileAndRunCode(std::string filepath) {
  std::ifstream prog_stream(filepath);
  toy::Lexer lexer(prog_stream);
  toy::Grammar grammar;
  toy::ParserGenerator pgen(grammar);
  toy::Parser parser(lexer, pgen);
  auto ast = parser.Parse();
  toy::SymbolTableVisitor symtab_visitor;
  ast->Accept(symtab_visitor);
  toy::TypeCheckVisitor typecheck_visitor;
  ast->Accept(typecheck_visitor);
  toy::MemSizeVisitor memsize_visitor;
  ast->Accept(memsize_visitor);
  toy::CodeGenVisitor codegen_visitor;
  ast->Accept(codegen_visitor);
  std::string res_str = toy::util::ExeGeneratedCode();
  auto res = toy::util::SplitStrByNewline(res_str);
  res.erase(res.begin());
  res.erase(res.end());
  return res;
}

// Basic test (assign, add, write)
TEST_F(CodeGenTest, TestCodeGen1) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test1.src");

  std::vector<std::string> expected_res = {"8", "10", "18", "-2", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for addOp/multOp
TEST_F(CodeGenTest, TestCodeGen2) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test2.src");

  std::vector<std::string> expected_res = {"3", "2", "-5", "26",
                                           "5", "3", "39", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for relOp
TEST_F(CodeGenTest, TestCodeGen3) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test3.src");

  std::vector<std::string> expected_res = {
      "0", "1", "0", "1", "0", "1", "1", "0", "1", "0", "1", "1",
      "0", "1", "1", "0", "1", "1", "1", "1", "0", "1", "1", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for ifStat
TEST_F(CodeGenTest, TestCodeGen4) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test4.src");

  std::vector<std::string> expected_res = {"-9", "9", "-31", "9", "-32", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for While
TEST_F(CodeGenTest, TestCodeGen5) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test5.src");

  std::vector<std::string> expected_res = {
      "0",  "1",  "2",  "3",  "4",  "5", "6", "7", "8", "9",

      "10", "9",  "8",  "7",  "6",  "5", "4", "3", "2", "1",

      "-5", "-4", "-3", "-2", "-1", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Tests recursion (factorial)
TEST_F(CodeGenTest, TestCodeGen6) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test6.src");
  std::vector<std::string> expected_res = {"40320", "47", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Tests recursion (factorial), with more than one param
TEST_F(CodeGenTest, TestCodeGen7) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test7.src");
  std::vector<std::string> expected_res = {
      "-9", "-9",      "-9", "-9", "-9", "-9", "-9", "-9",  "-9",
      "-9", "3628800", "-9", "-9", "-9", "-9", "-9", "120", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test 2d array
TEST_F(CodeGenTest, TestCodeGen8) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test8.src");
  std::vector<std::string> expected_res = {"1",  "2",  "3",  "4",  "5",  "6",
                                           "7",  "8",  "9",  "10", "11", "12",
                                           "13", "14", "15", "27", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test regular array
TEST_F(CodeGenTest, TestCodeGen9) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test9.src");
  std::vector<std::string> expected_res = {"1",  "2",  "3",  "4",  "5",  "6",
                                           "7",  "8",  "9",  "10", "11", "12",
                                           "13", "14", "15", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test more array stuff
TEST_F(CodeGenTest, TestCodeGen10) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test10.src");
  std::vector<std::string> expected_res = {"16", "1", "5", "1", "10",
                                           "9",  "8", "7", "6", "5",
                                           "4",  "3", "2", "1", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test more array stuff
TEST_F(CodeGenTest, TestCodeGen11) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test11.src");
  std::vector<std::string> expected_res = {"7", "-1334", "37", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for fibonacci
TEST_F(CodeGenTest, TestCodeGen12) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test12.src");
  std::vector<std::string> expected_res = {"0", "1",  "1",  "2",  "3",  "5",
                                           "8", "13", "21", "34", "55", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test for classes with only data members
TEST_F(CodeGenTest, TestCodeGen13) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test13.src");
  std::vector<std::string> expected_res = {"1", "-7", "3", "4", "5", "20", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test not
TEST_F(CodeGenTest, TestCodeGen14) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test14.src");
  std::vector<std::string> expected_res = {"1", "0", "1", "0", "0", "1", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test logical operators (and, or, not)
TEST_F(CodeGenTest, TestCodeGen15) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test15.src");
  std::vector<std::string> expected_res = {"0", "1", "1", "0", "0", "-4",
                                           "1", "1", "1", "0", "1", "0",
                                           "0", "0", "0", "1", "1", ""};
  EXPECT_EQ(actual_res, expected_res);
}

// Test passing array to function.
TEST_F(CodeGenTest, TestCodeGen16) {
  std::vector<std::string> actual_res =
      CompileAndRunCode("../test/fixtures/codegen/Test16.src");
  std::vector<std::string> expected_res = {"1", "2", "3", "4", "5", "1",
                                           "2", "3", "4", "5", "1", "2",
                                           "3", "4", "5", ""};
  EXPECT_EQ(actual_res, expected_res);
}

}  // namespace codegentest