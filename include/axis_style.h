#ifndef AXISSTYLE_H
#define AXISSTYLE_H

#include <concepts>

#include <TAxis.h>

namespace ROOThelpers {

constexpr float DEFAULT_TITLE_SIZE = 0.05F;
constexpr float DEFAULT_LABEL_SIZE = 0.05F;
constexpr float DEFAULT_X_TITLE_OFFSET = 1.0F;
constexpr float DEFAULT_Y_TITLE_OFFSET = 1.1F;

template <typename T>
concept HasAxis = requires(T *obj) {
  { obj->GetXaxis() } -> std::same_as<TAxis *>;
  { obj->GetYaxis() } -> std::same_as<TAxis *>;
};

inline void styleAxis(TAxis *axis, const char *title, float title_size,
                      float label_size, float title_offset) noexcept {
  axis->SetTitle(title);
  axis->SetTitleSize(title_size);
  axis->SetLabelSize(label_size);
  axis->SetTitleOffset(title_offset);
}

/// Style the X axis of an object
template <HasAxis TObject>
void styleXaxis(TObject *obj, const char *title = "",
                float title_size = DEFAULT_TITLE_SIZE,
                float label_size = DEFAULT_LABEL_SIZE,
                float title_offset = DEFAULT_X_TITLE_OFFSET) noexcept {
  TAxis *x_axis = obj->GetXaxis();
  styleAxis(x_axis, title, title_size, label_size, title_offset);
}

/// Style the Y axis of an object
template <HasAxis TObject>
void styleYaxis(TObject *obj, const char *title = "",
                float title_size = DEFAULT_TITLE_SIZE,
                float label_size = DEFAULT_LABEL_SIZE,
                float title_offset = DEFAULT_Y_TITLE_OFFSET) noexcept {
  TAxis *y_axis = obj->GetYaxis();
  styleAxis(y_axis, title, title_size, label_size, title_offset);
}

/// Style both X and Y axes with default settings
template <HasAxis TObject>
void styleAxes(TObject *obj, const char *xtitle = "",
               const char *ytitle = "") noexcept {
  styleXaxis(obj, xtitle);
  styleYaxis(obj, ytitle);
}

} // namespace ROOThelpers
#endif
