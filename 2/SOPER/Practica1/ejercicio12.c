/**
 * @brief	El programa crea e inicializa con valores de 0 a N-1 el campo "entrada" de los elementos de un array de tipo Params.
 * Tambien crea N hilos que llaman a la funcion calculoPotencia pasando como parametro cada elemento del array de Params.
 * Esta funcion realiza el calculo de 2 elevado a "entrada", y el resultado lo guarda en el campo "salida" del elemento de tipo Params. 
 * El programa principal espera por todos sus hilos al final utilizando un bucle que se realiza N veces, e imprime las potencias calculadas.
 * @file ejercicio12.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 21-02-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
/*biblioteca hilos*/
#include <pthread.h>

#define N 6

typedef struct _Params {
  double entrada;
  double salida;
} Params;

void *calculoPotencia (void *params) {
  ((Params*)params)->salida = pow(2.0, ((Params*)params)->entrada);

   pthread_exit(NULL);
}

int main(int argc , char *argv[]) {
   pthread_t hilos[N];
   Params params[N];
   int i;


   for(i = 0; i < N; i++){
     params[i].entrada = i;
     pthread_create(&hilos[i], NULL , calculoPotencia , (void *)(&params[i]));
   }

   for(i = 0; i < N; i++){
        pthread_join(hilos[i],NULL);
        printf("2 elevado a %d es %.1lf \n", i, params[i].salida);
   }

   printf("El programa %s termino correctamente \n", argv[0]);
   exit(EXIT_SUCCESS);
}
