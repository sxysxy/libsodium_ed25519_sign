#!/bin/sh
set -eu

cc -std=c99 -O2 -Wall -Wextra gen_keypair.c -o gen_keypair

./gen_keypair