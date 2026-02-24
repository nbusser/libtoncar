#include <stdlib.h>
#include <tonc.h>

#include "libtoncar/colors.h"
#include "libtoncar/registers.h"
#include "libtoncar/screen.h"
#include "src/libtoncar/panic.h"

using namespace toncar;

int main(void) {
  Dispcnt::Reset()
      .Dispcnt::SetMode(Dispcnt::Mode::DcntMode3)
      .Dispcnt::SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (1) {
    VBlankIntrWait();
  }
}
