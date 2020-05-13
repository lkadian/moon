#include "parser_gen.h"

#include "gtest/gtest.h"

namespace pgentest {

using namespace toy;

class ParserGenTest : public ::testing::Test {
 protected:
  ParserGenTest() {}
  virtual ~ParserGenTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
  ParserGenerator pgen = ParserGenerator(Grammar());
  Sets actual_first_sets = pgen.FirstSets();
  Sets actual_follow_sets = pgen.FollowSets();
};

TEST_F(ParserGenTest, TestStartSymbol) {
  Symbol symb("<START>");
  Set expected_first_set = {Symbol("'main'"), Symbol("'class'"),
                            Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'$'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVariableFactor2Symbol) {
  Symbol symb("<variableFactor2>");
  Set expected_first_set = {Symbol("'('"), Symbol("'.'"), Symbol("'['"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestContinueVariableFactor) {
  Symbol symb("<continueVariableFactor>");
  Set expected_first_set = {Symbol("'.'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVisibility) {
  Symbol symb("<visibility>");
  Set expected_first_set = {Symbol("'public'"), Symbol("'private'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'id'"), Symbol("'integer'"),
                             Symbol("'float'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFuncHead) {
  Symbol symb("<funcHead>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'do'"), Symbol("'local'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFunctionCallOrVariableFactor2) {
  Symbol symb("<functionCallOrVariableFactor2>");
  Set expected_first_set = {Symbol("'('"), Symbol("'.'"), Symbol("'['"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestContinueFunctionCallOrVariableFactor) {
  Symbol symb("<continueFunctionCallOrVariableFactor>");
  Set expected_first_set = {Symbol("'.'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestMemberDecl) {
  Symbol symb("<memberDecl>");
  Set expected_first_set = {Symbol("'id'"), Symbol("'integer'"),
                            Symbol("'float'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'}'"), Symbol("'public'"),
                             Symbol("'private'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestClassBodyRept0) {
  Symbol symb("<classBodyRept0>");
  Set expected_first_set = {Symbol("'public'"), Symbol("'private'"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'}'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestArraySizeRept0) {
  Symbol symb("<arraySizeRept0>");
  Set expected_first_set = {Symbol("'['"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'"), Symbol("','"), Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestIndice) {
  Symbol symb("<indice>");
  Set expected_first_set = {Symbol("'['")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("'.'"),   Symbol("'='"),  Symbol("';'"),
      Symbol("','"),   Symbol("')'"),   Symbol("'['")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAssignStatOrFunctionCallFactor2) {
  Symbol symb("<assignStatOrFunctionCallFactor2>");
  Set expected_first_set = {Symbol("'('"), Symbol("'.'"), Symbol("'['"),
                            Symbol("'='")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestIndiceRept0) {
  Symbol symb("<indiceRept0>");
  Set expected_first_set = {Symbol("'['"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'+'"),   Symbol("'-'"),  Symbol("'or'"),
                             Symbol("'*'"),   Symbol("'/'"),  Symbol("'and'"),
                             Symbol("']'"),   Symbol("'eq'"), Symbol("'neq'"),
                             Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
                             Symbol("'geq'"), Symbol("'.'"),  Symbol("'='"),
                             Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestContinueAssignStatFactor) {
  Symbol symb("<continueAssignStatFactor>");
  Set expected_first_set = {Symbol("'.'"), Symbol("'='")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAParams) {
  Symbol symb("<aParams>");
  Set expected_first_set = {Symbol("'intNum'"), Symbol("'floatNum'"),
                            Symbol("'('"),      Symbol("'not'"),
                            Symbol("'+'"),      Symbol("'-'"),
                            Symbol("'id'"),     Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestContinueFunctionCallFactor) {
  Symbol symb("<continueFunctionCallFactor>");
  Set expected_first_set = {Symbol("'.'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFuncLocalOpt) {
  Symbol symb("<funcLocalOpt>");
  Set expected_first_set = {Symbol("'local'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'do'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestStatBlock) {
  Symbol symb("<statBlock>");
  Set expected_first_set = {Symbol("'do'"),    Symbol("'if'"),
                            Symbol("'while'"), Symbol("'read'"),
                            Symbol("'write'"), Symbol("'return'"),
                            Symbol("'id'"),    Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'else'"), Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAssignOp) {
  Symbol symb("<assignOp>");
  Set expected_first_set = {Symbol("'='")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestRelExpr) {
  Symbol symb("<relExpr>");
  Set expected_first_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestRelop) {
  Symbol symb("<relOp>");
  Set expected_first_set = {Symbol("'eq'"), Symbol("'neq'"), Symbol("'lt'"),
                            Symbol("'gt'"), Symbol("'leq'"), Symbol("'geq'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVariable) {
  Symbol symb("<variable>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestSign) {
  Symbol symb("<sign>");
  Set expected_first_set = {Symbol("'+'"), Symbol("'-'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestProg) {
  Symbol symb("<prog>");
  Set expected_first_set = {Symbol("'main'"), Symbol("'class'"),
                            Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'$'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFuncBody) {
  Symbol symb("<funcBody>");
  Set expected_first_set = {Symbol("'do'"), Symbol("'local'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'id'"), Symbol("'main'"), Symbol("'$'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestStatement) {
  Symbol symb("<statement>");
  Set expected_first_set = {Symbol("'if'"),     Symbol("'while'"),
                            Symbol("'read'"),   Symbol("'write'"),
                            Symbol("'return'"), Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'if'"),    Symbol("'while'"),  Symbol("'read'"),
      Symbol("'write'"), Symbol("'return'"), Symbol("'id'"),
      Symbol("'end'"),   Symbol("'else'"),   Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestStatementRept0) {
  Symbol symb("<statementRept0>");
  Set expected_first_set = {Symbol("'if'"),     Symbol("'while'"),
                            Symbol("'read'"),   Symbol("'write'"),
                            Symbol("'return'"), Symbol("'id'"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'end'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFactor) {
  Symbol symb("<factor>");
  Set expected_first_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestTermRightRec) {
  Symbol symb("<termRightRec>");
  Set expected_first_set = {Symbol("'*'"), Symbol("'/'"), Symbol("'and'"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'+'"),   Symbol("'-'"),  Symbol("'or'"),
                             Symbol("']'"),   Symbol("'eq'"), Symbol("'neq'"),
                             Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
                             Symbol("'geq'"), Symbol("';'"),  Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestMultOp) {
  Symbol symb("<multOp>");
  Set expected_first_set = {Symbol("'*'"), Symbol("'/'"), Symbol("'and'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAddOp) {
  Symbol symb("<addOp>");
  Set expected_first_set = {Symbol("'+'"), Symbol("'-'"), Symbol("'or'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestScopeResOp) {
  Symbol symb("<scopeResOp>");
  Set expected_first_set = {Symbol("'sr'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'('")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestExpr) {
  Symbol symb("<expr>");
  Set expected_first_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"), Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'"), Symbol("')'"), Symbol("','")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestExprArithOrRel) {
  Symbol symb("<exprArithOrRel>");
  Set expected_first_set = {Symbol("'eq'"),   Symbol("'neq'"), Symbol("'lt'"),
                            Symbol("'gt'"),   Symbol("'leq'"), Symbol("'geq'"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'"), Symbol("')'"), Symbol("','")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFunctionCallOrVariable) {
  Symbol symb("<functionCallOrVariable>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFunctionCallOrVariableFactor1) {
  Symbol symb("<functionCallOrVariableFactor1>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {
      Symbol("'+'"),   Symbol("'-'"),   Symbol("'or'"), Symbol("'*'"),
      Symbol("'/'"),   Symbol("'and'"), Symbol("']'"),  Symbol("'eq'"),
      Symbol("'neq'"), Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
      Symbol("'geq'"), Symbol("';'"),   Symbol("','"),  Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestType) {
  Symbol symb("<type>");
  Set expected_first_set = {Symbol("'integer'"), Symbol("'float'"),
                            Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'"), Symbol("'id'"), Symbol("'do'"),
                             Symbol("'local'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestArithExpr) {
  Symbol symb("<arithExpr>");
  Set expected_first_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"),  Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'"),
  };
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("']'"),   Symbol("'eq'"), Symbol("'neq'"),
                             Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
                             Symbol("'geq'"), Symbol("';'"),  Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestTerm) {
  Symbol symb("<term>");
  Set expected_first_set = {
      Symbol("'intNum'"), Symbol("'floatNum'"), Symbol("'('"),  Symbol("'not'"),
      Symbol("'+'"),      Symbol("'-'"),        Symbol("'id'"),
  };
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'+'"),   Symbol("'-'"),  Symbol("'or'"),
                             Symbol("']'"),   Symbol("'eq'"), Symbol("'neq'"),
                             Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
                             Symbol("'geq'"), Symbol("';'"),  Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestArithExprRightRec) {
  Symbol symb("<arithExprRightRec>");
  Set expected_first_set = {Symbol("'+'"), Symbol("'-'"), Symbol("'or'"),
                            Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("']'"),   Symbol("'eq'"), Symbol("'neq'"),
                             Symbol("'lt'"),  Symbol("'gt'"), Symbol("'leq'"),
                             Symbol("'geq'"), Symbol("';'"),  Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestArraySize) {
  Symbol symb("<arraySize>");
  Set expected_first_set = {Symbol("'['")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'['"), Symbol("';'"), Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestContinueOrEndVariableFactor) {
  Symbol symb("<continueOrEndVariableFactor>");
  Set expected_first_set = {Symbol("'.'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVariableFactor1) {
  Symbol symb("<variableFactor1>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFuncDef) {
  Symbol symb("<funcDef>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'id'"), Symbol("'main'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFuncDefRept0) {
  Symbol symb("<funcDefRept0>");
  Set expected_first_set = {Symbol("'id'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'main'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestArraySizeFactor) {
  Symbol symb("<arraySizeFactor>");
  Set expected_first_set = {Symbol("']'"), Symbol("'intNum'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'['"), Symbol("';'"), Symbol("','"),
                             Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAParamsTail) {
  Symbol symb("<aParamsTail>");
  Set expected_first_set = {Symbol("','")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("','"), Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAParamsTailRept0) {
  Symbol symb("<aParamsTailRept0>");
  Set expected_first_set = {Symbol("','"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVarDecl) {
  Symbol symb("<varDecl>");
  Set expected_first_set = {Symbol("'integer'"), Symbol("'float'"),
                            Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'integer'"), Symbol("'float'"),
                             Symbol("'id'"), Symbol("'do'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestVarDeclRept0) {
  Symbol symb("<varDeclRept0>");
  Set expected_first_set = {Symbol("'integer'"), Symbol("'float'"),
                            Symbol("'id'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'do'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFParamsTail) {
  Symbol symb("<fParamsTail>");
  Set expected_first_set = {Symbol("','")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("','"), Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFParamsTailRept0) {
  Symbol symb("<fParamsTailRept0>");
  Set expected_first_set = {Symbol("','"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAssignStatOrFunctionCall) {
  Symbol symb("<assignStatOrFunctionCall>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestAssignStatOrFunctionCallFactor1j) {
  Symbol symb("<assignStatOrFunctionCallFactor1>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestClassDecl) {
  Symbol symb("<classDecl>");
  Set expected_first_set = {Symbol("'class'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'class'"), Symbol("'id'"),
                             Symbol("'main'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestClassDeclRept0) {
  Symbol symb("<classDeclRept0>");
  Set expected_first_set = {Symbol("'class'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'id'"), Symbol("'main'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestInheritsOpt) {
  Symbol symb("<inheritsOpt>");
  Set expected_first_set = {Symbol("'inherits'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'{'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestInheritsRept0) {
  Symbol symb("<inheritsRept0>");
  Set expected_first_set = {Symbol("','"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'{'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestMemberFuncDeclOrVarDeclFactor) {
  Symbol symb("<memberFuncDeclOrVarDeclFactor>");
  Set expected_first_set = {Symbol("'('"), Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'}'"), Symbol("'public'"),
                             Symbol("'private'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestFParams) {
  Symbol symb("<fParams>");
  Set expected_first_set = {Symbol("'integer'"), Symbol("'float'"),
                            Symbol("'id'"), Symbol("EPSILON")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("')'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestTypeOrVoid) {
  Symbol symb("<typeOrVoid>");
  Set expected_first_set = {Symbol("'void'"), Symbol("'integer'"),
                            Symbol("'float'"), Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("';'"), Symbol("'do'"), Symbol("'local'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

TEST_F(ParserGenTest, TestMemberVarDeclFactor) {
  Symbol symb("<memberVarDeclFactor>");
  Set expected_first_set = {Symbol("'id'")};
  Set actual_first_set = actual_first_sets.at(symb);
  EXPECT_EQ(expected_first_set, actual_first_set);

  Set expected_follow_set = {Symbol("'}'"), Symbol("'public'"),
                             Symbol("'private'")};
  Set actual_follow_set = actual_follow_sets.at(symb);
  EXPECT_EQ(expected_follow_set, actual_follow_set);
}

}  // namespace pgentest