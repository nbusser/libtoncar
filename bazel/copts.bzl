# Bazel + GCC,
#   Default:
#     -U_FORTIFY_SOURCE
#     -Wall
#     -Wunused-but-set-parameter
#     -Wno-free-nonheap-object
#     -fno-omit-frame-pointer
#   With -c dbg, adds:
#     -g
#   With -c opt, adds:
#     -g0 -O2

# Base C options
COPTS_BASE = [
    "-std=gnu11",
]

_COPTS_WARNING = [
    "-fno-exceptions",
    "-fno-unwind-tables",
    "-fno-asynchronous-unwind-tables",
    "-fno-rtti",
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
COPTS = (
    COPTS_BASE +
    select({
        "//bazel:strict_false": _COPTS_WARNING,
        "//bazel:strict_true": _COPTS_WARNING + ["-Werror"],
    })
)

CXXOPTS_BASE = [
    "-std=gnu++23",
]

_CXXOPTS_WARNING = [
    "-Wall",
    "-Wcast-align",
    "-Wconversion",
    "-Wdouble-promotion",
    "-Wextra",
    "-Wextra-semi",
    "-Wfloat-equal",
    "-Wformat=2",
    "-Wimplicit-fallthrough",
    "-Winit-self",
    "-Wnon-virtual-dtor",
    "-Wold-style-cast",
    "-Wpedantic",
    "-Wpointer-arith",
    "-Wshadow",
    "-Wswitch-enum",
    "-Wunused",
    "-Wunused-but-set-parameter",
    "-Wunused-function",
    "-Wwrite-strings",
]

# Internal C++ compilation options. Use this by default for all C++ targets in the repo.
CXXOPTS = (
    CXXOPTS_BASE +
    select({
        "//bazel:strict_false": _CXXOPTS_WARNING,
        "//bazel:strict_true": _CXXOPTS_WARNING + ["-Werror"],
    })
)
