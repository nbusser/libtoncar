#!/bin/bash -e

cd deps

rm -rf installed
mkdir installed

tmpdir=$(mktemp -d)
tar -xf tarballs/devkitarm-crtls-1.1.1-1-any.pkg.tar.xz -C "$tmpdir" >/dev/null
mv "$tmpdir/opt/devkitpro/devkitARM/arm-none-eabi/lib" installed/devkitarm-crtls
rm -rf "$tmpdir"

tmpdir=$(mktemp -d)
tar -xf tarballs/libgba-0.5.2-2-any.pkg.tar.xz -C "$tmpdir" >/dev/null
mv "$tmpdir/opt/devkitpro/libgba" installed/libgba
rm -rf "$tmpdir"
