#pragma once

#include <cstddef>
#include <cstdint>

namespace toncar {

namespace memory {
inline constexpr uintptr_t kIo{0x04000000};
inline constexpr uintptr_t kVram{0x06000000};
}  // namespace memory

void DeathTrap();

void CheckOrDie(bool condition);

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

template <typename Derived, typename T, uintptr_t base_addr>
class Zone {
 protected:
  static T Get(size_t offset) { return BaseAddress()[offset]; }
  static Derived& Set(size_t offset, T val) {
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

class Color15 {
 public:
  Color15(uint8_t red, uint8_t green, uint8_t blue)
      : value_{static_cast<uint16_t>(red | (green << 5) | (blue << 10))} {
    CheckOrDie(red < 32);
    CheckOrDie(green < 32);
    CheckOrDie(blue < 32);
  }

  constexpr uint16_t Value() const { return value_; }

  constexpr uint8_t Red() const { return value_ & 31; }
  constexpr uint8_t Green() const { return static_cast<uint8_t>((value_ >> 5) & 31); }
  constexpr uint8_t Blue() const { return static_cast<uint8_t>((value_ >> 10) & 31); }

 private:
  const uint16_t value_;
};

}  // namespace toncar
