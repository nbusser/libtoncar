#include "interrupt.h"

namespace toncar {

// NOLINTNEXTLINE(*-avoid-non-const-global-variables): the pointer itself is immutable
IrqRec isr_table[kInterruptionMax + 1];

}  // namespace toncar
