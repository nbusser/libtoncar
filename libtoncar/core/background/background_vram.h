#pragma once

#include <array>
#include <cstdint>

#include "asm/memcpy.h"
#include "gba-assert.h"
#include "mgba/logger.h"
#include "sprite.h"
#include "toncar.h"

namespace toncar {

class BackgroundVram final {
 public:
  static BackgroundVram& Instance() {
    static BackgroundVram vram{};
    return vram;
  }

 private:
  BackgroundVram() = default;
};

}  // namespace toncar
