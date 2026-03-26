#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ed25519_sign.c"

static void
print_hex(const char *label, const unsigned char *buf, size_t len)
{
    size_t i;

    printf("%s=", label);
    for (i = 0; i < len; i++) {
        printf("%02x", buf[i]);
    }
    putchar('\n');
}

static int
parse_seed_value(unsigned int *out, const char *s)
{
    char               *end = NULL;
    unsigned long long  v;

    if (s == NULL || *s == 0) {
        return -1;
    }
    v = strtoull(s, &end, 0);
    if (*end != 0 || v > UINT_MAX) {
        return -1;
    }
    *out = (unsigned int) v;
    return 0;
}

static void
generate_seed_from_rand(unsigned char seed[32], unsigned int seed_value)
{
    size_t i;

    srand(seed_value);
    for (i = 0; i < 32; i++) {
        seed[i] = (unsigned char) (rand() & 0xff);
    }
}

int
main(int argc, char **argv)
{
    unsigned char seed[32];
    unsigned char pk[32];
    unsigned char sk[64];
    char          seed_value_text[64];
    unsigned int  seed_value;

    if (argc > 2) {
        fprintf(stderr, "usage: %s [integer-seed]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        if (parse_seed_value(&seed_value, argv[1]) != 0) {
            fprintf(stderr, "invalid integer seed\n");
            return 1;
        }
    } else {
        fputs("Input integer seed for rand(): ", stdout);
        fflush(stdout);
        if (fgets(seed_value_text, sizeof seed_value_text, stdin) == NULL) {
            fprintf(stderr, "failed to read integer seed\n");
            return 1;
        }
        seed_value_text[strcspn(seed_value_text, "\r\n")] = 0;
        if (parse_seed_value(&seed_value, seed_value_text) != 0) {
            fprintf(stderr, "invalid integer seed\n");
            return 1;
        }
    }

    generate_seed_from_rand(seed, seed_value);
    ed25519_seed_keypair(pk, sk, seed);

    printf("rand_seed=%u\n", seed_value);
    print_hex("private_key", seed, sizeof seed);
    print_hex("public_key", pk, sizeof pk);

    return 0;
}
