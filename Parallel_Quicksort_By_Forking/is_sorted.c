#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <data file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    FILE *in = fopen(filename, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Couldn't open %s\n", argv[1]);
        return 1;
    }

    int64_t prev, curr;
    int rc;
    size_t num_elts = 0;

    // read first element
    rc = fread(&prev, sizeof(int64_t), 1, in);
    if (rc != 1)
    {
        fclose(in);
        fprintf(stderr, "Could not read first data value (file is empty?)\n");
        return 0;
    }
    num_elts++;

    // read remaining elements
    int done = 0;
    while (!done)
    {
        if (fread(&curr, sizeof(int64_t), 1, in) != 1)
            done = 1;
        else
        {
            num_elts++;
            if (curr < prev)
            {
                fclose(in);
                fprintf(stderr,
                        "Data values are not sorted! (element %lu is less than element %lu)\n",
                        num_elts, num_elts - 1);
                return 1;
            }
            prev = curr;
        }
    }

    fclose(in);
    printf("Data values are sorted!\n");

    return 0;
}
