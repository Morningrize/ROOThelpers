#include "AxisStyle.h"
#include <TH1F.h>

int main() {
  TH1F h("h", "test", 1, 0, 1);
  StyleAxis(&h, "x", "y");
  return 0;
}