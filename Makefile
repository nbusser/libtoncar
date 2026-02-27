.PHONY: build compile_commands.json build-dev run lint basic-clean clean super-clean

BAZEL?=bazel
QUERY?=//src:game
CLANG_TIDY?=clang-tidy
SOURCES:=$(shell find src -name "*.cpp")

.clangd:
	tools/clangd/gen_clangd.sh

.clang-tidy:
	tools/clangd/gen_clang_tidy.sh

build:
	$(BAZEL) build --config=gba --config=strict "$(QUERY)"

compile_commands.json:
	$(BAZEL) run --config=host //tools/generate_compile_commands -- \
		"$(QUERY)" --extra_aquery_arg=--config=gba

build-dev: build compile_commands.json

run:
	$(BAZEL) run --config=gba --config=strict //tools/mgba:run_rom

# Runs clang-tidy with a temporary compile_commands.json,
# modified to remove all thumb specific flags.
lint: .clangd .clang-tidy compile_commands.json
	tmp="$$(mktemp -d)" && \
	sed -E 's/(-mthumb-interwork|-mthumb)//g' compile_commands.json > "$$tmp/compile_commands.json" && \
	$(CLANG_TIDY) -p "$$tmp" $(SOURCES) || rm -rf "$$tmp"

basic-clean:
	rm compile_commands.json .clangd .clang-tidy

clean: basic-clean
	$(BAZEL) clean

super-clean: basic-clean
	$(BAZEL) clean --expunge
	rm -rf external
	rm -rf .cache
	rm -rf bazel-*
