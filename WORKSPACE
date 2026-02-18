workspace(name = "gba_example")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# ==============================================================================
# GBA Definitions
# ==============================================================================

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("//platform:repo.bzl", "devkitarm_repository", "devkitarm_urls")

devkitarm_repository(
    name = "devkitarm",
)

# Gets 403 forbidden because the server requests user agents
# which cannot be injected through bazel.
# http_archive(
#     name = "devkitarm_crtls",
#     build_file = "@//platform:devkitarm_crtls.bazel",
#     headers = {
#         "User-Agent": "Mozilla/5.0",
#     },
#     sha256 = "cdc159e16a931c173202b9a774c80af6a2c4d32c03dc3e7821eb183c7082b389",
#     strip_prefix = "opt/devkitpro/devkitARM/arm-none-eabi/lib",
#     urls = devkitarm_urls("devkitarm-crtls-1.1.1-1-any.pkg.tar.xz"),
# )

# Requires user to run ./setup.bash beforehand
new_local_repository(
    name = "devkitarm_crtls",
    build_file = "@//platform:devkitarm_crtls.bazel",
    path = "deps/installed/devkitarm",
)

new_git_repository(
    name = "libtonc",
    build_file = "@//platform:libtonc.bazel",
    commit = "ccc03fa321e56f51aed5e2ee1d6e3df3d1cbc803",
    remote = "https://github.com/devkitPro/libtonc.git",
    shallow_since = "1598408685 +0100",
)

# Gets 403 forbidden because the server requests user agents
# which cannot be injected through bazel.
# http_archive(
#     name = "libgba",
#     build_file = "@//platform:libgba.bazel",
#     sha256 = "ca806fce93e4f80d55577fa7a7cd34b12fa2934aee6a855d306c311c9cc2c876",
#     strip_prefix = "opt/devkitpro/libgba",
#     urls = devkitarm_urls("libgba-0.5.2-2-any.pkg.tar.xz"),
# )
# Requires user to run ./setup.bash beforehand
new_local_repository(
    name = "libgba",
    build_file = "@//platform:libgba.bazel",
    path = "deps/installed/libgba",
)

register_toolchains(
    "//platform:toolchain",
)

# ==============================================================================
# Go
# ==============================================================================

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "67b4d1f517ba73e0a92eb2f57d821f2ddc21f5bc2bd7a231573f11bd8758192e",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.50.0/rules_go-v0.50.0.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.50.0/rules_go-v0.50.0.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains(version = "1.19")
