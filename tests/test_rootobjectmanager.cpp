#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>
#include <type_traits>

#include "ROOTObjectManager.h"

namespace {

static_assert(ROOThelpers::TObjectTDirectoryOwned<TH1D>,
              "TH1D should satisfy concept TObjectGDirectoryOwned");
static_assert(ROOThelpers::TObjectTDirectoryOwned<TTree>,
              "TTree should satisfy concept TObjectGDirectoryOwned");
static_assert(!ROOThelpers::TObjectTDirectoryOwned<TFile>,
              "TFile should not satisfy concept TObjectGDirectoryOwned");
static_assert(!ROOThelpers::TObjectTDirectoryOwned<TGraph>,
              "TGraph should not satisfy concept TObjectGDirectoryOwned");

static_assert(
    !std::is_default_constructible_v<ROOThelpers::ROOTObjectManager<TH1D>>);
static_assert(
    !std::is_copy_constructible_v<ROOThelpers::ROOTObjectManager<TH1D>>);
static_assert(!std::is_copy_assignable_v<ROOThelpers::ROOTObjectManager<TH1D>>);
static_assert(
    std::is_move_constructible_v<ROOThelpers::ROOTObjectManager<TH1D>>);
static_assert(std::is_move_assignable_v<ROOThelpers::ROOTObjectManager<TH1D>>);
} // namespace

class ROOTObjectManagerTest : public ::testing::Test {
public:
  static void SetUpTestSuite() { TH1::AddDirectory(false); }
};

TEST_F(ROOTObjectManagerTest, MakeUserOwnedTH1D) {
  auto histo = ROOThelpers::ROOTObjectManager<TH1D>("histo", "histo", 1, 0, 1);
  ASSERT_NE(histo.get(), nullptr);
  EXPECT_EQ(histo->GetDirectory(), nullptr);
  EXPECT_STREQ(histo->GetName(), "histo");
}

TEST_F(ROOTObjectManagerTest, ArrowOperatorAccessObject) {
  auto histo = ROOThelpers::ROOTObjectManager<TH1D>("histo", "histo", 1, 0, 1);
  histo->Fill(0.5);
  EXPECT_EQ(histo->GetEntries(), 1);
}

TEST_F(ROOTObjectManagerTest, ReleaseToDirectoryTH1D) {
  TFile file("test_rootobjectmanager.root", "RECREATE", "",
             TFile::kWriteDelete);
  auto histo = ROOThelpers::ROOTObjectManager<TH1D>("histo", "histo", 1, 0, 1);
  TH1D *hraw = histo.releaseToDirectory(&file);
  EXPECT_EQ(histo.get(), nullptr);
  ASSERT_NE(hraw, nullptr);
  EXPECT_EQ(hraw->GetDirectory(), &file);
  EXPECT_NE(file.GetList()->FindObject("histo"), nullptr);
}

TEST_F(ROOTObjectManagerTest, ReleaseToNullThrows) {
  auto histo = ROOThelpers::ROOTObjectManager<TH1D>("histo", "histo", 1, 0, 1);
  EXPECT_THROW((void)histo.releaseToDirectory(nullptr), std::invalid_argument);
}

TEST_F(ROOTObjectManagerTest, DoubleReleaseThrows) {
  TFile file("test_rootobjectmanager.root", "RECREATE", "",
             TFile::kWriteDelete);
  auto histo = ROOThelpers::ROOTObjectManager<TH1D>("histo", "histo", 1, 0, 1);
  TH1D *hraw = histo.releaseToDirectory(&file);
  ASSERT_NE(hraw, nullptr);
  EXPECT_THROW((void)histo.releaseToDirectory(), std::invalid_argument);
}

TEST_F(ROOTObjectManagerTest, MoveAssignmentWorks) {
  auto histo1 =
      ROOThelpers::ROOTObjectManager<TH1D>("histo1", "histo1", 1, 0, 1);
  auto histo2 =
      ROOThelpers::ROOTObjectManager<TH1D>("histo2", "histo2", 1, 0, 1);

  histo1->Fill(0.5);
  histo2 = std::move(histo1);

  EXPECT_EQ(histo2->GetEntries(), 1);
  EXPECT_STREQ(histo2->GetTitle(), "histo1");
}
