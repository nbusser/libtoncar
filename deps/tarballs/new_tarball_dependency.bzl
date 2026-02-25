# Custom rule to extract a committed tarball.
# We need this to workaround devkitpro's server 403ing our requests.

def _new_tarball_dependency(ctx):
    ctx.extract(
        ctx.attr.path,
        strip_prefix = ctx.attr.strip_prefix,
        watch_archive = "no",
    )
    ctx.file("BUILD.bazel", ctx.read(ctx.attr.build_file))

new_tarball_dependency = repository_rule(
    attrs = {
        "path": attr.label(),
        "strip_prefix": attr.string(),
        "build_file": attr.label(),
    },
    implementation = _new_tarball_dependency,
)
