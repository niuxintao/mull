#pragma once

#include <string>
#include <vector>

#include "mull/ExecutionResult.h"
#include "mull/Instrumentation/InstrumentationInfo.h"

namespace llvm {
class Function;
}

namespace mull {

class Test {
public:
  Test(std::string test, std::string program, std::string driverFunctionName,
       std::vector<std::string> args, llvm::Function *testBody);

  std::string getTestName() const;
  std::string getProgramName() const;
  std::string getDriverFunctionName() const;
  std::string getTestDisplayName() const;
  std::string getUniqueIdentifier() const;
  const std::vector<std::string> &getArguments() const;
  const llvm::Function *getTestBody() const;

  void setExecutionResult(ExecutionResult result);
  const ExecutionResult &getExecutionResult() const;
  InstrumentationInfo &getInstrumentationInfo();

private:
  std::string testName;
  std::string programName;
  std::string driverFunctionName;
  std::vector<std::string> arguments;
  llvm::Function *testBody;

  ExecutionResult executionResult;
  InstrumentationInfo instrumentationInfo;
};

} // namespace mull
