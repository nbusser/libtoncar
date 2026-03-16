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
    uint16_t GetY() { return GetAnd(0x00FF); }
    ObjAttr0& SetY(uint16_t new_y) {
      MGBA_ASSERT(new_y <= 0xFF);
      return And(0xFF00).Or(new_y);
    }

    enum class OM : uint8_t {
      NormalRendering = 0b00,
      AffineRendering = 0b01,
      Hide = 0b11,
      AffineDoubleRendering = 0b11
    };
    OM GetOm() { return static_cast<OM>(GetAnd(kOmMask) >> kOmShift); }
    ObjAttr0& SetOm(OM om) {
      return And(static_cast<uint16_t>(~kOmMask))
          .Or(static_cast<uint16_t>(static_cast<uint16_t>(om) << kOmShift));
    }

    enum class GM : uint8_t { NormalRendering = 0b00, AlphaBlending = 0b01, ObjectWindow = 0b10 };
    GM GetGm() { return static_cast<GM>(GetAnd(kGmMask) >> kGmShift); }
    ObjAttr0& SetGm(GM gm) {
      return And(static_cast<uint16_t>(~kGmMask))
          .Or(static_cast<uint16_t>(static_cast<uint16_t>(gm) << kGmShift));
    }

    bool HasMosaic() { return HasBit<12>(); }
    ObjAttr0& SetMosaic() { return SetBit<12>(); }
    ObjAttr0& ClearMosaic() { return ClearBit<12>(); }

    bool IsColorMode4bpp() { return HasBit<13>(); }
    ObjAttr0& SetColorMode8bpp() { return SetBit<13>(); }
    ObjAttr0& SetColorMode4bpp() { return ClearBit<13>(); }

    enum class Shape : uint8_t { Square = 0b00, Wide = 0b01, Tall = 0b10 };
    Shape GetShape() { return static_cast<Shape>(GetAnd(kShapeMask) >> kShapeShift); }
    ObjAttr0& SetShape(Shape shape) {
      return And(static_cast<uint16_t>(~kShapeMask))
          .Or(static_cast<uint16_t>(static_cast<uint16_t>(shape) << kShapeShift));
    }

   private:
    /// OM: 8-9 bits
    static constexpr uint16_t kOmMask{0b0000001100000000};
    static constexpr uint8_t kOmShift{8};

    /// GM: 10-11 bits
    static constexpr uint16_t kGmMask{0b0000110000000000};
    static constexpr uint8_t kGmShift{10};

    /// Shape: 14-15 bits
    static constexpr uint16_t kShapeMask{0b1100000000000000};
    static constexpr uint8_t kShapeShift{14};
  };

  class ObjAttr1 : public Attribute<ObjAttr1, uint16_t> {
   public:
    uint16_t GetX() { return GetAnd(kXMask) >> kXShift; }
    ObjAttr1& SetX(uint16_t new_x) {
      MGBA_ASSERT(new_x <= kXMask >> kXShift);
      return And(static_cast<uint16_t>(~kXMask)).Or(static_cast<uint16_t>(new_x << kXShift));
    }

    /// Only valid if attr0{8} is set.
    uint16_t GetAffineIndex() { return GetAnd(kAidMask) >> kAidShift; }
    ObjAttr1& SetAffineIndex(uint16_t new_aid) {
      MGBA_ASSERT(new_aid <= kAidMask >> kAidShift);
      return And(static_cast<uint16_t>(~kAidMask)).Or(static_cast<uint16_t>(new_aid << kAidShift));
    }

    /// Only valid if attr0{8} is unset.
    bool IsHorizontalFlip() { return HasBit<12>(); }
    ObjAttr1& SetHorizontalFlip() { return SetBit<12>(); }
    ObjAttr1& ClearHorizontalFlip() { return ClearBit<12>(); }

    /// Only valid if attr0{8} is unset.
    bool IsVerticalFlip() { return HasBit<13>(); }
    ObjAttr1& SetVerticalFlip() { return SetBit<13>(); }
    ObjAttr1& ClearVerticalFlip() { return ClearBit<13>(); }

    enum class Size : uint8_t { Tiny = 0b00, Small = 0b01, Medium = 0b10, Large = 0b11 };
    Size GetSize() { return static_cast<Size>(GetAnd(kSizeMask) >> kSizeShift); }
    ObjAttr1& SetSize(Size size) {
      return And(static_cast<uint16_t>(~kSizeMask))
          .Or(static_cast<uint16_t>(static_cast<uint16_t>(size) << kSizeShift));
    }

   private:
    /// X: 0-8 bits.
    static constexpr uint16_t kXMask{0b0000000111111111};
    static constexpr uint8_t kXShift{0};

    /// AID: 9-13 bits.
    /// Only valid if attr0{8} is set.
    static constexpr uint16_t kAidMask{0b0011111000000000};
    static constexpr uint8_t kAidShift{9};

    /// Size: 14-15 bits
    static constexpr uint16_t kSizeMask{0b1100000000000000};
    static constexpr uint8_t kSizeShift{14};
  };

  class ObjAttr2 : public Attribute<ObjAttr2, uint16_t> {
   public:
    uint16_t GetTileId() { return GetAnd(kTileId) >> kTileIdShift; }
    ObjAttr2& SetTileId(uint16_t new_tile_id) {
      MGBA_ASSERT(new_tile_id <= kTileId >> kTileIdShift);
      return And(static_cast<uint16_t>(~kTileIdShift))
          .Or(static_cast<uint16_t>(new_tile_id << kTileIdShift));
    }

    uint16_t GetPrio() { return GetAnd(kPrioMask) >> kPrioShift; }
    ObjAttr2& SetPrio(uint16_t new_prio) {
      MGBA_ASSERT(new_prio <= kPrioMask >> kPrioShift);
      return And(static_cast<uint16_t>(~kPrioShift))
          .Or(static_cast<uint16_t>(new_prio << kPrioShift));
    }

    uint16_t Get4bppPalbank() { return GetAnd(kPalbankMask) >> kPalbankShift; }
    ObjAttr2& Set4bppPalbank(uint16_t new_palbank) {
      MGBA_ASSERT(new_palbank <= kPalbankMask >> kPalbankShift);
      return And(static_cast<uint16_t>(~kPalbankShift))
          .Or(static_cast<uint16_t>(new_palbank << kPalbankShift));
    }

   private:
    /// TID: 0-9 bits.
    static constexpr uint16_t kTileId{0b0000001111111111};
    static constexpr uint8_t kTileIdShift{0};

    /// Prio: 10-11 bits.
    static constexpr uint16_t kPrioMask{0b0000110000000000};
    static constexpr uint8_t kPrioShift{10};

    /// Palbank: 12-15 bits.
    static constexpr uint16_t kPalbankMask{0b1111000000000000};
    static constexpr uint8_t kPalbankShift{12};
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

  ObjAttr& GetSpriteAttributes(uint8_t index) {
    MGBA_ASSERT(index <= 128);
    return *const_cast<ObjAttr*>(base_address + index);
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
