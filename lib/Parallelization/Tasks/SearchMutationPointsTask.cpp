#include "mull/Parallelization/Tasks/SearchMutationPointsTask.h"

#include "mull/Parallelization/Progress.h"
#include "mull/Program/Program.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

#include <vector>

using namespace mull;
using namespace llvm;

SearchMutationPointsTask::SearchMutationPointsTask(
    const Program &program, std::vector<std::unique_ptr<Mutator>> &mutators,
    const ASTInformation &astInformation)
    : program(program), mutators(mutators), astInformation(astInformation) {}

void SearchMutationPointsTask::operator()(iterator begin, iterator end,
                                          Out &storage,
                                          progress_counter &counter) {
  for (auto it = begin; it != end; it++, counter.increment()) {
    FunctionUnderTest &functionUnderTest = *it;
    Function *function = functionUnderTest.getFunction();
    auto moduleID = function->getParent()->getModuleIdentifier();
    Bitcode *bitcode = program.bitcodeWithIdentifier(moduleID);

    std::string fileName =
        functionUnderTest.getFunction()->getParent()->getSourceFileName();

    for (auto &mutator : mutators) {
      auto mutants =
          mutator->getMutations(bitcode, functionUnderTest, astInformation);
      for (auto mutant : mutants) {
        for (auto &reachableTest : functionUnderTest.getReachableTests()) {
          mutant->addReachableTest(reachableTest.first, reachableTest.second);
        }
        storage.push_back(std::unique_ptr<MutationPoint>(mutant));
      }
    }
  }
}
