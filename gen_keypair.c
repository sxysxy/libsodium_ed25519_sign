#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ed25519_sign.c"

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <fcntl.h>
#include <unistd.h>
#endif

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
generate_seed(unsigned char seed[32])
{
#if defined(_WIN32)
    NTSTATUS status;

    status = BCryptGenRandom(NULL, seed, 32U, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return status == STATUS_SUCCESS ? 0 : -1;
#else
    int fd;
    size_t offset;
    ssize_t nread;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    offset = 0;
    while (offset < 32U) {
        nread = read(fd, seed + offset, 32U - offset);
        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            close(fd);
            return -1;
        }
        if (nread == 0) {
            close(fd);
            return -1;
        }
        offset += (size_t) nread;
    }

    close(fd);
    return 0;
#endif
}

int
main(int argc, char **argv)
{
    unsigned char seed[32];
    unsigned char pk[32];
    unsigned char sk[64];

    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        return 1;
    }

    if (generate_seed(seed) != 0) {
        fprintf(stderr, "failed to get secure random bytes from the operating system\n");
        return 1;
    }

    ed25519_seed_keypair(pk, sk, seed);

    print_hex("private_key", seed, sizeof seed);
    print_hex("public_key", pk, sizeof pk);

    return 0;
}
