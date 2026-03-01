#pragma once

#include <mgba/logger.h>
#include <panic.h>
#include <toncar.h>

#include <cassert>
#include <concepts>
#include <cstdint>

namespace toncar {

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io, T default_value>
class RegisterBase;

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io, T default_value>
class Register : public RegisterBase<Derived, T, offset_from_io, default_value> {
 protected:
  Derived& Set(T val) {
    Base::Ref() = val;
    return static_cast<Derived&>(*this);
  }

  Derived& Or(T val) { return Set(Base::GetOr(val)); }
  Derived& And(T val) { return Set(Base::GetAnd(val)); }

 private:
  using Base = RegisterBase<Derived, T, offset_from_io, default_value>;
};

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io, T default_value>
class TransactionRegister : public RegisterBase<Derived, T, offset_from_io, default_value> {
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

 public:
  TransactionRegister() : staged_value_{default_value}, last_commit_value_{default_value} {}

 private:
  using Base = RegisterBase<Derived, T, offset_from_io, default_value>;

  T staged_value_;
  T last_commit_value_;
};

template <typename Derived, std::unsigned_integral T, uint32_t offset_from_io, T default_value>
class RegisterBase {
  friend class Register<Derived, T, offset_from_io, default_value>;
  friend class TransactionRegister<Derived, T, offset_from_io, default_value>;

 public:
  static Derived& Instance() {
    static Derived instance{};
    return instance;
  }

 protected:
  T Get() { return Ref(); }

  T GetOr(T val) { return Ref() | val; }
  T GetAnd(T val) { return Ref() & val; }

  template <uint8_t position>
  bool HasBit() {
    assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return (Get() & kMask) == kMask;
  }

  Derived& Set(T /*val*/) {
    GBA_ASSERT(false);
    return static_cast<Derived&>(*this);
  }

  Derived& Or(T /*val*/) {
    GBA_ASSERT(false);
    return static_cast<Derived&>(*this);
  }
  Derived& And(T /*val*/) {
    GBA_ASSERT(false);
    return static_cast<Derived&>(*this);
  }

  template <uint8_t position>
  Derived& SetBit() {
    assert(position < sizeof(T) * 8);
    constexpr T kMask{T{1} << position};
    return Or(kMask);
  }

  template <uint8_t position>
  Derived& ClearBit() {
    assert(position < sizeof(T) * 8);
    constexpr T kMask{~(T{1} << position)};
    return And(kMask);
  }

  Derived& Reset() { return Set(default_value); }

 private:
  volatile T& Ref() { return *reinterpret_cast<volatile T*>(memory::kIo + offset_from_io); }
};

}  // namespace toncar
