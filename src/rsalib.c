#include "rsalib.h"
/* RSA algorithm demonstration
 * by Prodiggy
 */
unsigned long long STDMAX = 128; //1024 bits
bool verbose = false;

void genkeys(struct Keys *keys)
{
    srand(time(0));
    mpz_t phi;
    char   *n;
    unsigned int range, found = 0;
    { //Initializing each key and setting their value to 0
        mpz_init(phi);
        mpz_init(keys->p);
        mpz_init(keys->q);
        mpz_init(keys->n);
        mpz_init(keys->e);
        mpz_init(keys->d);
    }  
    getPrimes(&keys->p, STDMAX); //Radom prime for p
    getPrimes(&keys->q, STDMAX); //Radom prime for q
    mpz_mul(keys->n, keys->p, keys->q); //Calculating n
    getPhi(&phi, keys->p, keys->q); //Calculating Euler's totient function
    while (!found)
    {
        n = mpz_get_str(NULL, 10, keys->n);
        range = rand() % (strlen(n)); //Random exponent 10ˣ
        getPrimes(&keys->e, range);
        found = mpz_invert(keys->d, keys->e, phi); //Equals to 0 when satisfies d * e ≅ 1 mod (ϕ)
        free(n);
    }
    keys->size = mpz_sizeinbase(keys->n, 2);
    mpz_clear(phi);
}

void getPrimes(mpz_t *prime, const int STDMAX)
{
	char *string;
    mpz_t random;

    string = (char *)malloc(STDMAX + 1); //10ˢᵗᵈᵐᵃˣ

    string[0] = (rand() % 9) + '1'; //No 0s on the first digit
    string[STDMAX] = 0x00;
    for (int i = 1; i < STDMAX - 1; i++) string[i] = (rand() % 10) + '0'; //Random number

    mpz_init_set_str(random, string, 10);
    mpz_nextprime(*prime, random); //Finding prime
    
    mpz_clear(random);
    free(string);
}

void getPhi(mpz_t *n, mpz_t p, mpz_t q)
{
    mpz_sub_ui(p, p, 1L);
    mpz_sub_ui(q, q, 1L);
    mpz_mul(*n, p, q);
    mpz_add_ui(p, p, 1L);
    mpz_add_ui(q, q, 1L);
}

char **formatString(char *input, const int counter)
{
    char **result = (char **)calloc(counter, sizeof(char *));
    char d[] = {0x20, 0x00};
  
    if (result)
    {
        signed n = 0;
        char *ptr = strtok(input, d);
        while (ptr)
        {
        result[n] = strdup(ptr);
        ptr = strtok(NULL, " ");
        n++;
        }
        return result;
    }
    fprintf(stderr, "[Error]: Malfunction while formatting string\n");
    exit(EXIT_FAILURE);
}

void encodeString(const struct Keys k, const char *input)
{
    size_t length;
    char  *result;
    mpz_t    temp;
    
    length = strlen(input);
    printf("Input: %s\n", input);
    for (int i = 0; i < length; i++)
    {
        mpz_init_set_ui(temp, (unsigned long long) input[i]);
        mpz_powm_sec(temp, temp, k.e, k.n); //C = Mᵉ mod (n)
        result = mpz_get_str(NULL, 10, temp);
        printf("%s ", result);
        mpz_clear(temp);
        free(result);
    }
    printf("\n");
}

void encodeStream(const struct Keys k, const char *filename)
{
    mpz_t in_data, out_data, temp, p;
    long               file_size;
    char         *input, *output;
    int data_size, block_size, n;
    FILE     *in_file, *out_file;

    mpz_init(in_data);
    mpz_init(out_data);
    in_file = fopen(filename, "rb");

    fseek(in_file, 0, SEEK_END);
    file_size = ftell(in_file); //Get file size in bytes
    fseek(in_file, 0, SEEK_SET);
    input = (char *) malloc(file_size);

    fread(input, 1, file_size, in_file);
    fclose(in_file);

    mpz_import(in_data, file_size, 1, 1, 0, 0, input); //Copy bytes in sequence assigning to a mpz_t
    free(input);

    data_size = mpz_sizeinbase(in_data, 2); //Bits. No leading zeros
    block_size = k.size - 1;
    data_size += block_size;
    data_size -= data_size % block_size; //File % blocks = 0
    mpz_init(temp);
    mpz_init(p);
    for (int i = (data_size / block_size) - 1; i >= 0; i--)
    { //For block
        mpz_set_si(temp, 0);
        for (int j = block_size - 1; j >= 0; j--) 
        { /* mpz_tstbit(const mpz_t op, mp_bitcnt_t bit_index) -> Test bit index and return its value
           * mpz_setbit(mpz_t rop, mp_bitcnt_t bit_index)      -> Set bit index in rop
           */
            if (mpz_tstbit(in_data, i * block_size + j)) //Separate every bits
                mpz_setbit(temp, j); 
        }
        mpz_powm_sec(temp, temp, k.e, k.n); //C = Mᵉ mod (n)
        mpz_ui_pow_ui(p, 2, i * k.size);
        mpz_mul(temp, temp, p);
        mpz_add(out_data, out_data, temp);
    }
    mpz_clear(in_data);
        
    data_size = mpz_sizeinbase(out_data, 2);
    data_size += 8;
    data_size -= data_size % 8; //Same here, file % blocks = 0
    output = (char *)malloc(data_size / 8);
    n = (data_size / 8) - 1;
    for (int i = n; i >= 0; i--)
    {
        mpz_set_si(temp, 0);
        for (int j = 7; j >= 0; j--) 
        {
            if (mpz_tstbit(out_data, i * 8 + j)) 
                mpz_setbit(temp, j);       
        }
        output[n - i] = (char) mpz_get_ui(temp);
    }
    mpz_clear(out_data);

    out_file = fopen(filename, "wb");

    fseek(out_file, 0, SEEK_SET);
    fwrite(output, 1, data_size / 8, out_file);

    mpz_clear(temp);
    mpz_clear(p);
    fclose(out_file);
    free(output);

    if (verbose) printf("%s encoded. (%li bytes)\n", filename, file_size);
}

void decodeString(const struct Keys k, FILE *in_file)
{
    char *format, **input, *input_raw, result;
    int length = 1, file_size;
    mpz_t temp;

    fseek(in_file, 0, SEEK_END);
    file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);
    input_raw = (char *) malloc(file_size);
    fread(input_raw, 1, file_size, in_file);
    fclose(in_file);

    for (int i = 0, l = strlen(input_raw); i < l; i++)
    {
        if (input_raw[i] == ' ') length++;
    }
    format = strdup(input_raw);
    input = formatString(format, length);
    for (int i = 0; i < length; i++)
    {
        mpz_init_set_str(temp, input[i], 10);
        mpz_powm_sec(temp, temp, k.d, k.n);
        result = (char) mpz_get_ui(temp);

        if (!i) if (!isalnum(result)) printf("[Warning]: Result doesn't appear to be plaintext, check your key file\n");
        if (!i) printf("Output: ");
        printf("%c", result);
        mpz_clear(temp);
    }
    printf("\n");

    for (int i = 0; i < length; i++) free(input[i]); //Cleaning memory
    free(input_raw);
    free(format);
    free(input);
}

void decodeStream(struct Keys k, const char *filename)
{
    mpz_t in_data, out_data, temp, p;
    long               file_size;
    char         *input, *output;
    int data_size, block_size, n;
    FILE     *in_file, *out_file;

    mpz_init(in_data);
    mpz_init(out_data);
    in_file = fopen(filename, "rb");

    fseek(in_file, 0, SEEK_END);
    file_size = ftell(in_file); //Get file size in bytes
    fseek(in_file, 0, SEEK_SET);
    input = (char *) malloc(file_size);

    fread(input, 1, file_size, in_file);
    fclose(in_file);

    mpz_import(in_data, file_size, 1, 1, 0, 0, input); //Copy bytes in sequence assigning to a mpz_t
    free(input);
    
    data_size = mpz_sizeinbase(in_data, 2); //Bits. No leading zeros
    block_size = k.size;
    data_size += block_size;
    data_size -= data_size % block_size; //File % blocks = 0
    mpz_init(temp);
    mpz_init(p);
    
    for (int i = (data_size / block_size) - 1; i >= 0; i--)
    { //For block
        mpz_set_si(temp, 0);
        for (int j = block_size - 1; j >= 0; j--) 
        {
            if (mpz_tstbit(in_data, i * block_size + j)) //Separate every bits
                mpz_setbit(temp, j); 
        }
        mpz_powm_sec(temp, temp, k.d, k.n); //C = Mᵉ mod (n)
        mpz_ui_pow_ui(p, 2, i * (k.size - 1));
        mpz_mul(temp, temp, p);
        mpz_add(out_data, out_data, temp);
    }
    mpz_clear(in_data);

    data_size = mpz_sizeinbase(out_data, 2);
    data_size += 8;
    data_size -= data_size % 8;
    output = (char *) malloc(data_size / 8);
    n = (data_size / 8) - 1;
    for (int i = n; i >= 0; i--)
    {
        mpz_set_si(temp, 0);
        for (int j = 7; j >= 0; j--) 
        {
            if (mpz_tstbit(out_data, i * 8 + j)) 
                mpz_setbit(temp, j);       
        }
        output[n - i] = (char) mpz_get_ui(temp);
    }
    mpz_clear(out_data);

    out_file = fopen(filename, "wb");

    fseek(out_file, 0, SEEK_SET);
    fwrite(output, 1, data_size / 8, out_file);

    mpz_clear(temp);
    mpz_clear(p);
    fclose(out_file);
    free(output);

    if (verbose) printf("%s decoded. (%d bytes)\n", filename, data_size);
}
