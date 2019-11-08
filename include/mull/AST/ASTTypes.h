#pragma once

#include <mull/Mutators/MutatorKind.h>

#include <clang/AST/Stmt.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mull {

struct MutationPointSourceInfo {
  int beginLine = -1;
  int beginColumn = -1;
  int endLine = -1;
  int endColumn = -1;
};

using MutationLocationHash = int;

using SourceFilePath = std::string;

enum ASTNodeType {
  UNKNOWN = 0,

  BINARY_OPERATOR = 1,
  UNARY_OPERATOR,

  RETURN_STATEMENT,

  CALL_EXPRESSION,

  VARIABLE_DECLARATION,

  ARRAY_SUBSCRIPT_EXPRESSION,

  CXX_MEMBER_CALL_EXPRESSION,
  CXX_CONSTRUCTOR_DECLARATION,
  CXX_TEMPORARY_OBJECT_EXPRESSION
};

struct TraverseMask {
  TraverseMask(std::unordered_set<mull::MutatorKind> mutators)
      : mutators(mutators) {}

  static TraverseMask create(std::vector<MutatorKind> mutators);

  bool includesMutator(mull::MutatorKind mutatorKind) const;
private:
  std::unordered_set<mull::MutatorKind> mutators;
};

struct ASTMutation {
  mull::MutatorKind mutatorKind;
  int line;
  int column;
  const clang::Stmt *const stmt;
  ASTMutation(MutatorKind mutatorKind, int line, int column,
              const clang::Stmt *const stmt)
      : mutatorKind(mutatorKind), line(line), column(column), stmt(stmt) {}
};

using OneMutationTypeBucket =
    std::unordered_map<MutationLocationHash, ASTMutation>;

using SingleASTUnitMutations =
    std::unordered_map<mull::MutatorKind, OneMutationTypeBucket>;

} // namespace mull
