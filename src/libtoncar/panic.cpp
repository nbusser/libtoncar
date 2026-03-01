#include "panic.h"

#include <cstdint>
#include <string_view>

#include "colors.h"
#include "registers/display.h"
#include "screen.h"

namespace toncar {

// TODO: include error message
// TODO: disable on production mode
void Panic(std::string_view /*condition*/, std::string_view /*file*/, int32_t /*line*/) {
  Dispcnt::Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer(Dispcnt::Layer::DcntBg2);
  for (uint32_t i{0}; i < Screen::kHeight * Screen::kWidth; ++i) {
    uint32_t row{i / Screen::kWidth};
    uint32_t col{i % Screen::kWidth};
    Screen::Mode3WritePixel(static_cast<uint8_t>(col), static_cast<uint8_t>(row), colors15::kRed);
  }
  while (true) {
    // Trap
  }
}

}  // namespace toncar
