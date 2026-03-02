#pragma once

/// Put function in IWRAM.
#ifndef TONCAR_IWRAM_CODE
#define TONCAR_IWRAM_CODE __attribute__((section(".iwram"), long_call))
#endif
