#include "toncar.h"

#include "registers.h"
#include "screen.h"

namespace toncar {

// TODO: include error message
// TODO: disable on production mode
void DeathTrap() {
  Dispcnt::Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer(Dispcnt::Layer::DcntBg2);
  for (uint8_t i{0}; i < Screen::kHeight; ++i) {
    for (uint8_t j{0}; j < Screen::kWidth; ++j) {
      Screen::WritePixel(j, i, Color15{31, 0, 0});
    }
  }
  while (true) {
    // Trap
  }
}

void CheckOrDie(bool condition) {
  if (!condition) {
    DeathTrap();
  }
}

}  // namespace toncar
