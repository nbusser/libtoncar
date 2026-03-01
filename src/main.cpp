#include <tonc.h>

#include <cstdlib>

#include "libtoncar/colors.h"
#include "libtoncar/registers/display.h"
#include "libtoncar/screen.h"
#include "mgba/logger.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

int main() {
  mgba::Logger::Log(mgba::Logger::Level::Info, "Start");

  Dispcnt::Instance().Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (true) {
    VBlankIntrWait();
  }
}
