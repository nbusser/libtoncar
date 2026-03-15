#pragma once

#include <cstdint>

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

 private:
  T value_{};
};

}  // namespace toncar
