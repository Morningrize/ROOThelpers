#include "TChainManager.h"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "RtypesCore.h"
#include "TChain.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "TTree.h"

#include "ROOTObjectManager.h"

namespace ROOThelpers {

namespace {

constexpr std::size_t HASH_CONSTANT = 0x9e3779b9;
constexpr unsigned LEFT_SHIFT = 6;
constexpr unsigned RIGHT_SHIFT = 2;

bool inputFileExist(const std::string &filename) {
  return std::filesystem::is_regular_file(filename);
}

bool inputFileReadable(const TFile *file) {
  return file != nullptr && file->IsOpen() && !file->IsZombie();
}

bool inputFileContainsTree(TFile *file, const std::string &tree_name) {
  if (file == nullptr) {
    return false;
  }
  TTree *tree = nullptr;
  file->GetObject(tree_name.c_str(), tree);
  return tree != nullptr;
}

TObjArray *getLeaves(TTree *tree) {
  TObjArray *leaves = nullptr;
  if (tree == nullptr) {
    throw std::invalid_argument("TChainManager: TTree is null");
  }
  leaves = tree->GetListOfLeaves();
  if (leaves == nullptr) {
    throw std::runtime_error("TChainManager: leaf list is null for TTree");
  }
  return leaves;
}

std::set<std::string> collectLeafNames(TTree *tree) {
  std::set<std::string> leaf_names_types;
  auto *leaves = getLeaves(tree);
  for (auto *obj : *leaves) {
    auto *leaf = dynamic_cast<TLeaf *>(obj);
    if (leaf == nullptr) {
      throw std::runtime_error(
          "TChainManager: TLeaf is null while processing TTree");
    }
    std::string leaf_name = leaf->GetName();
    std::string leaf_type = leaf->GetTypeName();
    if (leaf_name.empty() || leaf_type.empty()) {
      throw std::runtime_error("TChainManager: invalid TLeaf");
    }
    std::string leaf_name_type = leaf_name;
    leaf_name_type += "_";
    leaf_name_type += leaf_type;
    leaf_names_types.insert(leaf_name_type);
  }
  return leaf_names_types;
}

void combineFingerprint(std::size_t &fingerprint, std::size_t value) {
  fingerprint ^= value + HASH_CONSTANT + (fingerprint << LEFT_SHIFT) +
                 (fingerprint >> RIGHT_SHIFT);
}

std::size_t computeFingerprint(TTree *tree, Long64_t sample_count) {
  auto *leaves = getLeaves(tree);
  const Long64_t entries = tree->GetEntries();

  if (sample_count > entries) {
    std::clog << "TChainManager: duplicate_sample_count exceeds number of "
                 "entries in tree\n";
    sample_count = entries;
  }

  std::size_t fingerprint = 0;

  for (Long64_t index = 0; index < sample_count; index++) {
    Long64_t event = (index * entries / sample_count);
    tree->GetEntry(event);
    for (auto *obj : *leaves) {
      auto *leaf = dynamic_cast<TLeaf *>(obj);
      if (leaf == nullptr) {
        throw std::runtime_error(
            "TChainManager: TLeaf is null while processing TTree");
      }
      auto leaf_value_bits = std::bit_cast<std::uint64_t>(leaf->GetValue());
      combineFingerprint(fingerprint, leaf_value_bits);
    }
  }
  return fingerprint;
}

} // namespace

TChainManager::TChainManager(std::vector<std::string> files,
                             std::string tree_name, TChainBuildOptions options)
    : files_(std::move(files)), tree_name_(std::move(tree_name)),
      options_(options) {

  if (files_.empty()) {
    throw std::invalid_argument("TChainManager: input file list is empty");
  }

  for (const auto &file : files_) {
    if (file.empty()) {
      throw std::invalid_argument(
          "TChainManager: input file list contains an empty path");
    }
  }

  if (tree_name_.empty()) {
    throw std::invalid_argument("TChainManager: tree name is empty");
  }

  if (options_.duplicate_sample_count < 1 && options_.check_duplicates) {
    throw std::invalid_argument("TChainManager: duplicate_sample_count must be "
                                "> 0 when duplicate checking is enabled");
  }

  if (options_.duplicate_sample_count > MAX_SAMPLE_COUNT &&
      options_.check_duplicates) {
    throw std::invalid_argument(
        "TChainManager: duplicate_sample_count exceeds maximum of " +
        std::to_string(MAX_SAMPLE_COUNT));
  }
}

ROOTObjectManager<TChain> TChainManager::build() const {

  std::set<std::string> reference_leaf_set;
  std::unordered_map<std::size_t, std::string> tree_fingerprints;

  for (const auto &filename : files_) {

    if (!inputFileExist(filename)) {
      throw std::invalid_argument(
          "TChainManager: input file does not exist or isn't a regular file: " +
          filename);
    }

    std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
    if (!inputFileReadable(file.get())) {
      throw std::invalid_argument(
          "TChainManager: input file is not readable by ROOT: " + filename);
    }
    if (!inputFileContainsTree(file.get(), tree_name_)) {
      throw std::invalid_argument(
          "TChainManager: input file: " + filename +
          " does not contain a TTree named: " + tree_name_);
    }

    TTree *tree = nullptr;
    file->GetObject(tree_name_.c_str(), tree);

    if (options_.validate_tree_structure) {
      if (reference_leaf_set.empty()) {
        reference_leaf_set = collectLeafNames(tree);
      } else {
        auto leaf_set = collectLeafNames(tree);
        if (reference_leaf_set != leaf_set) {
          throw std::invalid_argument(
              "TChainManager: leaf structure mismatch in file: " + filename);
        }
      }
    }

    if (options_.check_duplicates) {
      auto tree_fingerprint =
          computeFingerprint(tree, options_.duplicate_sample_count);
      auto seen = tree_fingerprints.find(tree_fingerprint);
      if (seen != tree_fingerprints.end()) {
        std::clog << "TChainManager: possible duplicate tree detected between '"
                  << seen->second << "' and '" << filename << "'\n";
      }
      tree_fingerprints.emplace(tree_fingerprint, filename);
    }
  }

  ROOTObjectManager<TChain> chain(tree_name_.c_str());
  for (const auto &filename : files_) {
    if (chain->Add(filename.c_str()) == 0) {
      throw std::runtime_error(
          "TChainManager: failed to add input file to chain: " + filename);
    }
  }
  return chain;
}
} // namespace ROOThelpers
