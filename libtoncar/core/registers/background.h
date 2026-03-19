#pragma once

#include <registers/registers.h>

#include <cstdint>

#include "libtoncar/registers/registers.h"

namespace toncar {

template <uint8_t N>
  requires(N < 4)
class BackgroundControl : public Register<BackgroundControl<N>, uint16_t, 0x00000008 + (2 * N)> {
 public:
  /// Pr: bits 0-1
  [[nodiscard]] uint16_t GetPriority() const { return this->template GetSpan<2, 0>(); }
  BackgroundControl<N> SetPriority(uint16_t priority) const {
    return this->template SetSpan<2, 0>(priority);
  }

  /// CBB: bits 2-3
  [[nodiscard]] uint16_t GetCharacterBaseBlock() const { return this->template GetSpan<2, 2>(); }
  BackgroundControl<N> SetCharacterBaseBlock(uint16_t cbb) const {
    return this->template SetSpan<2, 2>(cbb);
  }

  /// Mos: bit 6
  [[nodiscard]] bool HasMosaic() { return this->template HasBit<6>(); }
  BackgroundControl<N> EnableMosaic() { return this->template SetBit<6>(); }
  BackgroundControl<N> DisableMosaic() { return this->template ClearBit<6>(); }
  BackgroundControl<N> ToggleMosaic() { return this->template ToggleBit<6>(); }

  /// CM: bit 7
  [[nodiscard]] bool IsColorMode4bpp() { return !this->template HasBit<6>(); }
  BackgroundControl<N> SetColorMode8bpp() { return this->template SetBit<6>(); }
  BackgroundControl<N> SetColorMode4bpp() { return this->template ClearBit<6>(); }

  /// SBB: bits 8-12
  [[nodiscard]] uint16_t GetScreenBaseBlock() const { return this->template GetSpan<4, 8>(); }
  BackgroundControl<N> SetScreenBaseBlock(uint16_t cbb) const {
    return this->template SetSpan<4, 8>(cbb);
  }

  /// Wr: bit 13
  [[nodiscard]] bool HasAffineWrap() { return this->template HasBit<13>(); }
  BackgroundControl<N> EnableAffineWrap() { return this->template SetBit<13>(); }
  BackgroundControl<N> DisableAffineWrap() { return this->template ClearBit<13>(); }
  BackgroundControl<N> ToggleAffineWrap() { return this->template ToggleBit<13>(); }

  /// Sz: bits 14-15
  enum class Size : uint8_t {
    Reg32x32 = 0b00,
    Reg64x32 = 0b01,
    Reg32x64 = 0b10,
    Reg64x64 = 0b11,
    Aff16x16 = 0b00,
    Aff32x32 = 0b01,
    Aff64x64 = 0b10,
    Aff128x128 = 0b11,
  };
  [[nodiscard]] Size GetSize() const { return this->template GetSpan<2, 14>(); }
  BackgroundControl<N> SetSize(Size size) const {
    return this->template SetSpan<2, 14>(static_cast<uint16_t>(size));
  }
};

/// REG_BGxHOFS: Background horizontal offset register.
/// Write-only register.
template <uint8_t N>
  requires(N < 4)
class BackgroundHorizontalOffset
    : public Register<BackgroundHorizontalOffset<N>, uint16_t, 0x00000010 + (4 * N)> {
 public:
  BackgroundHorizontalOffset<N>& SetOffset(uint16_t offset) { return this->Set(offset); }
};

/// REG_BGxVOFS: Background vertical offset register.
/// Write-only register.
template <uint8_t N>
  requires(N < 4)
class BackgroundVerticalOffset
    : public Register<BackgroundVerticalOffset<N>, uint16_t, 0x00000012 + (4 * N)> {
 public:
  BackgroundVerticalOffset<N>& SetOffset(uint16_t offset) { return this->Set(offset); }
};

}  // namespace toncar
