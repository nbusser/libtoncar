#pragma once

#include <panic.h>
#include <toncar.h>

#include <cstdint>

namespace toncar {

template <typename Derived, typename T, uint32_t offset_from_io>
class Register {
 protected:
  static T Get() { return Ref(); }
  static Derived& Set(T val) {
    Ref() = val;
    return Self();
  }
  static T GetOr(T val) { return Ref() | val; }
  static T GetAnd(T val) { return Ref() & val; }

  static Derived& Or(T val) { return Set(GetOr(val)); }
  static Derived& And(T val) { return Set(GetAnd(val)); }

  static bool HasBit(uint8_t position) {
    GBA_ASSERT(position < sizeof(T) * 8);
    const T mask{1 << position};
    return (Get() & mask) == mask;
  }
  static Derived& SetBit(uint8_t position) {
    GBA_ASSERT(position < sizeof(T) * 8);
    const T mask{1 << position};
    return Or(mask);
  }
  static Derived& ClearBit(uint8_t position) {
    GBA_ASSERT(position < sizeof(T) * 8);
    const T mask{~(1 << position)};
    return And(mask);
  }

 private:
  static constexpr volatile T* kAddress{memory::kIo + offset_from_io};
  static volatile T& Ref() { return *kAddress; }

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }
};

}  // namespace toncar
