#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAND_SEED 1

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr,
                "Usage: %s <size> <output filename>\n"
                "  <size> can have 'M' suffix for size in megabytes\n",
                argv[0]);
        exit(1);
    }

    char *end;
    size_t size = strtoul(argv[1], &end, 10);
    if (*end == 'M')
        size *= (1024U * 1024U);

    FILE *out = fopen(argv[2], "wb");
    if (out == NULL)
    {
        fprintf(stderr, "Couldn't open '%s' for output\n", argv[2]);
        return 1;
    }

    srand(RAND_SEED);

    for (size_t i = 0; i < size; ++i)
    {
        uint8_t b = rand() & 0xFF;
        if (fwrite(&b, sizeof(uint8_t), 1, out) != 1)
        {
            fclose(out);
            fprintf(stderr, "Error: fwrite failed\n");
            return 1;
        }
    }

    fclose(out);
    printf("Wrote %lu bytes to '%s'\n", size, argv[2]);

    return 0;
}
