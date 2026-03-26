#include "ed25519_sign.h"

#include <stdio.h>
#include <stdlib.h>

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
    const char *private_key_hex;
    const char *file_path;
    unsigned char private_key[32];
    unsigned char signature[64];
    unsigned char *data;
    size_t data_len;
    char signature_hex[129];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <private_key_hex> <file_path>\n", argv[0]);
        return 1;
    }

    private_key_hex = argv[1];
    file_path = argv[2];
    data = NULL;
    data_len = 0;

    if (ed25519_util_hex2bytes(private_key, sizeof private_key, private_key_hex) != 0) {
        fprintf(stderr, "invalid private_key hex, expected 64 hex characters\n");
        return 1;
    }
    if (read_file(file_path, &data, &data_len) != 0) {
        fprintf(stderr, "failed to read file: %s\n", file_path);
        return 1;
    }
    if (ed25519_sign(private_key, data, data_len, signature) != 0) {
        free(data);
        fprintf(stderr, "failed to sign file: %s\n", file_path);
        return 1;
    }

    bytes_to_hex(signature_hex, signature, sizeof signature);
    puts(signature_hex);

    free(data);
    return 0;
}
