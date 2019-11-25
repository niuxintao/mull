#pragma once

#include "TrivialCXXMutator.h"

namespace mull {

namespace cxx {

class Negation : public TrivialCXXMutator {
public:
  static const std::string ID;
  Negation();
};

} // namespace cxx
} // namespace mull