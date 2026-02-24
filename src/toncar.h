#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace libtoncar {

namespace memory {
inline constexpr uintptr_t kIo{0x04000000};
inline constexpr uintptr_t kVram{0x06000000};
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

class Dispcnt : public Register<Dispcnt, uint16_t, memory::kIo> {
 public:
  enum class Mode : uint16_t {
    DcntMode0 = 0x0000,
    DcntMode1 = 0x0001,
    DcntMode2 = 0x0002,
    DcntMode3 = 0x0003,
    DcntMode4 = 0x0004,
    DcntMode5 = 0x0005
  };

  enum class Layer : uint16_t {
    DcntBg0 = 1 << 8,   // 0x0100
    DcntBg1 = 1 << 9,   // 0x0200
    DcntBg2 = 1 << 10,  // 0x0400
    DcntBg3 = 1 << 11,  // 0x0800
    DcntObj = 1 << 12   // 0x1000
  };

  static Dispcnt& SetLayer(Layer layer) { return Or(static_cast<uint16_t>(layer)); }

  static Dispcnt& ClearLayer(Layer layer) { return And(~static_cast<uint16_t>(layer)); }

  static Dispcnt& FlushLayers() { return And(0x0007); }

  static bool HasLayer(Layer layer) { return GetAnd(static_cast<uint16_t>(layer)); }

  static Mode GetMode() { return static_cast<Mode>(GetAnd(0x0007)); }
  static Dispcnt& SetMode(Mode mode) {
    return And(static_cast<uint16_t>(~0x0007)).Or(static_cast<uint16_t>(mode));
  }

  static Dispcnt& Reset() {
    return FlushLayers().SetMode(Mode::DcntMode0).SetLayer(Layer::DcntBg0);
  }
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
  constexpr Color15(uint8_t red, uint8_t green, uint8_t blue)
      : value_{static_cast<uint16_t>(red | (green << 5) | (blue << 10))} {
    assert(red < 32);
    assert(green < 32);
    assert(blue < 32);
  }

  constexpr uint16_t Value() const { return value_; }

  constexpr uint8_t Red() const { return value_ & 31; }
  constexpr uint8_t Green() const { return static_cast<uint8_t>((value_ >> 5) & 31); }
  constexpr uint8_t Blue() const { return static_cast<uint8_t>((value_ >> 10) & 31); }

 private:
  const uint16_t value_;
};

class Screen : public Zone<Screen, uint16_t, memory::kVram> {
 public:
  static Screen& WritePixel(uint8_t x, uint8_t y, Color15 color) {
    return Set(y * kWidth + x, color.Value());
  }

  static constexpr uint32_t kWidth{240};
  static constexpr uint32_t kHeight{160};
};

}  // namespace libtoncar
