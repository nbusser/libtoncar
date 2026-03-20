#pragma once

#include <asm/memcpy.h>
#include <libtoncar/toncar.h>
#include <mgba/logger.h>
#include <sprite.h>
#include <toncar.h>

#include <array>
#include <cstdint>

#include "gba-assert.h"

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
