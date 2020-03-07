#pragma once

#include <mutex>
#include <string>

namespace mull {

class Diagnostics {
public:
  enum LogLevel { Info, Warning, Error };

  Diagnostics();

  void info(const std::string &message);
  void warning(const std::string &message);
  void error(const std::string &message);
  void progress(const std::string &message, bool clear = true);

private:
  void log(LogLevel level, const std::string &message);
  void interceptProgress();

  std::mutex mutex;
  bool seenProgress;
  bool showColor;
  bool hasTerm;
  FILE *output;
};

} // namespace mull
