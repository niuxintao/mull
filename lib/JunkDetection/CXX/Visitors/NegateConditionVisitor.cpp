#include "mull/JunkDetection/CXX/Visitors/NegateConditionVisitor.h"

using namespace mull;

NegateConditionVisitor::NegateConditionVisitor(
    const VisitorParameters &parameters)
    : visitor(parameters) {}

bool NegateConditionVisitor::VisitUnaryOperator(
    clang::UnaryOperator *unaryOperator) {
  if (unaryOperator->getOpcode() == clang::UnaryOperatorKind::UO_LNot) {
    visitor.visitRangeWithASTExpr(unaryOperator);
  }
  return true;
}

bool NegateConditionVisitor::VisitExpr(clang::Expr *expr) {
  if (clang::DeclRefExpr *declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(expr)) {
    visitor.visitRangeWithASTExpr(declRefExpr);
  }
  return true;
}

clang::Expr *NegateConditionVisitor::foundMutant() {
  return visitor.getMatchingASTNode();
}
