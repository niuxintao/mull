#pragma once

#include "mull/AST/ASTMutations.h"
#include "mull/Filters/FilePathFilter.h"
#include "mull/Filters/InstructionFilter.h"

namespace mull {

class ASTInformation {
public:
  virtual bool validMutation(llvm::Instruction &instruction,
                             mull::MutatorKind mutatorKind) const = 0;
  virtual ~ASTInformation() = default;
};

class NoASTInformation: public ASTInformation {
  bool validMutation(llvm::Instruction &instruction,
                     mull::MutatorKind mutatorKind) const override {
    return true;
  }
};

class RealASTInformation : public ASTInformation {
public:
  explicit RealASTInformation(const ASTMutations &astMutations,
                              const FilePathFilter &filePathFilter);
  ~RealASTInformation() = default;

  std::string name();

  bool validMutation(llvm::Instruction &instruction,
                     mull::MutatorKind mutatorKind) const override;

private:
  const ASTMutations &astMutations;
  const FilePathFilter &filePathFilter;
};
} // namespace mull
