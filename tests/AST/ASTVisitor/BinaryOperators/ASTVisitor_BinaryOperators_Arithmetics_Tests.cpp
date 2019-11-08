#include "FixturePaths.h"

#include "mull/AST/ASTVisitor.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"

#include <clang/Tooling/Tooling.h>

#include <gtest/gtest.h>

using namespace mull;
using namespace llvm;

static const char *const fakeSourceFilePath = "input.cc";
static FilePathFilter nullPathFilter;

TEST(ASTVisitorTest, binaryAddOperator) {
  const char *const binaryOperator = R"(///
int add(int a, int b) {
  return a + b;
}
)";

  mull::TraverseMask traverseMask({mull::MutatorKind::CXX_Arithmetic_AddToSub});

  ASTStorage storage("", "");

  std::unique_ptr<clang::ASTUnit> astUnit(
      clang::tooling::buildASTFromCode(binaryOperator, fakeSourceFilePath));

  ASTMutations astMutations;

  ThreadSafeASTUnit threadSafeAstUnit(std::move(astUnit));
  ASTVisitor astVisitor(threadSafeAstUnit, astMutations, nullPathFilter,
                        traverseMask);

  astVisitor.traverse();

  ASSERT_EQ(astMutations.count(), 1U);
  ASSERT_TRUE(astMutations.locationExists(
      fakeSourceFilePath, mull::MutatorKind::CXX_Arithmetic_AddToSub, 3, 12));
}

TEST(ASTVisitorTest, binarySubOperator) {
  const char *const binaryOperator = R"(///
int add(int a, int b) {
  return a - b;
}
)";

  mull::TraverseMask traverseMask({mull::MutatorKind::CXX_Arithmetic_SubToAdd});

  ASTStorage storage("", "");

  std::unique_ptr<clang::ASTUnit> astUnit(
      clang::tooling::buildASTFromCode(binaryOperator, fakeSourceFilePath));

  ASTMutations astMutations;

  ThreadSafeASTUnit threadSafeAstUnit(std::move(astUnit));
  ASTVisitor astVisitor(threadSafeAstUnit, astMutations, nullPathFilter,
                        traverseMask);

  astVisitor.traverse();

  clang::SourceManager &sourceManager = threadSafeAstUnit.getSourceManager();

  const ASTMutation &mutation = astMutations.getMutation(
      fakeSourceFilePath, MutatorKind::CXX_Arithmetic_SubToAdd, 3, 12);
  clang::SourceLocation begin = mutation.stmt->getSourceRange().getBegin();

  int clangLine = sourceManager.getExpansionLineNumber(begin);
  int clangColumn = sourceManager.getExpansionColumnNumber(begin);

  ASSERT_EQ(astMutations.count(), 1U);
  ASSERT_TRUE(astMutations.locationExists(
      fakeSourceFilePath, MutatorKind::CXX_Arithmetic_SubToAdd, 3, 12));
  ASSERT_EQ(clangLine, 3U);
  ASSERT_EQ(clangColumn, 10U);
  ASSERT_EQ(mutation.line, 3U);
  ASSERT_EQ(mutation.column, 12U);
}
