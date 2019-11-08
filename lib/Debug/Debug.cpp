#include "mull/Debug/Debug.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Expr.h>

#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Module.h>

namespace mull {

void dump(const clang::Stmt &statement, clang::ASTContext &context,
          bool parent) {
  statement.dump(llvm::outs());

  if (parent) {
    for (auto &parent : context.getParents(statement)) {
      parent.dump(llvm::outs(), context.getSourceManager());
    }
  }
}

void dump(const llvm::Function &function) {
  function.print(llvm::outs(), nullptr, true);
  llvm::outs() << "\n";
}

void dump(const llvm::Instruction &instruction) {

  instruction.print(llvm::outs(), true);
  llvm::outs() << "\n";

  const llvm::DebugLoc &debugInfo = instruction.getDebugLoc();
  if (!debugInfo) {
    assert(0);
    return;
  }

  std::string moduleFileName = instruction.getModule()->getSourceFileName();

  int line = debugInfo.getLine();
  int column = debugInfo.getCol();

  llvm::outs() << moduleFileName << ":" << line << ":" << column << "\n";
}

} // namespace mull