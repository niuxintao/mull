#pragma once

#include <string>
#include <unordered_set>

namespace mull {

enum class MutatorKind {
  NegateMutator,
  RemoveVoidFunctionMutator,
  ReplaceCallMutator,
  AndOrReplacementMutator,
  ScalarValueMutator,

  CXX_Arithmetic_AddToSub,
  CXX_Arithmetic_AddAssignToSubAssign,
  CXX_Arithmetic_PreIncToPreDec,
  CXX_Arithmetic_PostIncToPostDec,

  CXX_Arithmetic_SubToAdd,
  CXX_Arithmetic_SubAssignToAddAssign,
  CXX_Arithmetic_PreDecToPreInc,
  CXX_Arithmetic_PostDecToPostInc,

  CXX_Arithmetic_MulToDiv,
  CXX_Arithmetic_MulAssignToDivAssign,

  CXX_Arithmetic_DivToMul,
  CXX_Arithmetic_DivAssignToMulAssign,

  CXX_Arithmetic_RemToDiv,
  CXX_Arithmetic_RemAssignToDivAssign,

  CXX_Bitwise_LShiftToRShift,
  CXX_Bitwise_LShiftAssignToRShiftAssign,

  CXX_Bitwise_RShiftToLShift,
  CXX_Bitwise_RShiftAssignToLShiftAssign,

  CXX_Bitwise_OrToAnd,
  CXX_Bitwise_OrAssignToAndAssign,
  CXX_Bitwise_AndToOr,
  CXX_Bitwise_AndAssignToOrAssign,
  CXX_Bitwise_XorToOr,
  CXX_Bitwise_XorAssignToOrAssign,

  CXX_Relation_LessThanToLessOrEqual,
  CXX_Relation_LessOrEqualToLessThan,
  CXX_Relation_GreaterThanToGreaterOrEqual,
  CXX_Relation_GreaterOrEqualToGreaterThan,

  CXX_Relation_GreaterThanToLessOrEqual,
  CXX_Relation_GreaterOrEqualToLessThan,
  CXX_Relation_LessThanToGreaterOrEqual,
  CXX_Relation_LessOrEqualToGreaterThan,

  CXX_Relation_EqualToNotEqual,
  CXX_Relation_NotEqualToEqual,

  CXX_Number_AssignConst,
  CXX_Number_InitConst,
};

static std::string MutationKindToString(MutatorKind mutatorKind) {
  switch (mutatorKind) {
    case MutatorKind::NegateMutator: {
      return "Negate";
    }
    case MutatorKind::RemoveVoidFunctionMutator: {
      return "Remove Void";
    }
    case MutatorKind::ReplaceCallMutator: {
      return "Replace Call";
    }
    case MutatorKind::AndOrReplacementMutator: {
      return "And-Or";
    }
    case MutatorKind::ScalarValueMutator: {
      return "Scalar Value";
    }
    case MutatorKind::CXX_Arithmetic_AddToSub: {
      return "Add to Sub";
    }
    default: {
      return "TODO";
    }
  }
}

} // namespace mull

namespace std {

template <> struct std::hash<mull::MutatorKind> {
  std::size_t operator()(const mull::MutatorKind &k) const {
    return static_cast<std::size_t>(k);
  }
};

} // namespace std
