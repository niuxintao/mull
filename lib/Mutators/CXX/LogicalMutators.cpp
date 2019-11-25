#include "mull/Mutators/CXX/LogicalMutators.h"

#include <irm/irm.h>

using namespace mull;
using namespace mull::cxx;

static std::vector<std::unique_ptr<irm::IRMutation>> getNegation() {
  std::vector<std::unique_ptr<irm::IRMutation>> mutators;
  mutators.emplace_back(new irm::ICMP_NEToICMP_EQ);
  mutators.emplace_back(new irm::FCMP_ONEToFCMP_OEQ);
  mutators.emplace_back(new irm::FCMP_UNEToFCMP_UEQ);
  return mutators;
}

const std::string Negation::ID = "cxx_logical_negation";

Negation::Negation()
    : TrivialCXXMutator(std::move(getNegation()), MutatorKind::CXX_Logical_Negation, Negation::ID,
                        "Replaces !x with x", "", "Replaced !x with x") {}