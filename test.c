#include <stdio.h>
#include <string.h>

#ifdef ED25519_SIGN_USE_HEADER_ONLY
#include "single-header-only/ed25519_sign.h"
#else
#include "ed25519_sign.h"
#endif

static size_t
read_message(const char *prompt, unsigned char *buf, size_t buf_size)
{
    size_t len;

    if (buf_size == 0U) {
        return 0U;
    }
    fputs(prompt, stdout);
    fflush(stdout);
    if (fgets((char *) buf, (int) buf_size, stdin) == NULL) {
        return (size_t) -1;
    }
    len = strlen((const char *) buf);
    if (len > 0U && buf[len - 1U] == '\n') {
        buf[--len] = 0U;
    }
    return len;
}

int
main(void)
{
    static const char seed_hex[] =
        "9d61b19deffd5a60ba844af492ec2cc4"
        "4449c5697b326919703bac031cae7f60";
    static const char public_key_hex[] =
        "d75a980182b10ab7d54bfed3c964073a"
        "0ee172f3daa62325af021a68f707511a";

    unsigned char seed[32];
    unsigned char public_key[32];
    unsigned char signature[64];
    unsigned char first_message[4096];
    unsigned char second_message[4096];
    size_t first_len;
    size_t second_len;

    if (ed25519_util_hex2bytes(seed, sizeof seed, seed_hex) != 0 ||
        ed25519_util_hex2bytes(public_key, sizeof public_key, public_key_hex) != 0) {
        fprintf(stderr, "hex decode failed\n");
        return 1;
    }

    first_len = read_message("Input message to sign: ",
                             first_message, sizeof first_message);
    if (first_len == (size_t) -1) {
        fprintf(stderr, "failed to read message\n");
        return 1;
    }
    if (ed25519_sign(seed, first_message, first_len, signature) != 0) {
        fprintf(stderr, "ed25519_sign() failed\n");
        return 1;
    }

    second_len = read_message("Input message again for verification: ",
                              second_message, sizeof second_message);
    if (second_len == (size_t) -1) {
        fprintf(stderr, "failed to read message\n");
        return 1;
    }

    if (ed25519_verify(public_key, second_message, second_len, signature)) {
        puts("verification passed");
    } else {
        puts("verification failed");
    }

    return 0;
}
