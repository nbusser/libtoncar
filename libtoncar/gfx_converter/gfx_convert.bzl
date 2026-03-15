load("@rules_cc//cc:defs.bzl", "cc_library")

def gfx_sprites(name, srcs, copts = [], **kwargs):
    generated_srcs = []
    generated_hdrs = []

    for src in srcs:
        stem = src.rsplit(".", 1)[0]

        cpp_out = stem + ".cpp"
        h_out = stem + ".h"

        genrule_name = name + "_gen_" + stem.replace("/", "_")

        native.genrule(
            name = genrule_name,
            srcs = [src],
            outs = [cpp_out, h_out],
            cmd = "$(execpath //libtoncar/gfx_converter:gfx_converter) --out-dir $(@D) $(SRCS)",
            tools = ["//libtoncar/gfx_converter:gfx_converter"],
        )

        generated_srcs.append(cpp_out)
        generated_hdrs.append(h_out)

    cc_library(
        name = name,
        srcs = generated_srcs,
        hdrs = generated_hdrs,
        copts = copts,
        include_prefix = "gfx",
        deps = ["//libtoncar"],
        **kwargs
    )
