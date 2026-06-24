#ifndef TCHAINMANAGER_H
#define TCHAINMANAGER_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class TChain;

namespace ROOThelpers {

struct TChainBuildOptions {
  std::size_t duplicate_sample_count = 10;
  bool check_branch_consistency = true;
  bool check_duplicates = true;
};

class TChainManager {
public:
  explicit TChainManager(std::vector<std::string> files, std::string tree_name,
                         TChainBuildOptions options = {});

  [[nodiscard]] std::unique_ptr<TChain> build() const;

private:
  std::vector<std::string> files_;
  std::string tree_name_;
  TChainBuildOptions options_;
};

} // namespace ROOThelpers
#endif