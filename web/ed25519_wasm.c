#include "../ed25519_sign.h"

#include <stddef.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

static void
bytes_to_hex(char *out, const unsigned char *in, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    size_t i;

    for (i = 0; i < len; i++) {
        out[i * 2] = hex[in[i] >> 4];
        out[i * 2 + 1] = hex[in[i] & 0x0f];
    }
    out[len * 2] = '\0';
}

EMSCRIPTEN_KEEPALIVE
int
wasm_sign_hex(const char *private_key_hex,
              const unsigned char *data,
              int data_len,
              char *signature_hex_out)
{
    unsigned char private_key[32];
    unsigned char signature[64];

    if (private_key_hex == NULL || signature_hex_out == NULL || data_len < 0) {
        return -1;
    }
    if (ed25519_util_hex2bytes(private_key, sizeof private_key, private_key_hex) != 0) {
        return -2;
    }
    if (ed25519_sign(private_key, data, (size_t) data_len, signature) != 0) {
        return -3;
    }

    bytes_to_hex(signature_hex_out, signature, sizeof signature);
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int
wasm_verify_hex(const char *public_key_hex,
                const unsigned char *data,
                int data_len,
                const char *signature_hex)
{
    unsigned char public_key[32];
    unsigned char signature[64];

    if (public_key_hex == NULL || signature_hex == NULL || data_len < 0) {
        return -1;
    }
    if (ed25519_util_hex2bytes(public_key, sizeof public_key, public_key_hex) != 0) {
        return -2;
    }
    if (ed25519_util_hex2bytes(signature, sizeof signature, signature_hex) != 0) {
        return -3;
    }

    return ed25519_verify(public_key, data, (size_t) data_len, signature) ? 1 : 0;
}
