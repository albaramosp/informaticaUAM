#include <stdio.h>
#include <stdlib.h>
#include "euclides.h"

int mcd(int a, int b){
	int t, r;
	/* Hacer que el valor a sea el mayor */
	if (a < b){	
		t = a;
		a = b;
		b = t;
	}
	
	/* Si el resto es igual a 0 termina el algoritmo */
	while (b != 0){
		r = a%b;
		a = b; /* Asignar el valor más pequeño a a */
		b = r; /* Asignar el resto a b */
	}
	
	return a;
}

int coprimos(mpz_t a, mpz_t b){
	mpz_t t, r, q, aa, bb;

	mpz_init (t);
	mpz_init (q);
	mpz_init (r);
	mpz_init (aa);
	mpz_init (bb);

	/* Almacenamos valores para no perder los originales durante el algoritmo */
	mpz_set(aa, a);
	mpz_set(bb, b);

	
	/* Hacer que el valor a sea el mayor */
	if (mpz_cmp(aa, bb) < 0){	
		mpz_set(t, aa);
		mpz_set(aa, bb);
		mpz_set(bb, t);
	}

	mpz_set_str (t, "0", 10);
	
	/* Si el resto es igual a 0 termina el algoritmo */
	while (mpz_cmp(bb, t) > 0){
		mpz_fdiv_qr(q, r, aa, bb);
		/* gmp_printf("%Zd = %Zd*%Zd + %Zd\n", aa, q, bb, r); */
		mpz_set(aa, bb); /* Asignar el valor más pequeño a a */
		mpz_set(bb, r); /* Asignar el resto a b */
	}

	mpz_set_str (t, "1", 10);

	/* a y b son coprimos si mcd(a, b) = 1*/
	if (mpz_cmp(aa, t) == 0){
		mpz_clear (t);
		mpz_clear (q);
		mpz_clear (r);
		mpz_clear (aa);
		mpz_clear (bb);
		return 1;
	} else {
		mpz_clear (t);
		mpz_clear (q);
		mpz_clear (r);
		mpz_clear (aa);
		mpz_clear (bb);
		return 0;
	}
}

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
	}/*else{
		gmp_printf ("el inverso de %Zd mod %Zd = %Zd\n", a, m,t);
	}*/

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
