
#include "ExecutionResult.h"
#include <functional>

namespace mull {

class ProcessSandbox {
public:
  virtual ~ProcessSandbox() = default;
  virtual ExecutionResult run(std::function<ExecutionStatus()> function,
                              long long timeoutMilliseconds) = 0;
};

class ForkProcessSandbox : public ProcessSandbox {
public:
  const static int MullExitCode = 0;
  const static int MullTimeoutCode = 239;

  ExecutionResult run(std::function<ExecutionStatus()> function,
                      long long timeoutMilliseconds) override;
};

class NullProcessSandbox : public ProcessSandbox {
public:
  ExecutionResult run(std::function<ExecutionStatus()> function,
                      long long timeoutMilliseconds) override;
};

} // namespace mull
