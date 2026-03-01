#pragma once

#include <panic.h>
#include <toncar.h>

#include <concepts>
#include <cstdint>

namespace toncar {

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class RegisterBase;

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class Register : public RegisterBase<Derived, T, offset_from_io> {
 protected:
  static Derived& Set(T val) {
    Base::Ref() = val;
    return Base::Self();
  }

  static Derived& Or(T val) { return Set(Base::GetOr(val)); }
  static Derived& And(T val) { return Set(Base::GetAnd(val)); }

 private:
  using Base = RegisterBase<Derived, T, offset_from_io>;
};

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class TransactionRegister : public RegisterBase<Derived, T, offset_from_io> {
 protected:
  static Derived& Reset(bool force_read = false) {
    staged_value = force_read ? Base::Ref() : last_commit_value;
    return Base::Self();
  }

  static Derived& Commit() {
    Base::Ref() = staged_value;
    last_commit_value = staged_value;
    return Base::Self();
  }

  static Derived& Set(T val) {
    staged_value = val;
    return Base::Self();
  }

  static Derived& Or(T val) { return Set(staged_value | val); }
  static Derived& And(T val) { return Set(staged_value & val); }

 private:
  using Base = RegisterBase<Derived, T, offset_from_io>;

  // WARNING: some registers might be read-only and might break the API if I init using `Ref()`.
  inline static T staged_value{0};
  inline static T last_commit_value{staged_value};
};

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class RegisterBase {
  friend class Register<Derived, T, offset_from_io>;
  friend class TransactionRegister<Derived, T, offset_from_io>;

 protected:
  static T Get() { return Ref(); }

  static T GetOr(T val) { return Ref() | val; }
  static T GetAnd(T val) { return Ref() & val; }

  template <uint8_t position>
  static bool HasBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return (Get() & kMask) == kMask;
  }

  static Derived& Set(T /*val*/) {
    GBA_ASSERT(false);
    return Self();
  }

  static Derived& Or(T /*val*/) {
    GBA_ASSERT(false);
    return Self();
  }
  static Derived& And(T /*val*/) {
    GBA_ASSERT(false);
    return Self();
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
