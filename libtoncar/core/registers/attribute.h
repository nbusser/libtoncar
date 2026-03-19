#pragma once

#include <cstdint>

#include "panic.h"

namespace toncar {

/// Similar to `Register`, but owns its value as a plain member instead of
/// mapping to a volatile IO address. Intended for structs that are cast
/// directly from memory (e.g. OAM entries), where adding an address member
/// would corrupt the memory layout.
template <class Derived, typename T>
class Attribute {
 public:
  Attribute() = delete;

 protected:
  [[nodiscard]] T Get() const { return value_; }

  [[nodiscard]] T GetOr(T val) const { return value_ | val; }
  [[nodiscard]] T GetAnd(T val) const { return value_ & val; }

  Derived& Set(T new_value) {
    value_ = new_value;
    return static_cast<Derived&>(*this);
  }

  Derived& Or(T val) { return Set(value_ | val); }
  Derived& And(T val) { return Set(value_ & val); }
  Derived& Xor(T val) { return Set(value_ ^ val); }

  template <uint8_t position>
  [[nodiscard]] bool HasBit() const {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return (Get() & kMask) == kMask;
  }

  template <uint8_t position>
  Derived& SetBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return Or(kMask);
  }

  template <uint8_t position>
  Derived& ClearBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{static_cast<T>(~(T{1} << position))};
    return And(kMask);
  }

  template <uint8_t position>
  Derived& ToggleBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return Xor(kMask);
  }

  // Span accessor
  template <uint8_t n_bits, uint8_t bit_position>
  [[nodiscard]] T GetSpan() const {
    static_assert(n_bits <= sizeof(T) * 8);
    static_assert(bit_position <= (sizeof(T) * 8) - n_bits);
    constexpr T kMask{((1 << n_bits) - 1) << bit_position};
    return GetAnd(kMask) >> bit_position;
  }

  template <uint8_t n_bits, uint8_t bit_position, uint16_t max_value = (1 << n_bits) - 1>
  Derived& SetSpan(T val) {
    static_assert(n_bits <= sizeof(T) * 8);
    static_assert(bit_position <= (sizeof(T) * 8) - n_bits);
    MGBA_ASSERT(val <= max_value);
    constexpr T kMask{((1 << n_bits) - 1) << bit_position};
    return And(~kMask).Or(val << bit_position);
  }

 private:
  T value_{};
};

}  // namespace toncar
