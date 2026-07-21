"""Forces GBA C++ targets to always build for the GBA platform.

`cc_binary`/`cc_library` (and other C++ rules) resolve their toolchain from
the current target platform, which defaults to the host platform unless
`--platforms` is passed on the command line. Since this repo also has
host-only tools (Go, Rust), we can't just set `--platforms=//platform:gba`
globally: that breaks the host tools instead.

`gba_binary`/`gba_library` fix this by wrapping `cc_binary`/`cc_library` in a
rule that transitions its target platform to `//platform:gba` regardless of
the top-level `--platforms` flag, so `bazel build //...` "just works": every
GBA C++ target builds with the GBA toolchain, everything else (Go, Rust,
Python, ...) builds for the host, and nothing gets skipped as incompatible.
"""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@rules_cc//cc/common:cc_info.bzl", "CcInfo")

def _gba_platform_transition_impl(_settings, _attr):
    return {"//command_line_option:platforms": [str(Label("//platform:gba"))]}

_gba_platform_transition = transition(
    implementation = _gba_platform_transition_impl,
    inputs = [],
    outputs = ["//command_line_option:platforms"],
)

def _gba_transition_wrapper_impl(ctx):
    target = ctx.attr.target[0]
    default_info = target[DefaultInfo]
    providers = [
        DefaultInfo(
            files = default_info.files,
            runfiles = default_info.default_runfiles,
        ),
    ]
    if CcInfo in target:
        providers.append(target[CcInfo])
    if OutputGroupInfo in target:
        providers.append(target[OutputGroupInfo])
    return providers

_gba_transition_wrapper = rule(
    implementation = _gba_transition_wrapper_impl,
    attrs = {
        "target": attr.label(
            cfg = _gba_platform_transition,
            mandatory = True,
        ),
        # Required by Bazel for any rule attr using a Starlark transition.
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
)

def _gba_wrap(rule_fn, name, **kwargs):
    visibility = kwargs.pop("visibility", None)
    internal_name = name + "_gba_internal"
    rule_fn(
        name = internal_name,
        target_compatible_with = [
            "//platform:os",
            "//platform:arm7tdmi",
        ],
        **kwargs
    )
    _gba_transition_wrapper(
        name = name,
        target = ":" + internal_name,
        visibility = visibility,
    )

def gba_binary(name, **kwargs):
    """Like `cc_binary`, but always builds for the GBA platform.

    Args:
      name: name of the target.
      **kwargs: forwarded to `cc_binary`, except `visibility` which is
        applied to the public target instead.
    """
    _gba_wrap(cc_binary, name, **kwargs)

def gba_library(name, **kwargs):
    """Like `cc_library`, but always builds for the GBA platform.

    Args:
      name: name of the target.
      **kwargs: forwarded to `cc_library`, except `visibility` which is
        applied to the public target instead.
    """
    _gba_wrap(cc_library, name, **kwargs)
