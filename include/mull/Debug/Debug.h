#pragma once

namespace clang {
class ASTContext;
class Stmt;
} // namespace clang

namespace llvm {
class Function;
class Instruction;
} // namespace llvm

namespace mull {

void dump(const clang::Stmt &statement, clang::ASTContext &context,
          bool parent = false);
void dump(const llvm::Function &function);
void dump(const llvm::Instruction &instruction);

} // namespace mull
