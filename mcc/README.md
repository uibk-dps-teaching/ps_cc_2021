# mC Compiler

This repository holds a *getting started* code-base for the [compiler construction course](..).

Be sure to understand the provided code before using it!

- See [Development Notes](docs/development_notes.md)

## Prerequisites

- [Meson](http://mesonbuild.com/)
- [Ninja](https://ninja-build.org/)
- `time`, typically located at `/usr/bin/time`, do not confuse this with the Bash built-in
- A compiler supporting C17 — typically GCC or Clang

## Build Instructions

First, generate the build directory.

    meson builddir

Meson creates Ninja build files.
Let's build.

    ninja -C builddir

Unit tests can be run directly with Meson.

    meson test -C builddir

For integration testing, we try to compile and run mC programs.
We verify the runtime behavior by checking the program's output.

    (cd builddir && ../scripts/run_integration_tests)

## Known Issues

- Only a small subset of the mC grammar is parsed.
- No semantic checks
- No compiler core
- No compiler backend
