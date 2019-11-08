#include "mull/AST/ASTInformation.h"

#include "mull/Debug/Debug.h"
#include "mull/SourceLocation.h"

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Module.h>

#include <iostream>
#include <llvm/IR/Instructions.h>
#include <unordered_map>

using namespace mull;

RealASTInformation::RealASTInformation(const ASTMutations &astMutations,
                                       const FilePathFilter &filePathFilter)
    : astMutations(astMutations), filePathFilter(filePathFilter) {}

std::string RealASTInformation::name() { return "ASTInformation"; }

bool RealASTInformation::validMutation(llvm::Instruction &instruction,
                                       mull::MutatorKind mutatorKind) const {
  const llvm::DebugLoc &debugInfo = instruction.getDebugLoc();
  if (!debugInfo) {
    return false;
  }

  if (filePathFilter.shouldSkip(instruction.getModule())) {
    return false;
  }

  int line = debugInfo.getLine();
  int column = debugInfo.getCol();

  if (line <= 0 || column <= 0) {
    assert(0 && "Unknown edge case.");
  }

  mull::SourceLocation sourceLocation =
      SourceLocation::locationFromInstruction(&instruction);

  const std::string fullSourcePath = sourceLocation.filePath;
  assert(llvm::sys::fs::is_regular_file(fullSourcePath));

  if (filePathFilter.shouldSkip(fullSourcePath)) {
    return false;
  }

  /// TODO: we might want to reconsider this because some mutations can be
  /// part of this instruction.
  if (llvm::dyn_cast<llvm::GetElementPtrInst>(&instruction)) {
    return false;
  }

  // TODO: For now we are not aware of the valid AST mutations that result in
  // the following instructions.
  if (llvm::dyn_cast<llvm::SExtInst>(&instruction) ||
      llvm::dyn_cast<llvm::LoadInst>(&instruction)) {
    return false;
  }

  bool locationExists =
      astMutations.locationExists(fullSourcePath, mutatorKind, line, column);

  if (locationExists) {
    //    llvm::errs() << "\n";
    //    instruction.print(llvm::errs(), true);
    //    llvm::errs() << "\nline/col: " << line << " " << column << "\n";
    //    llvm::errs() << "\n";
  }

  return locationExists;
}
