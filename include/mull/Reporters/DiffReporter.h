#pragma once

#include "Reporter.h"

namespace mull {

class Diagnostics;
class SourceInfoProvider;

class DiffReporter : public Reporter {
public:
  DiffReporter(Diagnostics &diagnostics, SourceInfoProvider &sourceInfoProvider);
  ~DiffReporter() override;
  void reportResults(const Result &result, const Metrics &metrics) override;

private:
  Diagnostics &diagnostics;
  SourceInfoProvider &sourceInfoProvider;
};

} // namespace mull
