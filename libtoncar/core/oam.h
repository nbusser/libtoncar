#pragma once

#include <registers/attribute.h>

#include <cstdint>

#include "libtoncar/core/panic.h"
#include "libtoncar/core/toncar.h"

namespace toncar {

class Oam final {
 public:
  class ObjAttr0 : public Attribute<ObjAttr0, uint16_t> {
   public:
    // Y: bits 0-7
    [[nodiscard]] uint16_t GetY() const { return GetSpan<8, 0>(); }
    ObjAttr0& SetY(uint16_t new_y) { return SetSpan<8, 0>(new_y); }

    /// OM: bits 8-9
    enum class OM : uint8_t {
      NormalRendering = 0b00,
      AffineRendering = 0b01,
      Hide = 0b11,
      AffineDoubleRendering = 0b11
    };
    [[nodiscard]] OM GetOm() const { return static_cast<OM>(GetSpan<2, 8>()); }
    ObjAttr0& SetOm(OM om) { return SetSpan<2, 8>(static_cast<uint16_t>(om)); }

    /// GM: bits 10-11
    enum class GM : uint8_t { NormalRendering = 0b00, AlphaBlending = 0b01, ObjectWindow = 0b10 };
    [[nodiscard]] GM GetGm() const { return static_cast<GM>(GetSpan<2, 10>()); }
    ObjAttr0& SetGm(GM gm) { return SetSpan<2, 10>(static_cast<uint16_t>(gm)); }

    [[nodiscard]] bool HasMosaic() const { return HasBit<12>(); }
    ObjAttr0& SetMosaic() { return SetBit<12>(); }
    ObjAttr0& ClearMosaic() { return ClearBit<12>(); }

    [[nodiscard]] bool IsColorMode4bpp() const { return !HasBit<13>(); }
    ObjAttr0& SetColorMode8bpp() { return SetBit<13>(); }
    ObjAttr0& SetColorMode4bpp() { return ClearBit<13>(); }

    /// Shape: bits 14-15
    enum class Shape : uint8_t { Square = 0b00, Wide = 0b01, Tall = 0b10 };
    [[nodiscard]] Shape GetShape() const { return static_cast<Shape>(GetSpan<2, 14>()); }
    ObjAttr0& SetShape(Shape shape) { return SetSpan<2, 14>(static_cast<uint16_t>(shape)); }
  };

  class ObjAttr1 : public Attribute<ObjAttr1, uint16_t> {
   public:
    /// X: bits 0-8
    [[nodiscard]] uint16_t GetX() const { return GetSpan<9, 0>(); }
    ObjAttr1& SetX(uint16_t new_x) { return SetSpan<9, 0>(new_x); }

    /// AID: bits 9-13
    /// Only valid if attr0{8} is set.
    [[nodiscard]] uint16_t GetAffineIndex() const { return GetSpan<5, 9>(); }
    ObjAttr1& SetAffineIndex(uint16_t new_aid) { return SetSpan<5, 9>(new_aid); }

    /// Only valid if attr0{8} is unset.
    [[nodiscard]] bool IsHorizontalFlip() const { return HasBit<12>(); }
    ObjAttr1& SetHorizontalFlip() { return SetBit<12>(); }
    ObjAttr1& ClearHorizontalFlip() { return ClearBit<12>(); }
    ObjAttr1& ToggleHorizontalFlip() { return ToggleBit<12>(); }

    /// Only valid if attr0{8} is unset.
    [[nodiscard]] bool IsVerticalFlip() const { return HasBit<13>(); }
    ObjAttr1& SetVerticalFlip() { return SetBit<13>(); }
    ObjAttr1& ClearVerticalFlip() { return ClearBit<13>(); }
    ObjAttr1& ToggleVerticalFlip() { return ToggleBit<13>(); }

    /// Size: bits 14-15
    enum class Size : uint8_t { Tiny = 0b00, Small = 0b01, Medium = 0b10, Large = 0b11 };
    Size GetSize() { return static_cast<Size>(GetSpan<2, 14>()); }
    ObjAttr1& SetSize(Size size) { return SetSpan<2, 14>(static_cast<uint16_t>(size)); }
  };

  class ObjAttr2 : public Attribute<ObjAttr2, uint16_t> {
   public:
    uint16_t GetTileId() { return GetSpan<10, 0>(); }
    ObjAttr2& SetTileId(uint16_t new_tile_id) { return SetSpan<10, 0>(new_tile_id); }

    uint16_t GetPrio() { return GetSpan<2, 10>(); }
    ObjAttr2& SetPrio(uint16_t new_prio) { return SetSpan<2, 10>(new_prio); }

    uint16_t Get4bppPalbank() { return GetSpan<4, 12>(); }
    ObjAttr2& Set4bppPalbank(uint16_t new_palbank) { return SetSpan<4, 12>(new_palbank); }
  };

  static_assert(sizeof(ObjAttr0) == sizeof(uint16_t));
  static_assert(sizeof(ObjAttr1) == sizeof(uint16_t));
  static_assert(sizeof(ObjAttr2) == sizeof(uint16_t));

  struct ObjAttr {
    ObjAttr0 attr0;
    ObjAttr1 attr1;
    ObjAttr2 attr2;
    uint16_t filler = 0;
  };

  static Oam& Instance() {
    static Oam oam{};
    return oam;
  }

  ObjAttr& RefSpriteAttributes(uint8_t index) {
    MGBA_ASSERT(index <= 128);
    return *const_cast<ObjAttr*>(base_address + index);
  }

  const ObjAttr& CRefSpriteAttributes(uint8_t index) {
    MGBA_ASSERT(index <= 128);
    return *const_cast<const ObjAttr*>(base_address + index);
  }

  Oam& SetSpriteAttributes(uint8_t index, ObjAttr attributes) {
    MGBA_ASSERT(index <= 128);
    *const_cast<ObjAttr*>(base_address + index) = attributes;
    return *this;
  }

 private:
  Oam() = default;

  inline static volatile ObjAttr* base_address = reinterpret_cast<volatile ObjAttr*>(memory::kOam);
};

}  // namespace toncar
