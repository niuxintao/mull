#pragma once

#include <map>
#include <vector>

#include "mull/AST/ASTInformation.h"
#include "MutationPoint.h"
#include "ReachableFunction.h"
#include "mull/Mutators/Mutator.h"

namespace mull {

struct Configuration;
class Program;
class ReachableFunction;

class MutationsFinder {
public:
  explicit MutationsFinder(std::vector<std::unique_ptr<Mutator>> mutators,
                           const Configuration &config,
                           const ASTInformation &astInformation);
  std::vector<MutationPoint *>
  getMutationPoints(const Program &program,
                    std::vector<FunctionUnderTest> &functions);

private:
  std::vector<std::unique_ptr<Mutator>> mutators;
  std::vector<std::unique_ptr<MutationPoint>> ownedPoints;
  const Configuration &config;
  const ASTInformation &astInformation;
};
} // namespace mull
