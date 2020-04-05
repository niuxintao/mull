#pragma once

#include "Reporter.h"

namespace mull {

class Diagnostics;

class DiffReporter : public Reporter {
public:
  DiffReporter(Diagnostics &diagnostics);
  ~DiffReporter() override;
  void reportResults(const Result &result, const Metrics &metrics) override;

private:
  Diagnostics &diagnostics;
};

} // namespace mull
