#include <tonc.h>

#include <cstdlib>

#include "libtoncar/colors.h"
#include "libtoncar/registers/display.h"
#include "libtoncar/screen.h"
#include "mgba/logger.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

namespace {
// Waiting for a proper VBlank synchronizer.
void ManualVsync() {
  // Wait until VDraw
  while (VCount::Instance().Value() >= 160) {
  }
  // Wait until VBlank
  while (VCount::Instance().Value() < 160) {
  }
}
}  // namespace

int main() {
  Dispcnt& dispcnt = Dispcnt::Instance();
  Screen& screen = Screen::Instance();
  // DispStat& dispstat = DispStat::Instance();

  MGBA_LOG_INFO("Start");

  dispcnt.Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer<Dispcnt::Layer::DcntBg2>();
  screen.Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (true) {
    ManualVsync();
  }
}
