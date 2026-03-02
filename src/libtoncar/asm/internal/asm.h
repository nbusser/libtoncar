/// Header file with goodies for assembly.
///
/// Author: J Vijn
/// Date: 20081019 - 20120519

#pragma once

// NOLINTBEGIN(readability-identifier-naming, cppcoreguidelines-macro-to-enum,
// bugprone-macro-parentheses): Critical code from libtonc

// Warning: This header file is for use in assembly only!

// --------------------------------------------------------------------
// MACROS
// --------------------------------------------------------------------

#define DEF_SIZE(_name) .size _name, .- _name

/// \name Section definitions for assembly.
//\{

#define CSEC_TEXT .text                              ///< Standard code section directive.
#define CSEC_EWRAM .section.ewram, "ax", % progbits  ///< EWRAM code section directive.
#define CSEC_IWRAM .section.iwram, "ax", % progbits  ///< IWRAM code section directive.

#define DSEC_DATA .data           //<! Standard data section directive.
#define DSEC_ROM .section.rodata  ///< ROM data section directive.
#define DSEC_BSS .section.bss     ///< Uninited data (RAM) section directive.
#define DSEC_SBSS .section.sbss   ///< Uninited data (DTCM?) section directive.

#define ARM_FUNC .arm           ///< Indicates an ARM function.
#define THUMB_FUNC .thumb_func  ///< Indicates a Thumb function.

// # NOTE: because these use commas, I can't pass them through CPP macros.
// # Yes, this is stupid, but do you have a better idea?

// Clang format totally breaks this macro.
// clang-format off
#undef CSEC_EWRAM
	.macro CSEC_EWRAM
	.section .ewram , "ax", %progbits
	.endm

#undef CSEC_IWRAM
	.macro CSEC_IWRAM
	.section .iwram , "ax", %progbits
	.endm
// clang-format on

//\}

/// \name Function definition macros.
//\{

/// Start an assembly function.
/*!
        \param _name	Name of function.
        \param _section	Section to place function in (like .text)
*/
#define BEGIN_FUNC(_name, _section, _iset) \
  _section;                                \
  _iset;                                   \
  .align 2;                                \
  .global _name;                           \
  .type _name STT_FUNC;                    \
  _name:

/// End of a function.
#define END_FUNC(_name) DEF_SIZE(_name)

/// Begin an ARM function
/*!
        \param _name	Name of function.
        \param _section	Section to place function in (like .text)
*/
#define BEGIN_FUNC_ARM(_name, _section) BEGIN_FUNC(_name, _section, ARM_FUNC)

/// Begin a THUMB function.
/*!
        \param _name	Name of function.
        \param _section	Section to place function in (like .text)
*/
#define BEGIN_FUNC_THUMB(_name, _section) BEGIN_FUNC(_name, _section, THUMB_FUNC)
//\}

/// \name Data definition macros.
//\{
#define BEGIN_SYMBOL(_name, _section) \
  _section;                           \
  .align;                             \
  .global _name;                      \
  _name:

#define END_SYMBOL(_name) DEF_SIZE(_name)
//\}

// --------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------

#define TSRF_data 0
#define TSRF_pitch 4
#define TSRF_width 8
#define TSRF_height 10
#define TSRF_bpp 12
#define TSRF_type 13
#define TSRF_palSize 14
#define TSRF_pal 16

             // NOLINTEND(readability-identifier-naming, cppcoreguidelines-macro-to-enum,
             // bugprone-macro-parentheses)
