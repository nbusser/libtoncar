#pragma once

#include <cstdint>

#include "panic.h"

namespace toncar {

class Color15 {
 public:
  Color15(uint8_t red, uint8_t green, uint8_t blue)
      : value_{static_cast<uint16_t>(red | (green << 5U) | (blue << 10U))} {
    CheckOrPanic(red < 32U);
    CheckOrPanic(green < 32U);
    CheckOrPanic(blue < 32U);
  }

  [[nodiscard]] constexpr uint16_t Value() const { return value_; }

  [[nodiscard]] constexpr uint8_t Red() const { return value_ & 31U; }
  [[nodiscard]] constexpr uint8_t Green() const {
    return static_cast<uint8_t>((value_ >> 5U) & 31U);
  }
  [[nodiscard]] constexpr uint8_t Blue() const {
    return static_cast<uint8_t>((value_ >> 10U) & 31U);
  }

 private:
  const uint16_t value_;
};

namespace colors15 {

static const Color15 kWhite{31, 31, 31};
static const Color15 kBlack{0, 0, 0};
static const Color15 kRed{31, 0, 0};
static const Color15 kGreen{0, 31, 0};
static const Color15 kBlue{0, 0, 31};

}  // namespace colors15

}  // namespace toncar
