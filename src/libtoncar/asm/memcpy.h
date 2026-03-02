#pragma once

// NOLINTBEGIN

#include <cstdint>

#include "asm/attributes.h"

namespace toncar {

// NOLINTBEGIN(readability-identifier-naming): use C naming conventions

// These functions are defined in ARM assembly, so extern C is mandatory here to avoid C++ name
// mangling.
extern "C" {
///	Fastfill for halfwords, analogous to `memset()`
///	Uses `memset32()` if `hwcount > 5`
/// @param dst	Destination address.
/// @param hw	Source halfword (not address).
/// @param hwcount	Number of halfwords to fill.
/// @note `dst` **must** be halfword aligned.
/// @note `r0` returns as `dst + hwcount*2`.
void memset16(void* dst, uint16_t hw, uint32_t hwcount);

/// Copy for halfwords.
/// Uses `memcpy32()` if `hwn > 6` and `src` and `dst` are aligned equally.
/// @param `dst` Destination address.
/// @param `src` Source address.
/// @param `hwcount`. Number of halfwords to fill.
/// @note `dst` and `src` **must** be halfword aligned.
/// @note `r0` and `r1` return as `dst + hwcount*2` and `src + hwcount*2`.
void memcpy16(void* dst, const void* src, uint32_t hwcount);

///	Fast-fill by words, analogous to `memset()`
/// Like CpuFastSet(), only without the requirement of 32byte chunks and no awkward
/// store-value-in-memory-first issue.
/// @param `dst` Destination address.
/// @param `wd` Fill word (not address).
/// @param `wdcount` Number of words to fill.
/// @note `dst` <b>must</b> be word aligned.
/// @note `r0` returns as `dst` + `wdcount*4`.
TONCAR_IWRAM_CODE void memset32(void* dst, uint32_t wd, uint32_t wdcount);

///	Fast-copy by words.
/// Like CpuFastFill(), only without the requirement of 32byte chunks
/// @param `dst` Destination address.
/// @param `src` Source address.
/// @param `wdcount` Number of words.
/// @note `src` and `dst` <b>must</b> be word aligned.
/// @note `r0` and `r1` return as `dst + wdcount*4` and `src + wdcount*4`.
TONCAR_IWRAM_CODE void memcpy32(void* dst, const void* src, uint32_t wdcount);
}

}  // namespace toncar

// NOLINTEND(readability-identifier-naming))
