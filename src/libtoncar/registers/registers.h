#pragma once

#include <panic.h>
#include <toncar.h>

#include <concepts>
#include <cstdint>

namespace toncar {

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class TransactionRegister {
 protected:
  static Derived& Reset() {
    staged_value = Ref();
    return Self();
  }

  static Derived& Commit() {
    Ref() = staged_value;
    Reset();
    return Self();
  }

  static T Get() { return Ref(); }
  static Derived& Set(T val) {
    staged_value = val;
    return Self();
  }
  static T GetOr(T val) { return Ref() | val; }
  static T GetAnd(T val) { return Ref() & val; }

  static Derived& Or(T val) { return Set(staged_value | val); }
  static Derived& And(T val) { return Set(staged_value & val); }

  template <uint8_t position>
  static bool HasBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return (Get() & kMask) == kMask;
  }

  template <uint8_t position>
  static Derived& SetBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return Or(kMask);
  }

  template <uint8_t position>
  static Derived& ClearBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{~(T{1} << position)};
    return And(kMask);
  }

 private:
  static volatile T& Ref() { return *reinterpret_cast<volatile T*>(memory::kIo + offset_from_io); }

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }

  inline static T staged_value;
};

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
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

  template <uint8_t position>
  static bool HasBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return (Get() & kMask) == kMask;
  }

  template <uint8_t position>
  static Derived& SetBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return Or(kMask);
  }

  template <uint8_t position>
  static Derived& ClearBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{~(T{1} << position)};
    return And(kMask);
  }

 private:
  static volatile T& Ref() { return *reinterpret_cast<volatile T*>(memory::kIo + offset_from_io); }

  static Derived& Self() {
    static Derived instance{};
    return instance;
  }
};

}  // namespace toncar
