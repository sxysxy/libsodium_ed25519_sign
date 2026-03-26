#!/bin/sh
set -eu

cc -std=c99 -O2 -Wall -Wextra \
  test.c ed25519_sign.c \
  -o test_separate

./test_separate
