#!/bin/sh
set -eu

cc -std=c99 -O2 -Wall -Wextra \
  -DED25519_SIGN_USE_HEADER_ONLY \
  test.c \
  -o test_header_only

./test_header_only
