# libsodium_ed25519_sign

[中文版 README](./README.zh-CN.md)

A minimal Ed25519 signing and verification extraction derived from libsodium.

## Why This Repository Exists

This repository exists to keep only the minimum code needed for data signing and signature verification.

The goal is practical:

- reduce dependency surface
- keep the code path focused on the sign/verify use case
- make static linking(Avoid dynamic linking injection bypassing verify) and code review easier
- avoid carrying the rest of the libsodium library when only Ed25519 signature verification is needed

Typical use cases include:

- checking that communication payloads have not been tampered with in transit
- verifying that a software activation key or license payload has not been modified

This repository is focused on integrity verification, not on general-purpose cryptographic coverage.

## API

```c
int ed25519_sign(const unsigned char private_key[32],
                 const unsigned char *data,
                 size_t data_len,
                 unsigned char signature[64]);

int ed25519_verify(const unsigned char public_key[32],
                   const unsigned char *data,
                   size_t data_len,
                   const unsigned char signature[64]);
```

## Conventions

- `private_key` is a 32-byte Ed25519 seed.
- `public_key` is a 32-byte Ed25519 public key.
- `signature` is a 64-byte Ed25519 signature.
- `ed25519_verify()` returns `1` on success and `0` on failure.

Hex note:

- a 32-byte key is often shown as 64 hex characters
- that is expected, because 2 hex characters represent 1 byte
- so a 64-character hex string still represents a 32-byte binary key

If you need to convert a 64-character hex key into a 32-byte buffer, use:

```c
int ed25519_util_hex2bytes(unsigned char *out,
                           size_t out_len,
                           const char *hex);
```

## Usage

Separate compilation:

```c
#include "ed25519_sign.h"
```

Compile with:

```sh
cc -std=c99 -O2 your_file.c ed25519_sign.c -o your_program
```

Header-only, please copy the contants in the folder single-header-only, then:

```c
#include "ed25519_sign.h"
```

Generate a keypair from an integer seed for `rand()`:

```sh
cc -std=c99 -O2 gen_keypair.c -o gen_keypair
./gen_keypair 12345
```

The tool will:

- seed C `rand()` with the integer you provide
- generate a 32-byte Ed25519 seed
- print the generated 32-byte private key and 32-byte public key

This is for testing/demo convenience, not for production-grade key generation.

## Test

`test.c` supports two modes:

- default: includes `ed25519_sign.h` and links with `ed25519_sign.c`
- header-only: just use the header-only version

Interactive flow:

1. input a message to sign
2. the program signs that message internally
3. input a message again for verification
4. if the second input is identical, verification passes
5. otherwise, verification fails

Run the separate-compilation test:

```sh
./build_test_separate.sh
```

Run the header-only test:

```sh
./build_test_header_only.sh
```

The two scripts automatically feed the same message twice, so they should print:

- `verification passed`

If you want to test tampering manually, run the produced binary directly and input different content the second time.
