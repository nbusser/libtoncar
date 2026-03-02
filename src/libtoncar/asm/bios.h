#pragma once

namespace toncar {

extern "C" {
// These functions are defined in ARM assembly, so extern C is mandatory here to avoid C++ name
// mangling.
void VBlankIntrWait();
}

}  // namespace toncar
