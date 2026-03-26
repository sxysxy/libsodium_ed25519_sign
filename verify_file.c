#include "ed25519_sign.h"

#include <stdio.h>
#include <stdlib.h>

static int
read_file(const char *path, unsigned char **out, size_t *out_len)
{
    FILE *fp;
    long file_size;
    unsigned char *buf;
    size_t read_size;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return -1;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    buf = NULL;
    if (file_size > 0) {
        buf = (unsigned char *) malloc((size_t) file_size);
        if (buf == NULL) {
            fclose(fp);
            return -1;
        }
        read_size = fread(buf, 1U, (size_t) file_size, fp);
        if (read_size != (size_t) file_size) {
            free(buf);
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);

    *out = buf;
    *out_len = (size_t) file_size;

    return 0;
}

int
main(int argc, char **argv)
{
    const char *public_key_hex;
    const char *signature_hex;
    const char *file_path;
    unsigned char public_key[32];
    unsigned char signature[64];
    unsigned char *data;
    size_t data_len;
    int verified;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <public_key_hex> <signature_hex> <file_path>\n", argv[0]);
        return 1;
    }

    public_key_hex = argv[1];
    signature_hex = argv[2];
    file_path = argv[3];
    data = NULL;
    data_len = 0;

    if (ed25519_util_hex2bytes(public_key, sizeof public_key, public_key_hex) != 0) {
        fprintf(stderr, "invalid public_key hex, expected 64 hex characters\n");
        return 1;
    }
    if (ed25519_util_hex2bytes(signature, sizeof signature, signature_hex) != 0) {
        fprintf(stderr, "invalid signature hex, expected 128 hex characters\n");
        return 1;
    }
    if (read_file(file_path, &data, &data_len) != 0) {
        fprintf(stderr, "failed to read file: %s\n", file_path);
        return 1;
    }

    verified = ed25519_verify(public_key, data, data_len, signature);
    free(data);

    if (verified) {
        puts("verified");
        return 0;
    }

    puts("failed");
    return 1;
}
