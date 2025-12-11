#ifndef AXISSTYLE_H
#define AXISSTYLE_H

#include <TAxis.h>

template <typename T> concept HasAxis = requires(T *obj) {
  { obj->GetXaxis() }
  ->std::same_as<TAxis *>;
  { obj->GetYaxis() }
  ->std::same_as<TAxis *>;
};

template <HasAxis TObject>
void StyleAxis(TObject *obj, const char *xtitle = "", const char *ytitle = "",
               double title_size = 0.05, double label_size = 0.05) {
  obj->GetXaxis()->SetTitle(xtitle);
  obj->GetYaxis()->SetTitle(ytitle);
  obj->GetXaxis()->SetTitleSize(title_size);
  obj->GetYaxis()->SetTitleSize(title_size);
  obj->GetXaxis()->SetLabelSize(label_size);
}

#endif
