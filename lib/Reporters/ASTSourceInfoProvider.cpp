#include "mull/Reporters/ASTSourceInfoProvider.h"

#include "mull/AST/ASTMutationStorage.h"
#include "mull/Diagnostics/Diagnostics.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"
#include "mull/MutationPoint.h"

#include <clang/AST/Expr.h>
#include <clang/Lex/Lexer.h>

using namespace mull;

ASTSourceInfoProvider::ASTSourceInfoProvider(ASTStorage &astStorage) : astStorage(astStorage) {}

MutationPointSourceInfo ASTSourceInfoProvider::getSourceInfo(Diagnostics &diagnostics,
                                                             MutationPoint *mutationPoint) {
  MutationPointSourceInfo info = MutationPointSourceInfo();
  clang::SourceRange sourceRange;

  const clang::Stmt *const mutantASTNode = getMutationStatement(diagnostics, mutationPoint);

  ThreadSafeASTUnit *astUnit = astStorage.findAST(mutationPoint);

  if (mutantASTNode == nullptr) {
    diagnostics.warning("Cannot find an AST node for mutation point");
    return info;
  }
  if (astUnit == nullptr) {
    diagnostics.warning("Cannot find an AST unit for mutation point");
    return info;
  }

  sourceRange = mutantASTNode->getSourceRange();

  clang::ASTContext &astContext = astUnit->getASTContext();

  clang::SourceManager &sourceManager = astUnit->getSourceManager();

  clang::SourceLocation sourceLocationBegin = sourceRange.getBegin();
  clang::SourceLocation sourceLocationEnd = sourceRange.getEnd();

  /// Clang AST: how to get more precise debug information in certain cases?
  /// http://clang-developers.42468.n3.nabble.com/Clang-AST-how-to-get-more-precise-debug-information-in-certain-cases-td4065195.html
  /// https://stackoverflow.com/questions/11083066/getting-the-source-behind-clangs-ast
  clang::SourceLocation sourceLocationEndActual = clang::Lexer::getLocForEndOfToken(
      sourceLocationEnd, 0, sourceManager, astContext.getLangOpts());

  info.beginColumn = sourceManager.getExpansionColumnNumber(sourceLocationBegin);
  info.beginLine = sourceManager.getExpansionLineNumber(sourceLocationBegin, nullptr);
  info.endColumn = sourceManager.getExpansionColumnNumber(sourceLocationEndActual);
  info.endLine = sourceManager.getExpansionLineNumber(sourceLocationEndActual, nullptr);

  const clang::BinaryOperator *const binop = llvm::dyn_cast<clang::BinaryOperator>(mutantASTNode);

  clang::SourceLocation binopEndActual = clang::Lexer::getLocForEndOfToken(
      binop->getOperatorLoc(), 0, sourceManager, astContext.getLangOpts());

  printf("%d %d %d %d\n",
         sourceManager.getExpansionColumnNumber(binop->getOperatorLoc()),
         sourceManager.getExpansionLineNumber(binop->getOperatorLoc(), nullptr),
         sourceManager.getExpansionColumnNumber(binopEndActual),
         sourceManager.getExpansionLineNumber(binopEndActual, nullptr));

  return info;
}

const clang::Stmt *ASTSourceInfoProvider::getMutationStatement(Diagnostics &diagnostics,
                                                               MutationPoint *mutationPoint) {
  const SourceLocation &sourceLocation = mutationPoint->getSourceLocation();
  const std::string &sourceFile = sourceLocation.unitFilePath;

  const ASTMutation &astMutation =
      astStorage.getMutation(sourceFile,
                             mutationPoint->getMutator()->mutatorKind(),
                             sourceLocation.line,
                             sourceLocation.column);

  const clang::Stmt *const mutantASTNode = astMutation.stmt;
  return mutantASTNode;
}
