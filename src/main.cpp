#include <tonc.h>

#include <cstdlib>

#include "libtoncar/colors.h"
#include "libtoncar/registers.h"
#include "libtoncar/screen.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

int main() {
  Dispcnt::Reset()
      .Dispcnt::SetMode(Dispcnt::Mode::DcntMode3)
      .Dispcnt::SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (true) {
    VBlankIntrWait();
  }
}
