#pragma once

#include <cstdint>

#include "asm/attributes.h"

// NOLINTBEGIN(readability-identifier-naming): use C naming conventions

namespace toncar {

// These functions are defined in ARM assembly, so extern C is mandatory here to avoid C++ name
// mangling.
extern "C" {
///	Fastfill for halfwords, analogous to memset()
///	Uses `memset32()` if `hwcount>5`
/// @param dst	Destination address.
/// @param hw	Source halfword (not address).
/// @param hwcount	Number of halfwords to fill.
/// @note `dst` **must** be halfword aligned.
/// @note `r0` returns as `dst + hwcount*2`.
void memset16(void* dst, uint16_t hw, uint32_t hwcount);

///	Fast-fill by words, analogous to memset()
/// Like CpuFastSet(), only without the requirement of 32byte chunks and no awkward
/// store-value-in-memory-first issue.
/// @param `dst` Destination address.
/// @param `wd` Fill word (not address).
/// @param `wdcount` Number of words to fill.
/// @note `dst` <b>must</b> be word aligned.
/// @note `r0` returns as `dst` + `wdcount*4`.
TONCAR_IWRAM_CODE void memset32(void* dst, uint32_t wd, uint32_t wdcount);
}

// NOLINTEND(readability-identifier-naming)

}  // namespace toncar
