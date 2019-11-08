#pragma once

#include "mull/AST/ASTTypes.h"

namespace mull {

class ASTStorage;
class MutationPoint;

class SourceInfoProvider {
public:
  virtual ~SourceInfoProvider() {}

  virtual MutationPointSourceInfo
  getSourceInfo(MutationPoint *mutationPoint) = 0;
};

class ASTSourceInfoProvider : public SourceInfoProvider {
public:
  ~ASTSourceInfoProvider() = default;
  explicit ASTSourceInfoProvider(ASTStorage &astStorage);
  MutationPointSourceInfo getSourceInfo(MutationPoint *mutationPoint) override;

private:
  ASTStorage &astStorage;
};

} // namespace mull
