#ifndef TCHAINMANAGER_H
#define TCHAINMANAGER_H

#include "ROOTObjectManager.h"
#include <cstddef>
#include <string>
#include <vector>

#include "TChain.h"

namespace ROOThelpers {

struct TChainBuildOptions {
  std::size_t duplicate_sample_count = 10;
  bool validate_tree_structure = true;
  bool check_duplicates = true;
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