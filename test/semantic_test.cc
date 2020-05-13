#include "ast.h"
#include "gtest/gtest.h"
#include "logger.h"
#include "parser.h"
#include "symbol_table_visitor.h"
#include "type_check_visitor.h"

namespace semantictest {

class SemanticTest : public ::testing::Test {
 protected:
  SemanticTest() {}
  virtual ~SemanticTest() {}
  virtual void SetUp() { toy::Logger::Clear(); }
  virtual void TearDown() {}
};

// Lex, parse and detect semantic errors for the given file
void SemanticTestHelper(const std::string& filepath) {
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
}

// 5. During symbol table creation, there are some semantic errors that are
// detected and reported, such as multiply declared identifiers in the same
// scope, as well warnings such as for shadowed inherited members.
TEST_F(SemanticTest, TestSemantic1) {
  SemanticTestHelper("../test/fixtures/semantic/Test1.src");

  std::vector<std::string> expected_errors(
      {"SemanticError: Member variable 'b' already declared in 'LINEAR' (line "
       "10) \n",
       "SemanticError: Variable 'new_function' already declared in scope "
       "'build' (line 54) \n",
       "SemanticError: Variable 'f1' already declared in scope 'main' (line "
       "78) \n"});
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();

  EXPECT_EQ(expected_errors, actual_errors);

  std::vector<std::string> expected_warnings(
      {"SemanticWarning: Shadowed member variable 'z' in 'QUADRATIC' already "
       "declared in 'POLYNOMIAL' (line 3) \n"});
  std::vector<std::string> actual_warnings = toy::Logger::GetWarnings();
  EXPECT_EQ(expected_warnings, actual_warnings);
}

// 6. All declared member functions should have a corresponding function
// definition, and inversely. A member function that is declared but not defined
// constitutes an “no definition for declared member function” semantic error.
// If a member function is defined but not declared, it constitutes an
// “definition provided for undeclared member function” semantic error.
TEST_F(SemanticTest, TestSemantic2) {
  SemanticTestHelper("../test/fixtures/semantic/Test2.src");

  std::vector<std::string> expected_errors(
      {"SemanticError: Declared function 'LINEAR::build(float, float)' has no "
       "definition (line 9) \n",
       "SemanticError: Definition provided for undeclared member function "
       "'LINEAR::evaluate(float)' (line 13) \n"});
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();

  EXPECT_EQ(expected_errors, actual_errors);
}

// 8. Class and variable identifiers cannot be declared twice in the same scope.
// In such a case, a “multiply declared class”, “multiply declared data member”,
// or multiply declared local variable” semantic error message is issued.
TEST_F(SemanticTest, TestSemantic3) {
  SemanticTestHelper("../test/fixtures/semantic/Test3.src");

  std::vector<std::string> expected_errors({
      "SemanticError: Class 'POLYNOMIAL' already declared in 'global' (line 6) "
      "\n",
      "SemanticError: Member variable 'a' already declared in 'LINEAR' (line "
      "12) \n",
      "SemanticError: Class 'QUADRATIC' already declared in 'global' (line 28) "
      "\n",
      "SemanticError: Variable 'f2' already declared in scope 'main' (line 88) "
      "\n",
  });
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();

  EXPECT_EQ(expected_errors, actual_errors);
}

// 9. Function overloading (i.e. two functions with the same name but with
// different parameter lists) should be allowed and reported as a semantic
// warning. This applies to member functions and free functions.
TEST_F(SemanticTest, TestSemantic4) {
  SemanticTestHelper("../test/fixtures/semantic/Test4.src");

  std::vector<std::string> expected_warnings(
      {"SemanticWarning: Overloaded member function "
       "'QUADRATIC::evaluate(integer)' in 'QUADRATIC' (line 21) \n",
       "SemanticWarning: Overloaded function 'QUADRATIC::evaluate(integer)' in "
       "'global' (line 49) \n",
       "SemanticWarning: Overloaded function '::fun1(integer, integer)' in "
       "'global' (line 79) \n"});
  std::vector<std::string> actual_warnings = toy::Logger::GetWarnings();

  EXPECT_EQ(expected_warnings, actual_warnings);
}

// 10. Type checking is applied on expressions (i.e. the type of sub-expressions
// should be inferred). Type checking should also be done for assignment (the
// type of the left and right hand side of the assignment operator must be the
// same) and return statements (the type of the returned value must be the same
// as the return type of the function, as declared in its function header).

// expect no errors
TEST_F(SemanticTest, TestSemantic5) {
  SemanticTestHelper("../test/fixtures/semantic/Test5.src");
  std::vector<std::string> expected_errors({});
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}
// expect errors
TEST_F(SemanticTest, TestSemantic6) {
  SemanticTestHelper("../test/fixtures/semantic/Test6.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: Function 'fun1' has return type 'float' but returns "
       "'integer' (line 4) \n",
       "SemanticError: Mismatching types 'float' and 'integer' in add "
       "operation (line 14) \n",
       "SemanticError: Mismatching types 'float' and 'typeerror' in assign "
       "operation (line 14) \n",
       "SemanticError: Mismatching types 'integer' and 'float' in mult "
       "operation (line 16) \n",
       "SemanticError: Mismatching types 'float' and 'typeerror' in assign "
       "operation (line 16) \n"});
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// 11. Any identifier referred to must be defined in the scope where it is used
// (failure should result in the following error messages: “use of undeclared
// local variable”, “use of undeclared free function”, “use of undeclared
// class”).
TEST_F(SemanticTest, TestSemantic7) {
  SemanticTestHelper("../test/fixtures/semantic/Test7.src");
  std::vector<std::string> expected_errors(

      {"SemanticError: Use of undeclared variable 'z' (line 46) \n",
       "SemanticError: Mismatching types 'typeerror' and 'integer' in assign "
       "operation (line 46) \n",
       "SemanticError: Use of undeclared free function '::add(integer, "
       "integer)' (line 47) \n",
       "SemanticError: Mismatching types 'float' and 'typeerror' in assign "
       "operation (line 47) \n"});
  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// 12. Function calls are made with the right number and type of parameters.
// Expressions passed as parameters in a function call must be of the same type
// as declared in the function declaration.
TEST_F(SemanticTest, TestSemantic8) {
  SemanticTestHelper("../test/fixtures/semantic/Test8.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: Undeclared member function 'LINEAR::build(float, "
       "integer)' not found in 'LINEAR' "
       "(line 51) \n",
       "SemanticError: Undeclared member function 'LINEAR::build(integer)' not "
       "found in 'LINEAR' (line "
       "52) \n",
       "SemanticError: Use of undeclared free function '::add(integer, "
       "integer)' (line 53) \n"});

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// 13. Referring to an array variable should be made using the same number of
// dimensions as declared in the variable declaration. Expressions used as an
// index must be of integer type. When passing an array as a parameter, the
// passed array must be of compatible dimensionality compared to the parameter
// declaration.
TEST_F(SemanticTest, TestSemantic9) {
  SemanticTestHelper("../test/fixtures/semantic/Test9.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: Expresions used as an index must be of integer type "
       "(line 61) \n",
       "SemanticError: Use of undeclared free function '::printArray(integer, "
       "integer)' (line 68) \n",
       "SemanticError: Cannot refer to an array of dimension 1 with 2 indices "
       "(line 69) \n",
       "SemanticError: Use of undeclared free function "
       "'::bubbleSort(typeerror, integer)' (line 69) \n"});

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// 14. Circular class dependencies (through data members\inheritance) should be
// reported as semantic errors.
TEST_F(SemanticTest, TestSemantic10) {
  SemanticTestHelper("../test/fixtures/semantic/Test10.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: Cyclic class dependency detected from class "
       "QUADRATIC\n"});

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// 15. The “.” operator should be used only on variables of a class type. If so,
// its right operand must be a member of that class. If not, a “undeclared data
// member” or “undeclared member function” semantic error should be issued.
TEST_F(SemanticTest, TestSemantic11) {
  SemanticTestHelper("../test/fixtures/semantic/Test11.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: dot operator can only be used on variables of class "
       "type (line 75) \n",
       "SemanticError: Mismatching types 'float' and 'typeerror' in assign "
       "operation (line 75) \n",
       "SemanticError: dot operator can only be used on variables of class "
       "type (line 76) \n",
       "SemanticError: Mismatching types 'float' and 'typeerror' in assign "
       "operation (line 76) \n",
       "SemanticError: Undeclared data member 'k' not found in 'QUADRATIC' "
       "(line 78) \n",
       "SemanticError: Mismatching types 'float' and 'typerror' in assign "
       "operation (line 78) \n",
       "SemanticError: Undeclared member function 'QUADRATIC::run()' not found "
       "in 'QUADRATIC' (line 79) \n",
       "SemanticError: Mismatching types 'float' and 'typerror' in assign "
       "operation (line 79) \n",
       "SemanticError: Can't access private variables from outside the class "
       "(line 81) \n",
       "SemanticError: Mismatching types 'float' and 'LINEAR' in assign "
       "operation (line 81) \n"

      });

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

TEST_F(SemanticTest, TestSemantic12) {
  SemanticTestHelper("../test/fixtures/semantic/Test12.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: dot operator can only be used on variables of class "
       "type "
       "(line 75) \n",
       "SemanticError: Mismatching types 'typeerror' and 'integer' in assign "
       "operation (line 75) \n",
       "SemanticError: Can't access private functions from outside the class "
       "(line 76) \n",
       "SemanticError: Undeclared member function "
       "'QUADRATIC::evaluate(integer)' not found in 'QUADRATIC' (line 78) \n",
       "SemanticError: Mismatching types 'float' and 'integer' in assign "
       "operation (line 79) \n",
       "SemanticError: Can't access private variables from outside the class "
       "(line 81) \n",
       "SemanticError: Mismatching types 'QUADRATIC' and 'float' in assign "
       "operation (line 81) \n"});

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

// Test for cycles due to memberVar
TEST_F(SemanticTest, TestSemantic13) {
  SemanticTestHelper("../test/fixtures/semantic/Test13.src");
  std::vector<std::string> expected_errors(
      {"SemanticError: Cyclic class dependency detected from class "
       "QUADRATIC\n"});

  std::vector<std::string> actual_errors = toy::Logger::GetErrors();
  EXPECT_EQ(expected_errors, actual_errors);
}

}  // namespace semantictest