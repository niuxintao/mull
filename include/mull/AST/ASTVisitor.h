#pragma once

#include "mull/Diagnostics/Diagnostics.h"
#include "mull/Filters/FilePathFilter.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"

#include <clang/AST/RecursiveASTVisitor.h>

#include <iostream>

namespace mull {

class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor> {
  mull::Diagnostics &diagnostics;
  mull::ThreadSafeASTUnit &astUnit;
  mull::SingleASTUnitMutations &singleUnitMutations;
  clang::SourceManager &sourceManager;
  mull::FilePathFilter &filePathFilter;
  mull::MutatorKindSet mutatorKindSet;
  bool shouldSkipCurrentFunction;
  std::unordered_map<MutatorKind, clang::Expr *> skipExpressions;
public:
  explicit ASTVisitor(mull::Diagnostics &diagnostics, mull::ThreadSafeASTUnit &astUnit,
                      mull::SingleASTUnitMutations &singleUnitMutations,
                      mull::FilePathFilter &filePathFilter, mull::MutatorKindSet mutatorKindSet);

  bool VisitFunctionDecl(clang::FunctionDecl *Decl);
  bool VisitExpr(clang::Expr *expr);

  void traverse();

private:
  void saveMutationPoint(mull::MutatorKind mutatorKind, const clang::Stmt *stmt,
                         clang::SourceLocation location);
};

} // namespace mull
