#include <asm/bios.h>
#include <charblock.h>
#include <palram.h>
#include <sprite.h>

#include <cstdint>
#include <cstdlib>

#include "example/assets/gfx/example.h"
#include "interrupt_manager.h"
#include "libtoncar/charblock.h"
#include "libtoncar/colors.h"
#include "libtoncar/palram.h"
#include "libtoncar/registers/display.h"
#include "libtoncar/screen.h"
#include "mgba/logger.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
uint32_t vblank_counter{0};

void VBlankCounter() {
  ++vblank_counter;
  MGBA_LOG_DEBUG("VBlank counter: %lu", static_cast<unsigned long>(vblank_counter));
}
}  // namespace

int main() {
  InterruptManager::Instance()
      .SetInterruptHandler(Interrupt::VBlank, VBlankCounter)
      .SetInterruptHandler(Interrupt::VBlank, VBlankCounter)
      .EnableInterrupt(Interrupt::VBlank);

  Dispcnt& dispcnt = Dispcnt::Instance();

  MGBA_LOG_INFO("Start");

  dispcnt.Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer<Dispcnt::Layer::DcntBg2>();

  Screen& screen = Screen::Instance();

  screen.Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  dispcnt.Reset().SetMode(Dispcnt::Mode::DcntMode0).SetLayer<Dispcnt::Layer::DcntBg2>();

  Palram::Instance().SpritesPalbank().LoadPalette(sprites::example::french.GetSprite(0).Palette());
  Charblock(0x00010000).LoadTiles(sprites::example::french.GetSprite(0));

  while (true) {
    VBlankIntrWait();
  }
}
