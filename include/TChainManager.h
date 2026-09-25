#ifndef TCHAINMANAGER_H
#define TCHAINMANAGER_H

#include <string>
#include <vector>

#include "RtypesCore.h"
#include "TChain.h"

#include "ROOTObjectManager.h"

namespace ROOThelpers {

constexpr Long64_t MAX_SAMPLE_COUNT = 1000;

struct TChainBuildOptions {
  bool validate_tree_structure = true;
  bool check_duplicates = true;
  Long64_t duplicate_sample_count = 10;
};

class TChainManager {
public:
  explicit TChainManager(std::vector<std::string> files, std::string tree_name,
                         TChainBuildOptions options = {});

  [[nodiscard]] ROOTObjectManager<TChain> build() const;

private:
  std::vector<std::string> files_;
  std::string tree_name_;
  TChainBuildOptions options_;
};

} // namespace ROOThelpers
#endif