#include <stdexcept>

#include <gtest/gtest.h>

#include "RtypesCore.h"

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
