#include "mull/AST/ASTMutations.h"

#include <llvm/Support/FileSystem.h>

#include <iostream>

using namespace mull;

ASTMutations::ASTMutations() : storage() {}

int ASTMutations::count() const {
  int count = 0;
  for (const std::pair<SourceFilePath, SingleASTUnitMutations>
           &singleUnitMutations : storage) {
    for (const std::pair<MutatorKind, OneMutationTypeBucket>
             &oneMutationBucket : singleUnitMutations.second) {
      /// std::cout << "ASTMutations.count() one ast unit: " << pair.first <<
      /// "\n";
      count += singleUnitMutations.second.at(oneMutationBucket.first).size();
    }
  }
  return count;
};

bool ASTMutations::locationExists(SourceFilePath sourceFile,
                                  MutatorKind mutatorKind, int line,
                                  int column) const {
  assert(llvm::sys::fs::is_regular_file(sourceFile) ||
         sourceFile == "input.cc");

  int hash = mull::locationHash(line, column);

  if (storage.count(sourceFile) == 0) {
    return false;
  }

  const SingleASTUnitMutations &singleAstUnitMutationsCopy =
      storage.at(sourceFile);
  if (singleAstUnitMutationsCopy.count(mutatorKind) == 0) {
    return false;
  }

  OneMutationTypeBucket mutationTypeBucket =
      singleAstUnitMutationsCopy.at(mutatorKind);
  return (mutationTypeBucket.count(hash) != 0);
}

const ASTMutation &ASTMutations::getMutation(const std::string &sourceFile,
                                             MutatorKind mutatorKind,
                                             int line, int column) const {
  assert(llvm::sys::fs::is_regular_file(sourceFile) ||
         sourceFile == "input.cc");

  assert(storage.count(sourceFile) > 0);

  const SingleASTUnitMutations &astUnitMutations = storage.at(sourceFile);
  assert(astUnitMutations.count(mutatorKind) > 0);

  const OneMutationTypeBucket &oneMutationBucket =
      astUnitMutations.at(mutatorKind);

  MutationLocationHash hash = locationHash(line, column);
  assert(oneMutationBucket.count(hash) > 0);

  const ASTMutation &mutation = oneMutationBucket.at(hash);
  return mutation;
}

void ASTMutations::dump() const {
  std::cout << "ASTMutations::dump()\n";

  for (const std::pair<SourceFilePath, SingleASTUnitMutations>
           &sourceFileMutations : storage) {
    std::cout << "file: " << sourceFileMutations.first << "\n";

    for (const std::pair<MutatorKind, OneMutationTypeBucket>
             &oneTypeMutations : sourceFileMutations.second) {
      std::cout << "\tmutation type: "
                << MutationKindToString(oneTypeMutations.first) << "\n";

      for (auto &info : oneTypeMutations.second) {

        std::cout << "\t\tline/col: " << info.second.line << " "
                  << info.second.column << "\n";
      }
    }
  }
}

void ASTMutations::saveExpr(std::string &sourceFile,
                            mull::MutatorKind mutatorKind,
                            ASTNodeType nodeType,
                            const clang::Stmt *const expression, int line,
                            int column) {
  assert(llvm::sys::fs::is_regular_file(sourceFile) ||
         sourceFile == "input.cc");

  int hash = mull::locationHash(line, column);

  if (storage.count(sourceFile) == 0) {
    storage.emplace(sourceFile, SingleASTUnitMutations());
  }

  if (storage[sourceFile].count(mutatorKind) == 0) {
    storage[sourceFile].emplace(mutatorKind, OneMutationTypeBucket());
  }

  storage[sourceFile][mutatorKind].emplace(
      hash, ASTMutation(mutatorKind, line, column, expression));

  assert(count() > 0 && "expected something to be set");
}
