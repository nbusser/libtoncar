#pragma once

#include <cstddef>
#include <cstdint>

#include "panic.h"

namespace toncar {

namespace memory {

constexpr uintptr_t kIo{0x04000000};
constexpr uintptr_t kVram{0x06000000};

}  // namespace memory

template <typename Derived, typename T, uintptr_t base_addr, size_t size>
class Zone {
 public:
  static Derived& Instance() {
    static Derived instance{};
    return instance;
  }

 protected:
  T Get(size_t offset) const { return BaseAddress()[offset]; }
  Derived& Set(size_t offset, T val) {
    GBA_ASSERT(offset < size);
    BaseAddress()[offset] = val;
    return Self();
  }

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }

 private:
  volatile T* BaseAddress() const { return reinterpret_cast<volatile T*>(base_addr); }
};

}  // namespace toncar
