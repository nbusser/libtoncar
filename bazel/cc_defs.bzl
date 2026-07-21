load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//bazel/gba_platform:transition.bzl", "gba_wrap")

# Base C options
_COPTS_BASE = [
    "-std=gnu11",
]

_COPTS_WARNING = [
    "-Wall",
    "-Wdouble-promotion",
    "-Werror=implicit-function-declaration",
    "-Wextra",
    "-Winit-self",
    "-Wmissing-prototypes",
    "-Wno-format-zero-length",
    "-Wstrict-prototypes",
    "-Wpointer-arith",
    "-Wwrite-strings",
]

# Internal C compilation options. Use this by default for all C targets in the repo.
DEFAULT_COPTS = (
    _COPTS_BASE +
    select({
        "//bazel:strict_false": _COPTS_WARNING,
        "//bazel:strict_true": _COPTS_WARNING + ["-Werror"],
    })
)

_CXXOPTS_BASE = [
    "-std=gnu++23",
]

_CXXOPTS_WARNING = [
    "-fno-exceptions",
    "-fno-threadsafe-statics",
    "-fno-unwind-tables",
    "-fno-asynchronous-unwind-tables",
    "-fno-rtti",
    "-Wall",
    "-Wcast-align",
    "-Wcast-qual",
    "-Wconversion",
    "-Wdouble-promotion",
    "-Wduplicated-branches",
    "-Wduplicated-cond",
    "-Wextra",
    "-Wextra-semi",
    "-Wfloat-equal",
    "-Wformat=2",
    "-Wimplicit-fallthrough",
    "-Winit-self",
    "-Wlogical-op",
    "-Wmissing-declarations",
    "-Wnon-virtual-dtor",
    "-Wnonnull",
    "-Wnull-dereference",
    "-Wold-style-cast",
    "-Woverloaded-virtual",
    "-Wpedantic",
    "-Wpointer-arith",
    "-Wredundant-decls",
    "-Wshadow",
    "-Wsuggest-override",
    "-Wswitch-enum",
    "-Wundef",
    "-Wunused",
    "-Wunused-but-set-parameter",
    "-Wunused-function",
    "-Wuseless-cast",
    "-Wvla",
    "-Wwrite-strings",
    "-Wzero-as-null-pointer-constant",
]

# Internal C++ compilation options. Use this by default for all C++ targets in the repo.
DEFAULT_CXXOPTS = (
    _CXXOPTS_BASE +
    select({
        "//bazel:strict_false": _CXXOPTS_WARNING,
        "//bazel:strict_true": _CXXOPTS_WARNING + ["-Werror"],
    })
)

def gba_binary(name, copts = [], **kwargs):
    """Like `cc_binary`, but always builds for the GBA platform."""

    gba_wrap(
        cc_binary,
        name,
        copts = DEFAULT_CXXOPTS + copts,
        **kwargs
    )

def gba_library(name, copts = [], **kwargs):
    """Like `cc_library`, but always builds for the GBA platform."""

    gba_wrap(
        cc_library,
        name,
        copts = DEFAULT_CXXOPTS + copts,
        **kwargs
    )
