#include "FixturePaths.h"

#include "mull/AST/ASTVisitor.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"

#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/Tooling.h>

#include <gtest/gtest.h>

using namespace mull;
using namespace llvm;

static const char *const fakeSourceFilePath = "input.cc";

static FilePathFilter nullPathFilter;

TEST(ASTVisitor_Negate, testNegatedUnaryToUnary) {
  const char *const binaryOperator = R"(///
bool isFalse(bool a) {
  return !a;
};
)";

  Diagnostics diagnostics;
  diagnostics.enableDebugMode();

  ASTStorage storage(diagnostics, "", "", {});

  std::unique_ptr<clang::ASTUnit> astUnit(
      clang::tooling::buildASTFromCode(binaryOperator, fakeSourceFilePath));
  assert(astUnit);

  SingleASTUnitMutations singleUnitMutations;

  MutatorKindSet mutatorKindSet = MutatorKindSet::create({ MutatorKind::NegateMutator });

  ThreadSafeASTUnit threadSafeAstUnit(std::move(astUnit));
  ASTVisitor astVisitor(
      diagnostics, threadSafeAstUnit, singleUnitMutations, nullPathFilter, mutatorKindSet);

  astVisitor.traverse();

  LineColumnHash locationHash = lineColumnHash(3, 10);

  ASSERT_EQ(singleUnitMutations.size(), 1U);
  ASSERT_EQ(singleUnitMutations.count(MutatorKind::NegateMutator), 1U);

  ASSERT_EQ(singleUnitMutations[MutatorKind::NegateMutator].size(), 1U);
  ASSERT_EQ(singleUnitMutations[MutatorKind::NegateMutator].count(locationHash), 1U);
}

TEST(ASTVisitor_Negate, testUnaryToNegatedUnary) {
  const char *const binaryOperator = R"(///
bool isTrue(bool a) {
  return a;
};
)";

  Diagnostics diagnostics;
  diagnostics.enableDebugMode();

  ASTStorage storage(diagnostics, "", "", {});

  std::unique_ptr<clang::ASTUnit> astUnit(
    clang::tooling::buildASTFromCode(binaryOperator, fakeSourceFilePath));
  assert(astUnit);

  SingleASTUnitMutations singleUnitMutations;

  MutatorKindSet mutatorKindSet = MutatorKindSet::create({ MutatorKind::NegateMutator });

  ThreadSafeASTUnit threadSafeAstUnit(std::move(astUnit));
  ASTVisitor astVisitor(
    diagnostics, threadSafeAstUnit, singleUnitMutations, nullPathFilter, mutatorKindSet);

  astVisitor.traverse();

  LineColumnHash locationHash = lineColumnHash(3, 10);

  ASSERT_EQ(singleUnitMutations.size(), 1U);
  ASSERT_EQ(singleUnitMutations.count(MutatorKind::NegateMutator), 1U);

  ASSERT_EQ(singleUnitMutations[MutatorKind::NegateMutator].size(), 1U);
  ASSERT_EQ(singleUnitMutations[MutatorKind::NegateMutator].count(locationHash), 1U);
}
