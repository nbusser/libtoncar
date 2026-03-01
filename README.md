# libtoncar - GBA C++ lib

Based on https://github.com/nbusser/CPP-GBA-Bazel-Base.

This repo hosts the modest libtoncar, a C++ lib inspired by libtonc, and built via Bazel.

## Building

Requires Bazel 8.3.1. You do not have to install devKitARM, Bazel will download it automatically.

```shell
make build
```

## Running

Requires mgba. I wish in the future to build mgba from the repo directly.

```shell
make run
```

## Clangd

The repo is configured to use `clangd`.

To generate the compile commands file, you can run:

```shell
make compile-commands.json
```

You can easily regenerate the file and build at the same time with:
```shell
make build-dev
```

**Warning:** the clangd configuration file is generated on-the-go by vscode with clangd extension.

If you are not using vscode, make sure to run:

```shell
make .clangd
```

## Clang tidy

You can lint the code by running:

```shell
make lint
```
