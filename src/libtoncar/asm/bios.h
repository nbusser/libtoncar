#pragma once

namespace toncar {

// These functions are defined in ARM assembly, so extern C is mandatory here to avoid C++ name
// mangling.
extern "C" {

/// Waits for the next VBlank interrupt.
/// Make sure to enable VBlank interrupts via the `InterruptManager`.
void VBlankIntrWait();
}

}  // namespace toncar
