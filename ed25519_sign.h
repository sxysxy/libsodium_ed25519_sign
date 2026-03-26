#ifndef ED25519_SIGN_H
#define ED25519_SIGN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int ed25519_sign(const unsigned char private_key[32],
                 const unsigned char *data,
                 size_t data_len,
                 unsigned char signature[64]);

int ed25519_verify(const unsigned char public_key[32],
                   const unsigned char *data,
                   size_t data_len,
                   const unsigned char signature[64]);

int ed25519_util_hex2bytes(unsigned char *out,
                           size_t out_len,
                           const char *hex);

#ifdef __cplusplus
}
#endif

#endif
