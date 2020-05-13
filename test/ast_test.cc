#include "ast.h"

#include "gtest/gtest.h"
#include "parser.h"

namespace asttest {

using namespace toy;

class ASTTest : public ::testing::Test {
 protected:
  ASTTest() {}
  virtual ~ASTTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ASTTest, TestBubblesort) {
  std::ifstream prog_stream("../test/fixtures/parser/bubblesort.src");
  Lexer lexer(prog_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  auto ast = parser.Parse();
  ast->ToDotFile("../test/fixtures/ast/test.gv");

  std::ifstream expected_stream("../test/fixtures/ast/bubblesort.gv");
  std::string expected_str((std::istreambuf_iterator<char>(expected_stream)),
                           std::istreambuf_iterator<char>());

  std::ifstream actual_stream("../test/fixtures/ast/test.gv");
  std::string actual_str((std::istreambuf_iterator<char>(actual_stream)),
                         std::istreambuf_iterator<char>());

  EXPECT_EQ(expected_str, actual_str);
}

TEST_F(ASTTest, TestPolynomial) {
  std::ifstream prog_stream("../test/fixtures/parser/polynomial.src");
  Lexer lexer(prog_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  auto ast = parser.Parse();
  ast->ToDotFile("../test/fixtures/ast/test.gv");

  std::ifstream expected_stream("../test/fixtures/ast/polynomial.gv");
  std::string expected_str((std::istreambuf_iterator<char>(expected_stream)),
                           std::istreambuf_iterator<char>());

  std::ifstream actual_stream("../test/fixtures/ast/test.gv");
  std::string actual_str((std::istreambuf_iterator<char>(actual_stream)),
                         std::istreambuf_iterator<char>());

  EXPECT_EQ(expected_str, actual_str);
}

TEST_F(ASTTest, TestSimpleMain) {
  std::ifstream prog_stream("../test/fixtures/parser/simplemain.src");
  Lexer lexer(prog_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  auto ast = parser.Parse();
  ast->ToDotFile("../test/fixtures/ast/test.gv");

  std::ifstream expected_stream("../test/fixtures/ast/simplemain.gv");
  std::string expected_str((std::istreambuf_iterator<char>(expected_stream)),
                           std::istreambuf_iterator<char>());

  std::ifstream actual_stream("../test/fixtures/ast/test.gv");
  std::string actual_str((std::istreambuf_iterator<char>(actual_stream)),
                         std::istreambuf_iterator<char>());

  EXPECT_EQ(expected_str, actual_str);
}

}  // namespace asttest