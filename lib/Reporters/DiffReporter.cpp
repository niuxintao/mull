#include "mull/Reporters/DiffReporter.h"
#include "mull/Diagnostics/Diagnostics.h"
#include "mull/Reporters/ASTSourceInfoProvider.h"
#include "mull/Result.h"
#include <clang/AST/Expr.h>
#include <clang/Lex/Lexer.h>
#include <git2.h>
#include <mull/Reporters/SourceManager.h>
#include <set>
#include <sstream>

using namespace mull;

static int diff_callback(const git_diff_delta *delta, const git_diff_hunk *hunk,
                         const git_diff_line *line, void *payload) {
  FILE *fp = stdout;

  if (line->origin == GIT_DIFF_LINE_FILE_HDR) {
    fwrite("--- ", 1, strlen("--- "), fp);
    fwrite("a/", 1, strlen("a/"), fp);
    fwrite(delta->old_file.path, 1, strlen(delta->old_file.path), fp);
    fwrite("\n", 1, strlen("\n"), fp);

    fwrite("--- ", 1, strlen("--- "), fp);
    fwrite("b/", 1, strlen("b/"), fp);
    fwrite(delta->new_file.path, 1, strlen(delta->new_file.path), fp);
    fwrite("\n", 1, strlen("\n"), fp);
    return 0;
  }

  if (line->origin == GIT_DIFF_LINE_HUNK_HDR) {
    //    std::string hunkContext();
    std::stringstream headerStream;
    headerStream << "@@ -" << hunk->old_start << "," << hunk->old_lines << " +" << hunk->new_start
                 << "," << hunk->new_lines << " @@\n";
    std::string header = headerStream.str();
    fwrite(header.c_str(), 1, header.size(), fp);
    fwrite(hunk->header, 1, hunk->header_len, fp);
    //    printf("%d %d %d %d %zu %s\n", hunk->old_start, hunk->old_lines, hunk->new_start,
    //    hunk->new_lines, hunk->header_len, hunk->header);
    // @@ -4,4 +4,4 @@ num1
    //    printf("--->>>> %s %c %d %d\n", line->content, line->origin, line->old_lineno,
    //    line->new_lineno);
    return 0;
  }

  if (line->origin != GIT_DIFF_LINE_CONTEXT && line->origin != GIT_DIFF_LINE_ADDITION &&
      line->origin != GIT_DIFF_LINE_DELETION) {
    return 0;
  }

  fputc(line->origin, fp);
  fwrite(line->content, 1, line->content_len, fp);
  return 0;
}

DiffReporter::DiffReporter(Diagnostics &diagnostics, SourceInfoProvider &sourceInfoProvider)
    : diagnostics(diagnostics), sourceInfoProvider(sourceInfoProvider) {
  this->diagnostics.info("hello");
  git_libgit2_init();
}

DiffReporter::~DiffReporter() {
  git_libgit2_shutdown();
}

static bool mutantSurvived(const ExecutionStatus &status) {
  return status == ExecutionStatus::Passed;
}

static std::vector<SourceLocation> locationsBefore(SourceManager &sourceManager,
                                                   const SourceLocation &mutantLocation) {
  std::vector<SourceLocation> locations;
  size_t before = std::max(1, mutantLocation.line - 3);
  for (int i = before; i < mutantLocation.line; i++) {
    locations.emplace_back(mutantLocation.unitDirectory,
                           mutantLocation.unitFilePath,
                           mutantLocation.directory,
                           mutantLocation.filePath,
                           mutantLocation.line - i,
                           mutantLocation.column);
  }
  return locations;
}

static std::vector<SourceLocation> locationsAfter(SourceManager &sourceManager,
                                                  const SourceLocation &mutantLocation) {
  std::vector<SourceLocation> locations;
  size_t before =
      std::min(sourceManager.getNumberOfLines(mutantLocation), size_t(mutantLocation.line + 3));
  for (int i = before; i < mutantLocation.line; i++) {
    locations.emplace_back(mutantLocation.unitDirectory,
                           mutantLocation.unitFilePath,
                           mutantLocation.directory,
                           mutantLocation.filePath,
                           mutantLocation.line + i,
                           mutantLocation.column);
  }
  return locations;
}

void DiffReporter::reportResults(const Result &result, const Metrics &metrics) {
  SourceManager sourceManager;
  for (auto &mutationResult : result.getMutationResults()) {
    auto mutant = mutationResult->getMutationPoint();
    auto &executionResult = mutationResult->getExecutionResult();

    if (!mutantSurvived(executionResult.status)) {
      continue;
    }
    fprintf(stdout, "Survived\n");
    fflush(stdout);
    const SourceLocation &sourceLocation = mutant->getSourceLocation();
    if (sourceLocation.isNull()) {
      continue;
    }
    std::stringstream beforeStream;
    for (SourceLocation &location : locationsBefore(sourceManager, sourceLocation)) {
      beforeStream << sourceManager.getLine(location);
    }
    std::stringstream afterStream;
    for (SourceLocation &location : locationsAfter(sourceManager, sourceLocation)) {
      afterStream << sourceManager.getLine(location);
    }
    std::string mutation = sourceManager.getLine(sourceLocation);
    std::stringstream oldStream;
    oldStream << beforeStream.str() << mutation << afterStream.str();
    mutation[sourceLocation.column] = '-';
    std::stringstream newSteram;
    newSteram << beforeStream.str() << mutation << afterStream.str();

    std::string oldContent = oldStream.str();
    std::string newContent = newSteram.str();

    sourceInfoProvider.getSourceInfo(diagnostics, mutant);

//    const clang::BinaryOperator *const binop = llvm::dyn_cast<clang::BinaryOperator>(stmt);
//    binop->getOperatorLoc().

//    auto info = sourceInfoProvider.getSourceInfo(diagnostics, mutant);
//    printf("%d %d %d %d\n", info.beginLine, info.beginColumn, info.endLine, info.endColumn);

    git_patch *patch;
    git_patch_from_buffers(&patch,
                           oldContent.c_str(),
                           oldContent.size(),
                           sourceLocation.unitFilePath.c_str(),
                           newContent.c_str(),
                           newContent.size(),
                           sourceLocation.unitFilePath.c_str(),
                           nullptr);

    git_patch_print(patch, diff_callback, nullptr);

    git_patch_free(patch);
  }
}
