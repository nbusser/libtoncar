#pragma once

#include <cstddef>
#include <cstdint>

#include "panic.h"

namespace toncar {

namespace memory {

constexpr uintptr_t kIo{0x04000000};
constexpr uintptr_t kVram{0x06000000};

}  // namespace memory

template <typename Derived, typename T, uintptr_t addr>
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

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }

 private:
  static volatile T& Ref() { return *reinterpret_cast<volatile T*>(addr); }
};

template <typename Derived, typename T, uintptr_t base_addr, size_t size>
class Zone {
 protected:
  static T Get(size_t offset) { return BaseAddress()[offset]; }
  static Derived& Set(size_t offset, T val) {
    CheckOrPanic(offset < size);
    BaseAddress()[offset] = val;
    return Self();
  }

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }

 private:
  static volatile T* BaseAddress() { return reinterpret_cast<volatile T*>(base_addr); }
};

}  // namespace toncar
