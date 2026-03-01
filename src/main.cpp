#include <tonc.h>

#include <cstdlib>

#include "libtoncar/colors.h"
#include "libtoncar/registers/display.h"
#include "libtoncar/screen.h"
#include "mgba-debug/mgba.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

int main() {
  mgba_open();
  mgba_console_open();

  Dispcnt::Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (true) {
    VBlankIntrWait();
  }
  mgba_close();
}
