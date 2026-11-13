#!/usr/bin/env bash
set -eu
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
if [ "$#" -eq 0 ]; then
    set -- tests.cpp
fi
binary="$(mktemp /tmp/asd-lsan.XXXXXX)"
trap 'rm -f -- "$binary"' EXIT
clang++ -std=c++17 -g -O0 -fno-omit-frame-pointer -fsanitize=leak "$@" -o "$binary"
LSAN_OPTIONS="${LSAN_OPTIONS:-exitcode=23}" "$binary"
