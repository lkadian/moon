#include "lexer.h"

#include <vector>

#include "gtest/gtest.h"
#include "token.h"

namespace lexertest {

using namespace toy;

class LexerTest : public ::testing::Test {
 protected:
  LexerTest() {}
  virtual ~LexerTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(LexerTest, Test1) {
  Token expected_tk(TokenType::ID, "abc", 1);
  std::istringstream is("abc");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test2) {
  Token expected_tk(TokenType::ID, "abc1", 1);
  std::istringstream is("abc1");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test3) {
  Token expected_tk(TokenType::ID, "abc_1", 1);
  std::istringstream is("abc_1");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test4) {
  Token expected_tk(TokenType::ID, "abc1_", 1);
  std::istringstream is("abc1_");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test5) {
  Token expected_tk(TokenType::INVALID_ID, "_abc1", 1);
  std::istringstream is("_abc1");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test6) {
  Token expected_tk(TokenType::INVALID_NUM, "0123", 1);
  std::istringstream is("0123");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test7) {
  Token expected_tk(TokenType::INVALID_NUM, "01.23", 1);
  std::istringstream is("01.23");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test8) {
  Token expected_tk(TokenType::FLOATNUM, "12345.6789e-123", 1);
  std::istringstream is("12345.6789e-123");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test9) {
  Token expected_tk(TokenType::INVALID_NUM, "12.340", 1);
  std::istringstream is("12.340");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test10) {
  Token expected_tk(TokenType::INVALID_NUM, "012.34", 1);
  std::istringstream is("012.34");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test11) {
  Token expected_tk(TokenType::INVALID_NUM, "12.34e01", 1);
  std::istringstream is("12.34e01");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test12) {
  Token expected_tk(TokenType::INTNUM, "12345", 1);
  std::istringstream is("12345");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test13) {
  Token expected_tk(TokenType::INVALID_NUM, "1abc", 1);
  std::istringstream is("1abc");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test14) {
  Token expected_tk(TokenType::INVALID_ID, "_1abc", 1);
  std::istringstream is("_1abc");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test15) {
  Token expected_tk(TokenType::FLOATNUM, "12.34e+10", 1);
  std::istringstream is("12.34e+10");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test16) {
  Token expected_tk(TokenType::FLOATNUM, "12.34e10", 1);
  std::istringstream is("12.34e10");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test17) {
  std::vector<Token> expected_tks = {Token(TokenType::INVALID_CHAR, "@", 1),
                                     Token(TokenType::INVALID_CHAR, "#", 1),
                                     Token(TokenType::INVALID_CHAR, "$", 1),
                                     Token(TokenType::INVALID_CHAR, "!", 1),
                                     Token(TokenType::INVALID_CHAR, "\"", 1),
                                     Token(TokenType::INVALID_CHAR, "'", 1),
                                     Token(TokenType::INVALID_CHAR, "?", 1),
                                     Token(TokenType::INVALID_CHAR, "\\", 1),
                                     Token(TokenType::INVALID_CHAR, "|", 1),
                                     Token(TokenType::INVALID_CHAR, "&", 1),
                                     Token(TokenType::INVALID_CHAR, "!", 1)};

  std::istringstream is("@# $ ! \" ' ? \\ | & !");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test18) {
  Token expected_tk(TokenType::INLINE_CMT, "// ... 1234 abc1", 1);
  std::istringstream is("// ... 1234 abc1");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test19) {
  Token expected_tk(TokenType::BLOCK_CMT, "/* ... 1234 abc1 */", 1);
  std::istringstream is("/* ... 1234 abc1 */");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test20) {
  Token expected_tk(TokenType::FLOATNUM, "1.23", 1);
  std::istringstream is("1.23");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test21) {
  Token expected_tk(TokenType::FLOATNUM, "12.34", 1);
  std::istringstream is("12.34");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test22) {
  Token expected_tk(TokenType::FLOATNUM, "12345.6789e-123", 1);
  std::istringstream is("12345.6789e-123");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test23) {
  Token expected_tk(TokenType::FLOATNUM, "0.123", 1);
  std::istringstream is("0.123");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test24) {
  Token expected_tk(TokenType::INVALID_NUM, "120.340", 1);
  std::istringstream is("120.340e10");
  std::vector<Token> expected_tks = {
      Token(TokenType::INVALID_NUM, "120.340", 1),
      Token(TokenType::ID, "e10", 1)};
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test25) {
  Token expected_tk(TokenType::INVALID_NUM, "0e0", 1);
  std::istringstream is("0e0");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test26) {
  Token expected_tk(TokenType::ID, "E52_", 1);
  std::istringstream is("E52_");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test27) {
  Token expected_tk(TokenType::INVALID_NUM, "0.760", 1);
  std::istringstream is("0.760");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test28) {
  Token expected_tk(TokenType::UNTERMINATED_CMT, "/* hello", 1);
  std::istringstream is("/* hello");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test29) {
  Token expected_tk(TokenType::UNTERMINATED_CMT, "/* hello *", 1);
  std::istringstream is("/* hello *");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test30) {
  Token expected_tk(TokenType::INTNUM, "0", 1);
  std::istringstream is("0");
  Lexer lexer(is);
  Token actual_tk = lexer.NextToken();
  EXPECT_EQ(expected_tk, actual_tk);
}

TEST_F(LexerTest, Test31) {
  std::vector<Token> expected_tks = {Token(TokenType::INTNUM, "5", 1),
                                     Token(TokenType::ASSGN, "=", 1),
                                     Token(TokenType::INTNUM, "1", 1)};
  std::istringstream is("5=1");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test32) {
  std::vector<Token> expected_tks = {Token(TokenType::INTNUM, "5", 1),
                                     Token(TokenType::EQ, "==", 1),
                                     Token(TokenType::INTNUM, "1", 1)};
  std::istringstream is("5==1");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test33) {
  std::vector<Token> expected_tks = {Token(TokenType::INTNUM, "5", 1),
                                     Token(TokenType::DIV, "/", 1),
                                     Token(TokenType::INTNUM, "1", 1)};
  std::istringstream is("5 /1");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test34) {
  std::vector<Token> expected_tks = {Token(TokenType::INTNUM, "0", 1),
                                     Token(TokenType::MULT, "*", 1),
                                     Token(TokenType::INTNUM, "5", 1)};
  std::istringstream is("0*5");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test35) {
  std::vector<Token> expected_tks = {Token(TokenType::FLOATNUM, "5.14e+10", 1),
                                     Token(TokenType::PLUS, "+", 1),
                                     Token(TokenType::FLOATNUM, "0.12", 1)};
  std::istringstream is("5.14e+10 +0.12");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test36) {
  std::vector<Token> expected_tks = {Token(TokenType::INVALID_NUM, "5.140", 1),
                                     Token(TokenType::MINUS, "-", 1),
                                     Token(TokenType::INVALID_NUM, "00", 1)};
  std::istringstream is("5.140-00");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test37) {
  std::vector<Token> expected_tks = {Token(TokenType::INVALID_NUM, "52.", 1),
                                     Token(TokenType::ID, "abc", 1)};
  std::istringstream is("52.abc");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test38) {
  std::vector<Token> expected_tks = {Token(TokenType::OPEN_PAR, "(", 1),
                                     Token(TokenType::INTNUM, "52", 1),
                                     Token(TokenType::MULT, "*", 1),
                                     Token(TokenType::INTNUM, "32", 1),
                                     Token(TokenType::CLOSE_PAR, ")", 1),
                                     Token(TokenType::PLUS, "+", 1),
                                     Token(TokenType::OPEN_PAR, "(", 1),
                                     Token(TokenType::FLOATNUM, "12.03", 1),
                                     Token(TokenType::CLOSE_PAR, ")", 1)};
  std::istringstream is("(52*32) +(12.03)");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test39) {
  std::vector<Token> expected_tks = {Token(TokenType::VOID, "void", 1),
                                     Token(TokenType::ID, "func", 1)};
  std::istringstream is("void func");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test40) {
  std::vector<Token> expected_tks = {Token(TokenType::OPEN_PAR, "(", 1),
                                     Token(TokenType::ID, "abc_1", 1),
                                     Token(TokenType::CLOSE_PAR, ")", 1)};
  std::istringstream is("(abc_1)");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  while (lexer.HasNext()) {
    actual_tks.push_back(lexer.NextToken());
  }
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test41) {
  std::vector<Token> expected_tks = {Token(TokenType::EOS, "$", 1),
                                     Token(TokenType::EOS, "$", 1),
                                     Token(TokenType::EOS, "$", 1)};
  std::istringstream is("");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  actual_tks.push_back(lexer.NextToken());
  actual_tks.push_back(lexer.NextToken());
  actual_tks.push_back(lexer.NextToken());
  EXPECT_EQ(expected_tks, actual_tks);
}

TEST_F(LexerTest, Test42) {
  std::vector<Token> expected_tks = {Token(TokenType::EOS, "$", 3),
                                     Token(TokenType::EOS, "$", 3),
                                     Token(TokenType::EOS, "$", 3)};
  std::istringstream is("\n\n");
  Lexer lexer(is);
  std::vector<Token> actual_tks;
  actual_tks.push_back(lexer.NextToken());
  actual_tks.push_back(lexer.NextToken());
  actual_tks.push_back(lexer.NextToken());
  EXPECT_EQ(expected_tks, actual_tks);
}

}  // namespace lexertest