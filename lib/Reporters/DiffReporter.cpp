#include "mull/Reporters/DiffReporter.h"
#include "mull/Diagnostics/Diagnostics.h"
#include "mull/Result.h"
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
    headerStream << "@@ -" << hunk->old_start << "," << hunk->old_lines << " +" << hunk->new_start << "," << hunk->new_lines << " @@\n";
    std::string header = headerStream.str();
    fwrite(header.c_str(), 1, header.size(), fp);
    fwrite(hunk->header, 1, hunk->header_len, fp);
//    printf("%d %d %d %d %zu %s\n", hunk->old_start, hunk->old_lines, hunk->new_start, hunk->new_lines, hunk->header_len, hunk->header);
    // @@ -4,4 +4,4 @@ num1
//    printf("--->>>> %s %c %d %d\n", line->content, line->origin, line->old_lineno, line->new_lineno);
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

DiffReporter::DiffReporter(Diagnostics &diagnostics) : diagnostics(diagnostics) {
  this->diagnostics.info("hello");
  git_libgit2_init();
}

DiffReporter::~DiffReporter() {
  git_libgit2_shutdown();
}

static bool mutantSurvived(const ExecutionStatus &status) {
  return status == ExecutionStatus::Passed;
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
    git_patch *patch;
    const char *old = "num1\nnum2\nnum3\nnum4\nnum5\nnum6\nnum7";
    const char *new_ = "num1\nnum2\nnum3\nnum4\nnum5\nnum6\nnew7";
    git_patch_from_buffers(
        &patch, old, strlen(old), "hello.c", new_, strlen(new_), "hello.c", nullptr);

    git_patch_print(patch, diff_callback, nullptr);

    git_patch_free(patch);
  }
}
