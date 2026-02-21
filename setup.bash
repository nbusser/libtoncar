#!/bin/bash -e

cd deps

rm -rf installed
mkdir installed

tmpdir=$(mktemp -d)
tar -xzf tarballs/devkitarm-crtls-1.2.6-1-any.pkg.tar.zst -C "$tmpdir" >/dev/null
mv "$tmpdir/opt/devkitpro/devkitARM/arm-none-eabi/lib" installed/devkitarm-crtls-1.2.6-1
rm -rf "$tmpdir"

tmpdir=$(mktemp -d)
tar -xzf tarballs/libgba-0.5.4-1-any.pkg.tar.zst -C "$tmpdir" >/dev/null
mv "$tmpdir/opt/devkitpro/libgba" installed/libgba-0.5.4-1
rm -rf "$tmpdir"
