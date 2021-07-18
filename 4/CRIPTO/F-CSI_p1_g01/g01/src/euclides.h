
#include <gmp.h>

/*=== Funciones =============================================================*/

/*-----------------------------------------------------------------------------
Nombre: mcd

Descripción: Devuelve el mcd entre dos numeros.

Argumentos de entrada: 
1. a, primer número
2. b, segundo número

Retorno: 
- maximo comun divisor entre los numeros
-----------------------------------------------------------------------------*/

int mcd(int a, int b);

/*-----------------------------------------------------------------------------
Nombre: coprimos

Descripción: Devuelve si dos números a y b son coprimos, es decir, mcd=1.

Argumentos de entrada: 
1. a, primer número
2. b, segundo número

Retorno: 
- 1 si son comprimos, 0 si no lo son
-----------------------------------------------------------------------------*/

int coprimos(mpz_t a, mpz_t b);

/*-----------------------------------------------------------------------------
Nombre: obtener_inverso

Descripción: Devuelve el inverso multiplicativo de un número 

Argumentos de entrada: 
1. a, número del que buscamos el inverso
2. m, modulo
3. t, inverso multiplicativo de a mod m

Retorno: 
- No devuelve nada
-----------------------------------------------------------------------------*/
void obtener_inverso(mpz_t  a, mpz_t  m, mpz_t  t);

