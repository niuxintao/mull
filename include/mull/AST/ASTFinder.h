#pragma once

#include "mull/AST/ASTMutations.h"

namespace mull {

class Configuration;
class Program;
class ASTStorage;
class FilePathFilter;

class ASTFinder {
public:
  mull::ASTMutations findMutations(const mull::Configuration &config,
                                   mull::Program &program,
                                   mull::FilePathFilter &pathFilter,
                                   mull::ASTStorage &storage,
                                   mull::TraverseMask traverseMask);
};
} // namespace mull
