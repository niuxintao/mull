#include "mull/AST/ASTVisitor.h"

#include "mull/Debug/Debug.h"
#include "mull/Program/Program.h"

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceLocation.h>
#include <llvm/IR/Module.h>

#include <cassert>

using namespace mull;

static std::vector<std::pair<clang::BinaryOperator::Opcode, mull::MutatorKind>>
    BINARY_MUTATIONS = {
        /// CXX Arithmetic

        {clang::BO_Add, mull::MutatorKind::CXX_Arithmetic_AddToSub},
        {clang::BO_Sub, mull::MutatorKind::CXX_Arithmetic_SubToAdd},
        {clang::BO_Mul, mull::MutatorKind::CXX_Arithmetic_MulToDiv},
        {clang::BO_Rem, mull::MutatorKind::CXX_Arithmetic_RemToDiv},
        {clang::BO_Div, mull::MutatorKind::CXX_Arithmetic_DivToMul},

        {clang::BO_AddAssign,
         mull::MutatorKind::CXX_Arithmetic_AddAssignToSubAssign},
        {clang::BO_SubAssign,
         mull::MutatorKind::CXX_Arithmetic_SubAssignToAddAssign},
        {clang::BO_MulAssign,
         mull::MutatorKind::CXX_Arithmetic_MulAssignToDivAssign},
        {clang::BO_RemAssign,
         mull::MutatorKind::CXX_Arithmetic_RemAssignToDivAssign},
        {clang::BO_DivAssign,
         mull::MutatorKind::CXX_Arithmetic_DivAssignToMulAssign},

        /// Relational

        {clang::BO_EQ, mull::MutatorKind::CXX_Relation_EqualToNotEqual},
        {clang::BO_NE, mull::MutatorKind::CXX_Relation_NotEqualToEqual},

        {clang::BO_GT,
         mull::MutatorKind::CXX_Relation_GreaterThanToLessOrEqual},
        {clang::BO_LT,
         mull::MutatorKind::CXX_Relation_LessThanToGreaterOrEqual},

        /// Bitwise

        {clang::BO_Shl, mull::MutatorKind::CXX_Bitwise_LShiftToRShift},
        {clang::BO_Shr, mull::MutatorKind::CXX_Bitwise_RShiftToLShift},

        /// AND-OR

        {clang::BO_LAnd, mull::MutatorKind::AndOrReplacementMutator},
        {clang::BO_LOr, mull::MutatorKind::AndOrReplacementMutator},
};

static bool isConstant(clang::Stmt *statement) {
  statement = statement->IgnoreImplicit();
  if (clang::isa<clang::IntegerLiteral>(statement)) {
    return true;
  }
  if (clang::isa<clang::FloatingLiteral>(statement)) {
    return true;
  }

  if (clang::isa<clang::CharacterLiteral>(statement)) {
    return true;
  }
  if (const clang::DeclRefExpr *declRefExpr =
          clang::dyn_cast<clang::DeclRefExpr>(statement)) {
    const clang::Decl *referencedDecl =
        declRefExpr->getReferencedDeclOfCallee();
    if (clang::isa<clang::EnumConstantDecl>(referencedDecl)) {
      return true;
    }
  }
  return false;
}

static bool findPotentialMutableParentStmt(
    const clang::Stmt *statement, clang::ASTContext &astContext,
    clang::SourceManager &sourceManager,
    clang::SourceLocation *mutationLocation, ASTNodeType *astNodeType) {
  assert(mutationLocation);

  for (auto &parent : astContext.getParents(*statement)) {
    //    llvm::errs() << "\n";
    //    statement->dump(llvm::errs(), sourceManager);
    //    parent.dump(llvm::errs(), sourceManager);
    if (const clang::BinaryOperator *binaryOpParent =
            parent.get<clang::BinaryOperator>()) {
      *mutationLocation = binaryOpParent->getOperatorLoc();
      *astNodeType = ASTNodeType::BINARY_OPERATOR;
      return true;
    }

    if (const clang::ReturnStmt *returnParent =
            parent.get<clang::ReturnStmt>()) {
      *mutationLocation = returnParent->getBeginLoc();
      *astNodeType = ASTNodeType::RETURN_STATEMENT;
      return true;
    }

    if (const clang::CXXMemberCallExpr *callExpr =
            parent.get<clang::CXXMemberCallExpr>()) {
      *mutationLocation = callExpr->getExprLoc();
      *astNodeType = ASTNodeType::CXX_MEMBER_CALL_EXPRESSION;
      return true;
    }

    if (const clang::CallExpr *callExpr = parent.get<clang::CallExpr>()) {
      *mutationLocation = callExpr->getBeginLoc();
      *astNodeType = ASTNodeType::CALL_EXPRESSION;
      return true;
    }

    if (const clang::ImplicitCastExpr *castExpr =
            parent.get<clang::ImplicitCastExpr>()) {
      return findPotentialMutableParentStmt(castExpr, astContext, sourceManager,
                                            mutationLocation, astNodeType);
    }

    if (const clang::CStyleCastExpr *castExpr =
            parent.get<clang::CStyleCastExpr>()) {
      return findPotentialMutableParentStmt(castExpr, astContext, sourceManager,
                                            mutationLocation, astNodeType);
    }

    // TODO: Not implemented
    if (const clang::ConstantExpr *constantExpr =
            parent.get<clang::ConstantExpr>()) {
      return false;
    }

    if (const clang::CXXConstructorDecl *cxxConstructorDecl =
            parent.get<clang::CXXConstructorDecl>()) {

      for (auto &member : cxxConstructorDecl->inits()) {
        if (member->getInit() == statement) {
          *mutationLocation = member->getMemberLocation();
          *astNodeType = ASTNodeType::CXX_CONSTRUCTOR_DECLARATION;
          return true;
        }
      }
    }

    if (const clang::VarDecl *varDecl = parent.get<clang::VarDecl>()) {
      *mutationLocation = varDecl->getLocation();
      *astNodeType = ASTNodeType::VARIABLE_DECLARATION;
      return true;
    }

    if (const clang::CXXTemporaryObjectExpr *cxxTemporaryObjectExpr =
            parent.get<clang::CXXTemporaryObjectExpr>()) {
      *mutationLocation = cxxTemporaryObjectExpr->getExprLoc();
      *astNodeType = ASTNodeType::CXX_TEMPORARY_OBJECT_EXPRESSION;
      return true;
    }

    // TODO: Not implemented on the LLVM IR level.
    if (const clang::ArraySubscriptExpr *arraySubscriptExpr =
            parent.get<clang::ArraySubscriptExpr>()) {
      *mutationLocation = arraySubscriptExpr->getExprLoc();
      *astNodeType = ASTNodeType::ARRAY_SUBSCRIPT_EXPRESSION;
      return false;
    }

    // TODO: Not implemented
    llvm::errs() << "\n";
    statement->dump(llvm::errs(), sourceManager);
    parent.dump(llvm::errs(), sourceManager);
    //    assert(0);

    return false;
  }

  return false;
}

ASTVisitor::ASTVisitor(mull::ThreadSafeASTUnit &astUnit,
                       mull::ASTMutations &mutations,
                       mull::FilePathFilter &filePathFilter,
                       mull::TraverseMask traverseMask)
    : astUnit(astUnit), mutations(mutations), filePathFilter(filePathFilter),
      sourceManager(astUnit.getSourceManager()), traverseMask(traverseMask),
      shouldSkipCurrentFunction(false) {}

bool ASTVisitor::VisitFunctionDecl(clang::FunctionDecl *Decl) {
  if (Decl->getNameAsString() == "main") {
    shouldSkipCurrentFunction = true;
  } else {
    shouldSkipCurrentFunction = false;
  }
  return clang::RecursiveASTVisitor<ASTVisitor>::VisitFunctionDecl(Decl);
}

bool ASTVisitor::VisitExpr(clang::Expr *expr) {
  //  expr->dump();

  if (shouldSkipCurrentFunction) {
    return true;
  }

  clang::SourceLocation exprLocation = expr->getBeginLoc();
  if (astUnit.isInSystemHeader(exprLocation)) {
    return true;
  }

  const std::string sourceLocation =
      astUnit.getSourceManager().getFilename(exprLocation).str();

  if (sourceLocation.empty()) {
    /// TODO: asserts only?
    return true;
  }

  if (filePathFilter.shouldSkip(sourceLocation)) {
    return true;
  }

  /// In case of PredefinedExpr, the expression expr->children() yields
  /// children which are nullptr. These nodes should be junk anyway, so we
  /// ignore them early.
  if (clang::isa<clang::PredefinedExpr>(expr)) {
    return true;
  }

  /// Binary operations

  if (clang::BinaryOperator *binaryOperator =
          clang::dyn_cast<clang::BinaryOperator>(expr)) {

    clang::SourceLocation binaryOperatorLocation =
        binaryOperator->getOperatorLoc();

    for (const std::pair<clang::BinaryOperator::Opcode, mull::MutatorKind>
             &mutation : BINARY_MUTATIONS) {
      if (binaryOperator->getOpcode() == mutation.first &&
          traverseMask.includesMutator(mutation.second)) {

        for (auto &parent :
             astUnit.getASTContext().getParents(*binaryOperator)) {
          /// This case removes binary operators that are parts of template
          /// arguments, for example:
          /// return variant_index<VariantType, T, index + 1>();
          /// TODO: Some other edge cases can hide behind this though and we
          /// might want to whitelist them back later.
          if (parent.get<clang::UnresolvedLookupExpr>()) {
            return true;
          }
        }

        saveMutationPoint(mutation.second, ASTNodeType::BINARY_OPERATOR,
                          binaryOperator, binaryOperatorLocation);
        return true;
      }
    }

    return true;
  }

  /// Replace Call

  if (traverseMask.includesMutator(MutatorKind::ReplaceCallMutator)) {
    if (clang::isa<clang::CallExpr>(expr)) {
      saveMutationPoint(mull::MutatorKind::ReplaceCallMutator,
                        ASTNodeType::CALL_EXPRESSION, expr,
                        expr->getBeginLoc());

      return true;
    }
  }

  /// Negate

  if (traverseMask.includesMutator(MutatorKind::NegateMutator)) {
    if (clang::isa<clang::ImplicitCastExpr>(expr)) {
      return true;
    }

    if (clang::DeclRefExpr *declRefExpr =
            clang::dyn_cast<clang::DeclRefExpr>(expr)) {
      for (auto &parent : astUnit.getASTContext().getParents(*declRefExpr)) {
        if (const clang::UnaryOperator *unaryOperator =
                parent.get<clang::UnaryOperator>()) {
          if (clang::Expr *subExpr = unaryOperator->getSubExpr()) {
            saveMutationPoint(mull::MutatorKind::NegateMutator,
                              ASTNodeType::UNARY_OPERATOR, unaryOperator,
                              subExpr->getExprLoc());
            return true;
          }
        }
      }

      // TODO does this have to be here? CALL_EXPRESSION?
      saveMutationPoint(mull::MutatorKind::NegateMutator,
                        ASTNodeType::CALL_EXPRESSION, declRefExpr,
                        declRefExpr->getExprLoc());
    }

    return true;
  }

  /// Scalar

  if (traverseMask.includesMutator(MutatorKind::ScalarValueMutator) &&
      isConstant(expr)) {
    expr = expr->IgnoreImplicit();

    //    expr = expr->Ignore();
    /// The constant literals do not have their own debug information in the
    /// LLVM IR if they are part of a parent instruction, such as binary
    /// operator, return instruction, call expr, etc.
    /// Therefore we check for the parent nodes that can result in mutable IR
    /// instructions. We record the parent node' location instead of the
    /// constant location.

    clang::SourceLocation mutationLocation;
    ASTNodeType astNodeType;
    bool foundMutableParent = findPotentialMutableParentStmt(
        expr, astUnit.getASTContext(), sourceManager, &mutationLocation,
        &astNodeType);

    if (foundMutableParent) {
      saveMutationPoint(mull::MutatorKind::ScalarValueMutator, astNodeType,
                        expr, mutationLocation);
      return true;
    }

    // parent.dump(llvm::errs(), sourceManager);

    /// TODO: STAN
    /// assert(0 && "Should not reach here");
  }

  /// Remove Void

  if (const clang::CallExpr *callExpr =
          clang::dyn_cast<clang::CallExpr>(expr)) {
    if (traverseMask.includesMutator(MutatorKind::RemoveVoidFunctionMutator)) {
      auto *type = callExpr->getType().getTypePtrOrNull();
      if (type && type->isVoidType()) {
        saveMutationPoint(mull::MutatorKind::RemoveVoidFunctionMutator,
                          ASTNodeType::CALL_EXPRESSION, callExpr,
                          callExpr->getBeginLoc());
      }
      return true;
    }
  }

  return true;
}

void ASTVisitor::traverse() {
  clang::ASTContext &context = astUnit.getASTContext();
  TraverseDecl(context.getTranslationUnitDecl());
}

void ASTVisitor::saveMutationPoint(mull::MutatorKind mutatorKind,
                                   ASTNodeType nodeType,
                                   const clang::Stmt *stmt,
                                   clang::SourceLocation location) {

  int beginLine = sourceManager.getExpansionLineNumber(location, nullptr);
  int beginColumn = sourceManager.getExpansionColumnNumber(location);

  std::string sourceFilePath =
      astUnit.getSourceManager().getFilename(location).str();
  if (sourceFilePath.size() == 0) {
    /// we reach here because of asserts()
    /// TODO: maybe there are more cases.
    return;
  }

  if (!llvm::sys::fs::is_regular_file(sourceFilePath) &&
      sourceFilePath != "input.cc") {
    llvm::errs() << "error: ASTVisitor: invalid source file path: '"
                 << sourceFilePath << "'\n";

    exit(1);
  }

  mutations.saveExpr(sourceFilePath, mutatorKind, nodeType, stmt, beginLine,
                     beginColumn);
}
