#include <gtest/gtest.h>

#include <TAxis.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH2F.h>

#include "axis_style.h"

class AxisStyleTest : public ::testing::Test {
public:
  static void SetUpTestSuite() { TH1::AddDirectory(false); }
};

TEST_F(AxisStyleTest, StyleAxisTH1F) {
  TH1F histo("h", "h", 1, 0, 1);
  TAxis *xaxis = histo.GetXaxis();
  ROOThelpers::styleAxis(xaxis, "XTitle", 0.06F, 0.06F, 1.2F);

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleAxisTH1D) {
  TH1D histo("h", "h", 1, 0, 1);
  TAxis *xaxis = histo.GetXaxis();
  ROOThelpers::styleAxis(xaxis, "XTitle", 0.06F, 0.06F, 1.2F);

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleXAxisTH1F) {
  TH1F histo("h", "h", 1, 0, 1);
  ROOThelpers::styleXaxis(&histo);
  TAxis *xaxis = histo.GetXaxis();

  EXPECT_STREQ(xaxis->GetTitle(), "");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), ROOThelpers::DEFAULT_X_TITLE_OFFSET);
}

TEST_F(AxisStyleTest, StyleYAxisTH1F) {
  TH1F histo("h", "h", 1, 0, 1);
  ROOThelpers::styleYaxis(&histo, "YTitle", 0.06F, 0.06F, 1.2F);
  TAxis *yaxis = histo.GetYaxis();

  EXPECT_STREQ(yaxis->GetTitle(), "YTitle");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleXAxisTH2F) {
  TH2F histo("h", "h", 1, 0, 1, 1, 0, 1);

  TAxis *yaxis = histo.GetYaxis();
  yaxis->SetTitle("YDontTouch");
  yaxis->SetTitleSize(0.06F);
  yaxis->SetLabelSize(0.06F);
  yaxis->SetTitleOffset(1.2F);

  ROOThelpers::styleXaxis(&histo);

  TAxis *xaxis = histo.GetXaxis();
  EXPECT_STREQ(xaxis->GetTitle(), "");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), ROOThelpers::DEFAULT_X_TITLE_OFFSET);
  EXPECT_STREQ(yaxis->GetTitle(), "YDontTouch");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleYAxisTH2F) {
  TH2F histo("h", "h", 1, 0, 1, 1, 0, 1);

  TAxis *xaxis = histo.GetXaxis();
  xaxis->SetTitle("XDontTouch");
  xaxis->SetTitleSize(0.06F);
  xaxis->SetLabelSize(0.06F);
  xaxis->SetTitleOffset(1.2F);

  ROOThelpers::styleYaxis(&histo);

  TAxis *yaxis = histo.GetYaxis();
  EXPECT_STREQ(yaxis->GetTitle(), "");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), ROOThelpers::DEFAULT_Y_TITLE_OFFSET);
  EXPECT_STREQ(xaxis->GetTitle(), "XDontTouch");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleAxesTH2F) {
  TH2F histo("h", "h", 1, 0, 1, 1, 0, 1);
  ROOThelpers::styleAxes(&histo, "XTitle", "YTitle");

  TAxis *xaxis = histo.GetXaxis();
  TAxis *yaxis = histo.GetYaxis();

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), ROOThelpers::DEFAULT_X_TITLE_OFFSET);

  EXPECT_STREQ(yaxis->GetTitle(), "YTitle");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), ROOThelpers::DEFAULT_Y_TITLE_OFFSET);
}