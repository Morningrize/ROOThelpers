#include <gtest/gtest.h>

#include <TH1D.h>
#include <TH1F.h>
#include <TH2F.h>

#include "axis_style.h"

class AxisStyleTest : public ::testing::Test {
public:
  static void SetUpTestSuite() { TH1::AddDirectory(false); }
};

TEST_F(AxisStyleTest, StyleAxisTH1F) {
  TH1F h("h", "h", 1, 0, 1);
  TAxis *xaxis = h.GetXaxis();
  ROOThelpers::style_axis(xaxis, "XTitle", 0.06F, 0.06F, 1.2F);

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleAxisTH1D) {
  TH1D h("h", "h", 1, 0, 1);
  TAxis *xaxis = h.GetXaxis();
  ROOThelpers::style_axis(xaxis, "XTitle", 0.06F, 0.06F, 1.2F);

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleXAxisTH1F) {
  TH1F h("h", "h", 1, 0, 1);
  ROOThelpers::style_x_axis(&h);
  TAxis *xaxis = h.GetXaxis();

  EXPECT_STREQ(xaxis->GetTitle(), "");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), ROOThelpers::DEFAULT_X_TITLE_OFFSET);
}

TEST_F(AxisStyleTest, StyleYAxisTH1F) {
  TH1F h("h", "h", 1, 0, 1);
  ROOThelpers::style_y_axis(&h, "YTitle", 0.06F, 0.06F, 1.2F);
  TAxis *yaxis = h.GetYaxis();

  EXPECT_STREQ(yaxis->GetTitle(), "YTitle");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), 0.06F);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), 1.2F);
}

TEST_F(AxisStyleTest, StyleXAxisTH2F) {
  TH2F h("h", "h", 1, 0, 1, 1, 0, 1);

  TAxis *yaxis = h.GetYaxis();
  yaxis->SetTitle("YDontTouch");
  yaxis->SetTitleSize(0.06F);
  yaxis->SetLabelSize(0.06F);
  yaxis->SetTitleOffset(1.2F);

  ROOThelpers::style_x_axis(&h);

  TAxis *xaxis = h.GetXaxis();
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
  TH2F h("h", "h", 1, 0, 1, 1, 0, 1);

  TAxis *xaxis = h.GetXaxis();
  xaxis->SetTitle("XDontTouch");
  xaxis->SetTitleSize(0.06F);
  xaxis->SetLabelSize(0.06F);
  xaxis->SetTitleOffset(1.2F);

  ROOThelpers::style_y_axis(&h);

  TAxis *yaxis = h.GetYaxis();
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
  TH2F h("h", "h", 1, 0, 1, 1, 0, 1);
  ROOThelpers::style_axes(&h, "XTitle", "YTitle");

  TAxis *xaxis = h.GetXaxis();
  TAxis *yaxis = h.GetYaxis();

  EXPECT_STREQ(xaxis->GetTitle(), "XTitle");
  EXPECT_FLOAT_EQ(xaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(xaxis->GetTitleOffset(), ROOThelpers::DEFAULT_X_TITLE_OFFSET);

  EXPECT_STREQ(yaxis->GetTitle(), "YTitle");
  EXPECT_FLOAT_EQ(yaxis->GetTitleSize(), ROOThelpers::DEFAULT_TITLE_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetLabelSize(), ROOThelpers::DEFAULT_LABEL_SIZE);
  EXPECT_FLOAT_EQ(yaxis->GetTitleOffset(), ROOThelpers::DEFAULT_Y_TITLE_OFFSET);
}