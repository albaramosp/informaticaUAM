#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "euclides.h"


/*int main( int argc, char *argv[]){
	mpz_t a, b, mcd, x, y, t, zero, one;
*/

	/*inicialización de variables reservando espacio de memoria*/
/*	mpz_init (a);
	mpz_init (b);
	mpz_init (mcd);
	mpz_init (x);
	mpz_init (y);
	mpz_init (t);
	mpz_init (zero);
	mpz_init (one);

	mpz_set_str (zero, "0", 10);
	mpz_set_str (one, "1", 10);
*/
	/*asocia los dos argumentos de entrada a las variables a y b*/
/*	gmp_sscanf(argv[1], "%Zd", a);
	gmp_sscanf(argv[2], "%Zd", b);
*/

	/* Hacer que el valor a sea el mayor */
/*	if (mpz_cmp(a, b) < 0){*//*si a < b*/

		/*intercambia a por b*/
/*		mpz_set(t, a);
		mpz_set(a, b);
		mpz_set(b, t);
	}
*/	
/*	extended_euclides (a, b, mcd, x, y);
*/	
	/*se imprime el valor de x, y, y el mcd(a,b)*/	
/*	gmp_printf("x=%Zd, y=%Zd, mcd(%Zd,%Zd) = %Zd\n", x, y, a, b, mcd);
*/
	/*liberalización de espacio de memoria*/
/*	mpz_clear (a);
	mpz_clear (b);
	mpz_clear (mcd);
	mpz_clear (x);
	mpz_clear (y);
	mpz_clear (t);
	mpz_clear (zero);
	mpz_clear (one);

	return 0;
}*/

void obtener_inverso(mpz_t  a, mpz_t  m, mpz_t  t){
	mpz_t mo, ao, to, q, r, temp, zero, one;
	
	/*inicialización de variables reservando espacio de memoria*/
	mpz_init (mo);
	mpz_init (ao);
	mpz_init (to);
	mpz_init (q);
	mpz_init (r);
	mpz_init (temp);
	mpz_init (zero);
	mpz_init (one);

	mpz_set_str (zero, "0", 10);
	mpz_set_str (one, "1", 10);

	mpz_set(mo,m);/*mo = m*/
	mpz_set(ao,a);/* ao = a;*/
	mpz_set(to,zero);/* to = 0;*/
	mpz_set(t,one);/* t = 1;*/
	mpz_fdiv_q(q, mo, ao);/* q = mo/ao;*/


	/*r= mo - (q*ao);*/
	mpz_mul(r, q, ao);
	mpz_sub(r, mo, r);

	/*while (r>0){*/
	while (mpz_cmp(r, zero) > 0){
		/*temp = to - (q*t)*/
		mpz_mul(temp, q, t);
		mpz_sub(temp, to, temp);

		
		/*if (temp >=0){*/
		if (mpz_cmp(temp, zero) >= 0){
			/*temp = temp%m;*/
			mpz_fdiv_r(temp, temp, m);
		}else{
			/*temp = m-((-temp%m));*/
			mpz_neg(temp, temp);
			mpz_fdiv_r(temp, temp, m);
			mpz_sub(temp, m, temp);
		}
		
		mpz_set(to,t);/*to = t;*/
		mpz_set(t,temp);/*t=temp;*/
		mpz_set(mo,ao);/*mo= ao;*/
		mpz_set(ao,r);/*ao = r;*/
		mpz_fdiv_q(q, mo, ao);/*q = mo/ao;*/ /*numero entero inmediatamente inferior o igual a mo/ao*/
		/*r = mo-(q*ao);*/
		mpz_mul(r, q, ao);
		mpz_sub(r, mo, r);
	}
	
	/*if(ao!=1){*/
	if (mpz_cmp(ao, one) != 0){
		gmp_printf ("%Zd no tiene inverso módulo %Zd\n", a, m);
		mpz_set(t,zero);/* t = 0; se inicializa para sucesivas llamadas*/
	}else{
		gmp_printf ("el inverso de %Zd mod %Zd = %Zd\n", a, m,t);
	}

	mpz_clear (mo);
	mpz_clear (ao);
	mpz_clear (to);
	mpz_clear (q);
	mpz_clear (r);
	mpz_clear (temp);
	mpz_clear (zero);
	mpz_clear (one);
	
	return;

}
