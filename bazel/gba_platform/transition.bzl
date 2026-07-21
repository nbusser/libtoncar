"""Forces GBA C++ targets to always build for the GBA platform.

Declares `gba_wrap`, which wraps another rule and forces it to build for the GBA platform.
"""

load("@rules_cc//cc/common:cc_info.bzl", "CcInfo")

def _gba_platform_transition_impl(_settings, _attr):
    return {"//command_line_option:platforms": [str(Label("//bazel/gba_platform:gba"))]}

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

def gba_wrap(rule_fn, name, **kwargs):
    visibility = kwargs.pop("visibility", None)
    internal_name = name + "_gba_internal"
    rule_fn(
        name = internal_name,
        target_compatible_with = [
            "//bazel/gba_platform:os",
            "//bazel/gba_platform:arm7tdmi",
        ],
        **kwargs
    )
    _gba_transition_wrapper(
        name = name,
        target = ":" + internal_name,
        visibility = visibility,
    )
