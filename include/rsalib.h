#pragma once

#include   <ctype.h>
#include  <getopt.h>
#include     <gmp.h>
#include <stdbool.h>
#include   <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    <time.h>
#include  <unistd.h>

#ifndef __RSA_C_
#define __RSA_C_

#define _GNU_SOURCE

typedef struct Keys
{
    mpz_t p, q, n, d, e;
    int size;
} Keys_t; //RSA keys own type

extern unsigned long long STDMAX; //Base
extern bool verbose;

/* Initialize RSA keys and define a value for each
 */
void genkeys(struct Keys *keys);

/* Generate a random prime number in a specific range
 * Randomize a string usign ASCII table and assign to a mpz_t variable
 */
void getPrimes(mpz_t *prime, const int STDMAX);

/* Get Euler's totient function of P and Q
 * ϕ = (p - 1) * (q - 1)
 */
void getPhi(mpz_t *n, mpz_t p, mpz_t q);

/* Breaks the string into a sequence of substrings
 * Uses blank spaces as delimiter
 */
char **formatFile(char *input, int *words);

/* This function encodes strings byte by byte.
 * It's strongly recommended that you use it carefully.
 * It may fail against known-plaintext attacks.
 * Cipher message = Mᵉ mod (n)
 */
void encodeString(const struct Keys k, const char *input);

/* This function encodes packets of data
 * Each encoded packet should represent "block_size" characters 
 * Cipher message = Mᵉ mod (n)
 */
void encodeStream(const struct Keys k, const char *filename);

/* This function decodes strings byte by byte.
 * Plaintext message = Cᵈ mod (n)
 */
void decodeString(const struct Keys k, FILE *in_file);

/* Decodes packets of data using private keys
 * Plaintext message = Cᵈ mod (n)
 */
void decodeStream(struct Keys k, const char *filename);

/* Generate a key log using a file or stdout
 */
void printLogs(const struct Keys keys);

/* Reads a key log and populate a struct Keys
 * Recieve the name of the file and the target struct
 */
void readLogs(char *input, struct Keys *keys);

#endif