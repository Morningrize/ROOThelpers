#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RtypesCore.h"
#include "TFile.h"
#include "TTree.h"

#include "StreamCapture.h"

#include "ROOTObjectManager.h"
#include "TChainManager.h"

TEST(TChainManagerTest, ThrowsOnEmptyFileList) {
  EXPECT_THROW((void)ROOThelpers::TChainManager({}, "evt"),
               std::invalid_argument);
}

TEST(TChainManagerTest, ThrowsOnAnyEmptyFilePath) {
  EXPECT_THROW(
      (void)ROOThelpers::TChainManager({"file1.root", "", "file2.root"}, "evt"),
      std::invalid_argument);
}

TEST(TChainManagerTest, ThrowsOnEmptyTreeName) {
  EXPECT_THROW((void)ROOThelpers::TChainManager({"file.root"}, ""),
               std::invalid_argument);
}

class DuplicateCheckEnabledInvalidSampleCountTest
    : public ::testing::TestWithParam<Long64_t> {};

TEST_P(DuplicateCheckEnabledInvalidSampleCountTest, Throws) {
  ROOThelpers::TChainBuildOptions options;
  options.check_duplicates = true;
  options.duplicate_sample_count = GetParam();

  EXPECT_THROW((void)ROOThelpers::TChainManager({"file.root"}, "evt", options),
               std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
    InvalidCountsWhenEnabled, DuplicateCheckEnabledInvalidSampleCountTest,
    ::testing::Values(static_cast<Long64_t>(-1), static_cast<Long64_t>(0),
                      static_cast<Long64_t>(ROOThelpers::MAX_SAMPLE_COUNT +
                                            1)));

class DuplicateCheckDisabledSampleCountTest
    : public ::testing::TestWithParam<Long64_t> {};

TEST_P(DuplicateCheckDisabledSampleCountTest, DoesNotThrow) {
  ROOThelpers::TChainBuildOptions options;
  options.check_duplicates = false;
  options.duplicate_sample_count = GetParam();

  EXPECT_NO_THROW(
      (void)ROOThelpers::TChainManager({"file.root"}, "evt", options));
}

INSTANTIATE_TEST_SUITE_P(
    InvalidCountsWhenDisabled, DuplicateCheckDisabledSampleCountTest,
    ::testing::Values(static_cast<Long64_t>(1),
                      static_cast<Long64_t>(ROOThelpers::MAX_SAMPLE_COUNT),
                      static_cast<Long64_t>(-1), static_cast<Long64_t>(0),
                      static_cast<Long64_t>(ROOThelpers::MAX_SAMPLE_COUNT +
                                            1)));

TEST(TChainManagerTest, DoesNotThrowOnValidInputs) {
  ROOThelpers::TChainBuildOptions options;
  options.check_duplicates = true;
  options.duplicate_sample_count = ROOThelpers::MAX_SAMPLE_COUNT;

  EXPECT_NO_THROW((void)ROOThelpers::TChainManager({"file1.root", "file2.root"},
                                                   "evt", options));
}

class TChainManagerBuildTest : public ::testing::Test {
protected:
  std::filesystem::path tmp_dir; // NOLINT
  void SetUp() override {
    tmp_dir = std::filesystem::temp_directory_path() / "test_directory";
    std::filesystem::create_directory(tmp_dir);
  }
  void TearDown() override { std::filesystem::remove_all(tmp_dir); }
  template <typename T = int>
  std::string makeRootFileWithTree(const std::string &filename,
                                   const std::string &tree_name,
                                   const std::string &branch_name = "",
                                   std::initializer_list<T> values = {}) {
    ROOThelpers::ROOTObjectManager<TTree> test_tree(tree_name.c_str(), "test");

    T fill_value{};

    if (!branch_name.empty()) {
      test_tree->Branch(branch_name.c_str(), &fill_value);

      for (auto value : values) {
        fill_value = value;
        test_tree->Fill();
      }
    }

    const auto tmp_path = (tmp_dir / filename).string();
    TFile tmp_ROOT_file(tmp_path.c_str(), "RECREATE");
    tmp_ROOT_file.WriteObject(test_tree.get(), tree_name.c_str());
    return tmp_path;
  }
};

TEST_F(TChainManagerBuildTest, ThrowsOnMissingFile) {
  const ROOThelpers::TChainManager event_chain_mgr(
      {tmp_dir / "nonexistent_file.root"}, "evt");
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(), ::testing::HasSubstr("does not exist"));
  }
}

TEST_F(TChainManagerBuildTest, ThrowsOnFileNotROOT) {
  const std::filesystem::path tmp_file_path = tmp_dir / "file.txt";
  std::ofstream(tmp_file_path) << "not a ROOT file";
  const ROOThelpers::TChainManager event_chain_mgr({tmp_file_path.string()},
                                                   "evt");
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(), ::testing::HasSubstr("not readable by ROOT"));
  }
}

TEST_F(TChainManagerBuildTest, ThrowsOnRequestedTTreeNotInFile) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "requested_name", "branch", {0});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "wrong_name", "branch", {0});
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "requested_name");
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(),
                testing::HasSubstr("does not contain a TTree named"));
  }
}

TEST_F(TChainManagerBuildTest, ThrowsOnEmptyTree) {
  const auto test_file_path = makeRootFileWithTree("file.root", "evt");
  const ROOThelpers::TChainManager event_chain_mgr({test_file_path}, "evt");
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(), testing::HasSubstr("has zero entries"));
  }
}

TEST_F(TChainManagerBuildTest, ReturnsTChainOnValidFiles) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", "branch", {0});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", "branch", {1});
  const ROOThelpers::TChainBuildOptions options{
      .validate_tree_structure = false, .check_duplicates = false};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);
  auto chain = event_chain_mgr.build();
  EXPECT_EQ(chain->GetEntries(), 2);
}

TEST_F(TChainManagerBuildTest, TreeStructureTestPassesOnMatch) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", "branch", {0});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", "branch", {1});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = false};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);
  auto chain = event_chain_mgr.build();
  EXPECT_EQ(chain->GetEntries(), 2);
}

TEST_F(TChainManagerBuildTest, TreeStructureTestFailsOnNameMismatch) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", {"branch_one"}, {0});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", {"branch_two"}, {1});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = false};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(), testing::HasSubstr("structure mismatch"));
  }
}

TEST_F(TChainManagerBuildTest, TreeStructureTestFailsOnTypeMismatch) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", {"branch"}, {0});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", {"branch"}, {1.F});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = false};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);
  try {
    (void)event_chain_mgr.build();
    FAIL();
  } catch (const std::invalid_argument &exc) {
    EXPECT_THAT(exc.what(), testing::HasSubstr("structure mismatch"));
  }
}

TEST_F(TChainManagerBuildTest, WarnsOnPotentialDuplicate) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", "branch", {0, 1, 2});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", "branch", {0, 1, 2});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = true,
                                                .duplicate_sample_count = 1};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);

  const AnalysisCore::StreamCapture captured(std::clog);
  auto chain = event_chain_mgr.build();

  EXPECT_EQ(chain->GetEntries(), 6);
  EXPECT_THAT(captured.str(), ::testing::HasSubstr("duplicate"));
}

TEST_F(TChainManagerBuildTest, DoesNotWarnForDifferentTrees) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", "branch", {2, 4, 6});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", "branch", {1, 3, 5});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = true,
                                                .duplicate_sample_count = 2};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);

  const AnalysisCore::StreamCapture captured(std::clog);
  auto chain = event_chain_mgr.build();

  EXPECT_EQ(chain->GetEntries(), 6);
  EXPECT_TRUE(captured.str().empty());
}

TEST_F(TChainManagerBuildTest, HandlesGetEntriesLessThanSampleCount) {
  const auto file1_path =
      makeRootFileWithTree("file1.root", "evt", "branch", {1});
  const auto file2_path =
      makeRootFileWithTree("file2.root", "evt", "branch", {0});
  const ROOThelpers::TChainBuildOptions options{.validate_tree_structure = true,
                                                .check_duplicates = true,
                                                .duplicate_sample_count = 2};
  const ROOThelpers::TChainManager event_chain_mgr({file1_path, file2_path},
                                                   "evt", options);

  const AnalysisCore::StreamCapture captured(std::clog);
  auto chain = event_chain_mgr.build();

  EXPECT_EQ(chain->GetEntries(), 2);
  EXPECT_THAT(captured.str(),
              ::testing::HasSubstr("duplicate_sample_count exceeds"));
}
