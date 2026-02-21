.PHONY: setup build generate_commands build-dev run clean super-clean

BAZEL = bazel
QUERY = //src:game
MGBA = mgba

DEPS_DIR := deps

# Installs dependencies that cannot be fetched form bazel.
setup:
	rm -rf $(DEPS_DIR)/installed && \
	mkdir $(DEPS_DIR)/installed && \
	tmpdir=$$(mktemp -d) && \
	tar --zstd -xf $(DEPS_DIR)/tarballs/devkitarm-crtls-1.2.6-1-any.pkg.tar.zst -C "$$tmpdir" >/dev/null && \
	mv "$$tmpdir/opt/devkitpro/devkitARM/arm-none-eabi/lib" $(DEPS_DIR)/installed/devkitarm-crtls-1.2.6-1 && \
	rm -rf "$$tmpdir" && \
	tmpdir=$$(mktemp -d) && \
	tar --zstd -xf $(DEPS_DIR)/tarballs/libgba-0.5.4-1-any.pkg.tar.zst -C "$$tmpdir" >/dev/null && \
	mv "$$tmpdir/opt/devkitpro/libgba" $(DEPS_DIR)/installed/libgba-0.5.4-1 && \
	rm -rf "$$tmpdir"

build:
	$(BAZEL) build --config=gba --config=strict "$(QUERY)"

generate_commands:
	$(BAZEL) run --config=host --config=strict //tools/generate_compile_commands -- "//src:game"

build-dev: generate_commands build

# TODO: build mgba via bazel
run:
	$(MGBA) bazel-bin/src/game.gba

clean:
	$(BAZEL) clean
	rm compile_commands.json

super-clean:
	$(BAZEL) clean --expunge
	rm compile_commands.json
