#include "rsalib.h"

void printLogs(const struct Keys keys)
{
    if (verbose) printf("Key file generated.\n");
    FILE *keys_out;
    keys_out = fopen("keys", "w+");
    gmp_fprintf(keys_out, "%Zd\n%Zd\n%Zd\n%Zd\n%Zd", 
                           keys.p, keys.q, keys.n, keys.e, keys.d);
    fclose(keys_out);
    return;  
}

void readLogs(char *input, struct Keys *keys)
{
    mpz_t *keys_ptr[] = {&keys->p, &keys->q, &keys->n, &keys->e, &keys->d};
    char buffer[1000];   

    FILE *f = fopen(input, "r");
    if (!f)
    {
        fprintf(stderr, "[Error]: Key file not found!\n"); 
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 5; i++)
    {
        fgets(buffer, STDMAX * 2, f);
        mpz_init_set_str(*keys_ptr[i], buffer, 10);
    }
    keys->size = mpz_sizeinbase(keys->n, 2); //Size in bits of N = blocks

    fclose(f);
    return;
}
