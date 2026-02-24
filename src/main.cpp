#include <stdlib.h>
#include <tonc.h>

#include "libtoncar/colors.h"
#include "libtoncar/registers.h"
#include "libtoncar/screen.h"

using namespace toncar;

int main(void) {
  Dispcnt::Reset()
      .Dispcnt::SetMode(Dispcnt::Mode::DcntMode3)
      .Dispcnt::SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::WritePixel(120, 80, colors15::kRed)
      .WritePixel(136, 80, colors15::kGreen)
      .WritePixel(120, 96, colors15::kBlue);

  while (1) {
    VBlankIntrWait();
  }
}
