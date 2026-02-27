#include "panic.h"

#include <cstdint>

#include "colors.h"
#include "registers.h"
#include "screen.h"

namespace toncar {

// TODO: include error message
// TODO: disable on production mode
void Panic() {
  Dispcnt::Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer(Dispcnt::Layer::DcntBg2);
  for (uint8_t i{0}; i < Screen::kHeight; ++i) {
    for (uint8_t j{0}; j < Screen::kWidth; ++j) {
      Screen::Mode3WritePixel(j, i, colors15::kRed);
    }
  }
  while (true) {
    // Trap
  }
}

void CheckOrPanic(bool condition) {
  if (!condition) {
    Panic();
  }
}

}  // namespace toncar
