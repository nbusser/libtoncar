#include <stdlib.h>
#include <tonc.h>

#include "toncar.h"

using namespace libtoncar;

int main(void) {
  Dispcnt::Reset()
      .Dispcnt::SetMode(Dispcnt::Mode::DcntMode3)
      .Dispcnt::SetLayer(Dispcnt::Layer::DcntBg2);

  screen::Screen::WritePixel(120, 80, 0x001F)
      .WritePixel(136, 80, 0x03E0)
      .WritePixel(120, 96, 0x7C00);

  while (1) {
    VBlankIntrWait();
  }
}
