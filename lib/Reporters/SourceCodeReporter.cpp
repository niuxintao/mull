#include "mull/Reporters/SourceCodeReporter.h"
#include "mull/Result.h"
#include "mull/JunkDetection/CXX/ASTStorage.h"
#include <clang/Rewrite/Core/Rewriter.h>

using namespace mull;

SourceCodeReporter::SourceCodeReporter(ASTStorage &astStorage)
    : astStorage(astStorage) {}

void SourceCodeReporter::reportResults(const Result &result,
                                       const RawConfig &config,
                                       const Metrics &metrics) {
  for (auto point : result.getMutationPoints()) {
    auto ast = astStorage.findAST(point);
    auto expr = astStorage.getMutantASTNode(point);
    auto &sourceManager = ast->getSourceManager();
    auto fileID = sourceManager.getFileID(expr->getBeginLoc());

    clang::LangOptions langOptions;
    clang::Rewriter rewriter(sourceManager, langOptions);

    rewriter.ReplaceText(expr->getSourceRange(), "AAAAAAAA");

    auto buffer = rewriter.getRewriteBufferFor(fileID);
    buffer->write(llvm::errs());
  }
}
