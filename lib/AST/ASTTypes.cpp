#include "mull/AST/ASTTypes.h"

namespace mull {

TraverseMask TraverseMask::create(std::vector<MutatorKind> mutators) {
  std::unordered_set<MutatorKind> mutatorsSet(mutators.begin(), mutators.end());
  return TraverseMask(mutatorsSet);
}

bool TraverseMask::includesMutator(mull::MutatorKind mutatorKind) const {
  return mutators.count(mutatorKind) > 0;
}

} // namespace mull
