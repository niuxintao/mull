#pragma once

#include "Reporter.h"

namespace mull {

class ASTStorage;

class SourceCodeReporter : public Reporter {
public:
  explicit SourceCodeReporter(ASTStorage &astStorage);
  void reportResults(const Result &result, const RawConfig &config,
                     const Metrics &metrics) override;

private:
  ASTStorage &astStorage;
};

} // namespace mull
