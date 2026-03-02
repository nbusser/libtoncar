#include <asm/bios.h>
#include <tonc.h>
#include <toncar.h>

#include <cstdlib>

#include "interrupt_manager.h"
#include "libtoncar/colors.h"
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
  MGBA_LOG_DEBUG("VBlank counter: %lu", vblank_counter);
}
}  // namespace

int main() {
  InterruptManager::GetInstance()
      .AddInterruptHandler(Interrupt::VBlank, VBlankCounter)
      .EnableInterrupt(Interrupt::VBlank);

  Dispcnt& dispcnt = Dispcnt::Instance();

  MGBA_LOG_INFO("Start");

  dispcnt.Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer<Dispcnt::Layer::DcntBg2>();

  Screen& screen = Screen::Instance();

  screen.Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  while (true) {
    toncar::VBlankIntrWait();
  }
}
