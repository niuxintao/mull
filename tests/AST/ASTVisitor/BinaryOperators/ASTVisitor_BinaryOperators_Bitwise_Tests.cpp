#include "FixturePaths.h"

#include "mull/AST/ASTVisitor.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"

#include <clang/Tooling/Tooling.h>

#include <gtest/gtest.h>

using namespace mull;
using namespace llvm;

static const char *const fakeSourceFilePath = "input.cc";

static FilePathFilter nullPathFilter;

TEST(ASTVisitorTest, bitwise_leftShiftToRightShift) {
  const char *const binaryOperator = R"(///
int add(int a, int b) {
  return a << b;
}
)";

  mull::TraverseMask traverseMask(
      {mull::MutatorKind::CXX_Bitwise_LShiftToRShift});

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
      fakeSourceFilePath, mull::MutatorKind::CXX_Bitwise_LShiftToRShift, 3,
      12));
}
