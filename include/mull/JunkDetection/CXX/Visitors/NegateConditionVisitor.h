#pragma once

#include "InstructionRangeVisitor.h"
#include "VisitorParameters.h"

#include <clang/AST/RecursiveASTVisitor.h>

namespace mull {

class NegateConditionVisitor
    : public clang::RecursiveASTVisitor<NegateConditionVisitor> {
public:
  NegateConditionVisitor(const VisitorParameters &parameters);

  bool VisitExpr(clang::Expr *expr);
  bool VisitUnaryOperator(clang::UnaryOperator *unaryOperator);

  clang::Expr *foundMutant();

private:
  InstructionRangeVisitor visitor;
};

} // namespace mull