#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include "des.h"

uint64_t permutar(uint64_t input, const unsigned short *tabla, int tamanio_inicial, int tamanio_final);
uint64_t* generarSubclaves (uint64_t k);
uint64_t sbox(uint64_t input);
uint64_t des(uint64_t mensaje, uint64_t k, int cifrar);
int findParity(uint64_t n);
