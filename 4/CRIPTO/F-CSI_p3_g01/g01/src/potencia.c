#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

void potenciaGMP(int b, int e, int m) {
	mpz_t base, exp, mod, res;

    mpz_init(base);
    mpz_init(exp);
    mpz_init(mod);
	mpz_init(res);

	mpz_set_si(base, b);
	mpz_set_si(exp, e);
	mpz_set_si(mod, m);

	mpz_powm(res, base, exp, mod);

	gmp_printf("\nCon GMP, (%Zd ^ %Zd) mod %Zd = %Zd\n", base, exp, mod, res);

	mpz_clear(base);
	mpz_clear(exp);
	mpz_clear(mod);
	mpz_clear(res);
}

//Fuente: https://www.programminglogic.com/fast-exponentiation-algorithms/
void potenciaPropia(int b, int e, int m) {
	mpz_t base, exp, mod, one, zero, res, aux;
	
	mpz_init(base);
    mpz_init(exp);
    mpz_init(mod);
    mpz_init(res);
    mpz_init(one);
    mpz_init(zero);
    mpz_init(aux);

	mpz_set_si(base, b);
	mpz_set_si(exp, e);
	mpz_set_si(mod, m);
	mpz_set_si(one, 1);
	mpz_set_si(zero, 0);
	mpz_set_si(res, 1); //x^0 = 1

    gmp_printf("\nCon lo nuestro, (%Zd ^ %Zd) mod %Zd = ", base, exp, mod);
   
	while(mpz_cmp(exp, zero) != 0) {
        /*gmp_printf("\nEntra con %Zd\n", exp);*/
        mpz_and(aux, exp, one); 

		if(mpz_cmp(aux, one) == 0) {	// bit i == 1
			mpz_mul(res, res, base);
			mpz_mod(res, res, mod);
		}
		
		mpz_fdiv_q_2exp(exp, exp, 1); // b >>= 1

		mpz_mul(base, base, base);
		mpz_mod(base, base, mod); // x = x^2 mod n
	}

	gmp_printf("%Zd\n",res);

	mpz_clear(base);
	mpz_clear(exp);
	mpz_clear(mod);
	mpz_clear(res);
	mpz_clear(one);
	mpz_clear(zero);
	mpz_clear(aux);
}

void compararRendimiento(){
	clock_t start, end;
	int i;
	FILE* output = fopen("tiempos.data", "w");
	
	for(i = 1; i < 1000000; i++){
		start = clock();
		potenciaPropia(i, 30, 240000);
		end = clock();
		fprintf(output, "%d\t%f\t", i, (float)(end - start) / CLOCKS_PER_SEC);
	
		start = clock(); 
		potenciaGMP(i, 30, 240000);
		end = clock();
		fprintf(output, "%f\n", (float)(end - start) / CLOCKS_PER_SEC);
	}
			
	fclose(output);
}

int main(int argc, char *argv[]) {
	int base, exponente, modulo;

	if (argc != 4) {
		printf("ERROR: utilizar con los siguientes argumentos: /.potencia base exponente modulo\n");
		return 1;
	} 

    base = atoi(argv[1]);
    exponente = atoi(argv[2]);
    modulo = atoi(argv[3]);

    potenciaGMP(base, exponente, modulo);
    potenciaPropia(base, exponente, modulo);
	/*compararRendimiento();*/

    return 0;
}
