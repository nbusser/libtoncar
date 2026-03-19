#pragma once

#include <mgba/logger.h>
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
  Derived& Set(T val) {
    Base::Ref() = val;
    return static_cast<Derived&>(*this);
  }

  Derived& Or(T val) { return Set(Base::GetOr(val)); }
  Derived& And(T val) { return Set(Base::GetAnd(val)); }
  Derived& Xor(T val) { return Set(Base::GetXor(val)); }

 private:
  using Base = RegisterBase<Derived, T, offset_from_io>;
  friend Base;
};

template <typename Derived,
          std::unsigned_integral T,
          uint32_t offset_from_io,
          T default_value = T{}>
class TransactionRegister : public RegisterBase<Derived, T, offset_from_io> {
 protected:
  Derived& Commit() {
    Base::Ref() = staged_value_;
    last_commit_value_ = staged_value_;
    return static_cast<Derived&>(*this);
  }

  Derived& Abort() {
    staged_value_ = last_commit_value_;
    return static_cast<Derived&>(*this);
  }

  Derived& Set(T val) {
    staged_value_ = val;
    return static_cast<Derived&>(*this);
  }

  Derived& Or(T val) { return Set(staged_value_ | val); }
  Derived& And(T val) { return Set(staged_value_ & val); }
  Derived& Xor(T val) { return Set(staged_value_ ^ val); }

 public:
  TransactionRegister() : staged_value_{default_value}, last_commit_value_{default_value} {}

 private:
  using Base = RegisterBase<Derived, T, offset_from_io>;
  friend Base;

  T staged_value_;
  T last_commit_value_;
};

// The subclasses must define these methods:
// Derived& Set(T /*val*/);
// Derived& Or(T /*val*/);
// Derived& And(T /*val*/);
// Derived& Xor(T /*val*/);
template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io>
class RegisterBase {
  friend class Register<Derived, T, offset_from_io>;
  friend class TransactionRegister<Derived, T, offset_from_io>;

 public:
  static Derived& Instance() {
    static Derived instance{};
    return instance;
  }

 protected:
  [[nodiscard]] T Get() const { return CRef(); }

  [[nodiscard]] T GetOr(T val) const { return CRef() | val; }
  [[nodiscard]] T GetAnd(T val) const { return CRef() & val; }
  [[nodiscard]] T GetXor(T val) const { return CRef() ^ val; }

  // Single bit accessors
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
    return static_cast<Derived&>(*this).Or(kMask);
  }

  template <uint8_t position>
  Derived& ClearBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{static_cast<T>(~(T{1} << position))};
    return static_cast<Derived&>(*this).And(kMask);
  }

  template <uint8_t position>
  Derived& ToggleBit() {
    static_assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return static_cast<Derived&>(*this).Xor(kMask);
  }

  // Span accessor
  template <uint8_t n_bits, uint8_t bit_position>
  [[nodiscard]] T GetSpan() const {
    static_assert(n_bits <= sizeof(T) * 8);
    static_assert(bit_position <= (sizeof(T) * 8) - n_bits);
    constexpr T kMask{((1 << n_bits) - 1) << bit_position};
    return GetAnd(kMask) >> bit_position;
  }

  template <uint8_t n_bits, uint8_t bit_position>
  [[nodiscard]] Derived& SetSpan(T val) const {
    static_assert(n_bits <= sizeof(T) * 8);
    static_assert(bit_position <= (sizeof(T) * 8) - n_bits);
    constexpr T kMask{((1 << n_bits) - 1) << bit_position};
    return static_cast<Derived&>(*this).And(~kMask).Or((val << bit_position) & kMask);
  }

 private:
  volatile T& Ref() { return *reinterpret_cast<volatile T*>(memory::kIo + offset_from_io); }
  volatile T& CRef() const { return *reinterpret_cast<volatile T*>(memory::kIo + offset_from_io); }
};

}  // namespace toncar
