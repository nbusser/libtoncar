#include <stdlib.h>
#include <tonc.h>

#include "toncar.h"

using namespace libtoncar;

int main(void) {
  Dispcnt::Reset()
      .Dispcnt::SetMode(Dispcnt::Mode::DcntMode3)
      .Dispcnt::SetLayer(Dispcnt::Layer::DcntBg2);
  Screen::WritePixel(120, 80, Color15{31, 0, 0})
      .WritePixel(136, 80, Color15{0, 31, 0})
      .WritePixel(120, 96, Color15{0, 0, 31});

  while (1) {
    VBlankIntrWait();
  }
}
