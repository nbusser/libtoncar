#pragma once

#include <cstdint>

namespace toncar {

enum class Interrupt : uint8_t {
  VBlank = 0,
  HBlank = 1,
  VCount = 2,
};

class InterruptManager {
 public:
  void RequestInterrupt(Interrupt interrupt);

  static InterruptManager& GetInstance() {
    static InterruptManager instance{};
    return instance;
  }

 private:
  InterruptManager();
};

}  // namespace toncar
