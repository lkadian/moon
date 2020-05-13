#include "parser.h"

#include "gtest/gtest.h"
#include "lexer.h"
#include "logger.h"

namespace parsertest {

using namespace toy;

class ParserTest : public ::testing::Test {
 protected:
  ParserTest() {}
  virtual ~ParserTest() {}
  virtual void SetUp() { Logger::Clear(); }
  virtual void TearDown() {}
};

TEST_F(ParserTest, Test1) {
  std::ifstream file_stream("../test/fixtures/parser/minProg.src");
  Lexer lexer(file_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  parser.Parse();
  EXPECT_FALSE(Logger::HasErrors());
}

TEST_F(ParserTest, Test2) {
  std::ifstream file_stream("../test/fixtures/parser/bubblesort.src");
  Lexer lexer(file_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  parser.Parse();
  EXPECT_FALSE(Logger::HasErrors());
}

TEST_F(ParserTest, Test3) {
  std::ifstream file_stream("../test/fixtures/parser/polynomial-errors.src");
  Lexer lexer(file_stream);
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  parser.Parse();
  std::vector<std::string> actual_errors = Logger::GetErrors();
  std::vector<std::string> expected_errors = {
      "SyntaxError: Expected '';'', but next token was '}' (line 20) \n"};

  EXPECT_EQ(actual_errors, expected_errors);
}

}  // namespace parsertest