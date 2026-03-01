#pragma once

#include <cstdint>

namespace toncar {

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

}  // namespace toncar
