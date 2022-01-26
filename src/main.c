#include "rsalib.h"

#define MAX_BUFFER 5096

void printUsage(void)
{
    printf("RSA algorithm demonstration\n\n"
           "Usage: ./rsa [-e | -d | -c ] [-f FILENAME] [-v | -b] \n\n"
           "Optional arguments:\n"
           "    -b BASE     Keys decimal base [e.g. 309 for 1024 bits]\n"
           "    -c          Generate keys\n"
           "    -e          Encode data\n"
           "    -d          Decode data\n"
           "    -h          Print this message\n"
           "    -f FILENAME Input file [e.g. file.txt]\n"
           "    -v          Verbose\n"
    );
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{   
    FILE *f;
    Keys_t keys;
    char *input, buffer[STDMAX];
    time_t start_time = time(0), etc;
    int input_format = 0;
    int x, op = 0;

    if (argc < 2)
        printUsage();

    while ((x = getopt(argc, argv, "cdehf:b:v")) != -1)
    {
        switch(x)
        {
            
            case 'f': input = optarg; 
                      input_format++; break;
            case 'b': STDMAX = atoi(optarg); break;
            case 'e': op = 1; break;
            case 'd': op = 2; break;
            case 'c': op = 3; break;
            case 'h': printUsage(); break;
            case 'v': verbose = true; break;
            case '?': printUsage(); break;
        }
    }
    if (!op) printUsage();
    if (STDMAX > 309)
    {
        fprintf(stderr, "[Error]: Unsupported primes length.\n"); 
        exit(EXIT_FAILURE);
    }
    switch(op)
    {
        case 1:
            genkeys(&keys);
            if (input_format) encodeStream(keys, input);
            else
            {
                printf("Input plaintext: ");
                if (fgets(buffer, MAX_BUFFER, stdin)) 
                    buffer[strcspn(buffer, "\n")] = 0x00;
                encodeString(keys, buffer);
            }
            printLogs(keys);
            break;
        case 2:
            readLogs("keys", &keys);
            if (input_format) decodeStream(keys, input);
            else
            {   
                printf("Paste your ciphertext in a file and input its name: ");
                if (fgets(buffer, MAX_BUFFER, stdin)) 
                    buffer[strcspn(buffer, "\n")] = 0x00;
                f = fopen(buffer, "r");                
                if (!f)
                {
                    fprintf(stderr, "[Error]: File not found!\n"); 
                    exit(EXIT_FAILURE);
                }
                else decodeString(keys, f);
            }
            break;
        case 3:
            genkeys(&keys);
            printLogs(keys);
            if (verbose) printf("Chaves geradas com sucesso\n");
            exit(EXIT_SUCCESS);
    }

    mpz_clear(keys.p);
    mpz_clear(keys.q);
    mpz_clear(keys.n);
    mpz_clear(keys.e);
    mpz_clear(keys.d);

    etc = time(0) - start_time;
    if (verbose)
    {
        printf("Time elapsed: %lu hours, %lu min, %lu secs.\n", etc / 3600,
                etc % 3600 ? (etc % 3600) / 60 : etc % 60,
                etc % 3600 ? (etc % 3600) % 60 : etc % 60);
    }
    exit(EXIT_SUCCESS);
}

