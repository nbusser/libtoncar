bazel := env_var_or_default("BAZEL", "bazel")
clang_tidy := env_var_or_default("CLANG_TIDY", "clang-tidy")

bazel_flags := "--config=gba --config=strict"

# Build a target (defaults to the example ROM).
build query="//example:game":
    {{bazel}} build {{bazel_flags}} "{{query}}"

# Build a target in release mode.
build-prod query="//example:game":
    {{bazel}} build {{bazel_flags}} --compilation_mode=opt "{{query}}"

# Regenerate compile_commands.json for clangd/clang-tidy.
compile-commands:
    {{bazel}} run --config=host //tools/generate_compile_commands -- \
        'kind(gba_wrapper, //...)'

# Build then refresh compile_commands.json.
build-dev: build compile-commands

# Run the ROM in mgba.
run:
    {{bazel}} run {{bazel_flags}} //tools/mgba:run_rom

# Run the ROM in mgba (release mode).
run-prod:
    {{bazel}} run {{bazel_flags}} --compilation_mode=opt //tools/mgba:run_rom

# Generate the clangd config, unless it already exists.
gen-clangd:
    test -f .clangd || tools/clangd/gen_clangd.sh

# Generate the clang-tidy config, unless it already exists.
gen-clang-tidy:
    test -f .clang-tidy || tools/clangd/gen_clang_tidy.sh

# Lint the example sources with clang-tidy.
lint: gen-clangd gen-clang-tidy compile-commands
    #!/usr/bin/env bash
    set -euo pipefail
    tmp="$(mktemp -d)"
    trap 'rm -rf "$tmp"' EXIT
    sed -E 's/(-mthumb-interwork|-mthumb)//g' compile_commands.json > "$tmp/compile_commands.json"
    {{clang_tidy}} -p "$tmp" $(find example -name '*.cpp')

basic-clean:
    rm -f compile_commands.json .clangd .clang-tidy

clean: basic-clean
    {{bazel}} clean

super-clean: basic-clean
    {{bazel}} clean --expunge
    rm -rf external
    rm -rf .cache
    rm -rf bazel-*
