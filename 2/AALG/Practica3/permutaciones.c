/**
 *
 * Descripcion: Implementacion de funciones de generacion de permutaciones
 *
 * Fichero: permutaciones.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2017
 *
 */


#include "permutaciones.h"
#include <stdlib.h>
#include <stdio.h>

/***************************************************/
/* Funcion: aleat_num Fecha: 21/09/2018            */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Rutina que genera un numero aleatorio           */
/* entre dos numeros dados                         */
/*                                                 */
/* Entrada:                                        */
/* int inf: limite inferior                        */
/* int sup: limite superior                        */
/* Salida:                                         */
/* int: numero aleatorio                           */
/***************************************************/
int aleat_num(int inf, int sup)
{
	int result;
	double cte;

	if(inf < 0 || sup < 0){
			return -1;
	}

	cte = (RAND_MAX+1.0)/((sup-inf)+1);
	result = inf+(rand()/cte);

	return result;
}

/***************************************************/
/* Funcion: genera_perm Fecha: 21/09/2018          */
/* Autores:   Alba Ramos, Javier Lozano            */
/*                                                 */
/* Rutina que genera una permutacion               */
/* aleatoria                                       */
/*                                                 */
/* Entrada:                                        */
/* int n: Numero de elementos de la                */
/* permutacion                                     */
/* Salida:                                         */
/* int *: puntero a un array de enteros            */
/* que contiene a la permutacion                   */
/* o NULL en caso de error                         */
/***************************************************/
int* genera_perm(int N)
{
	int* tabla;
	int i, aux, random;

	if(N <= 0){
		return NULL;
	}

	tabla = (int*)calloc(N,sizeof(int));
	if(tabla == NULL){
		return NULL;
	}

	for(i = 0; i < N; i++){
		tabla[i] = i+1;
	}

	for(i = 0; i < N; i++){
		aux = tabla[i];
		random = aleat_num(i, N-1);
		if(random == -1){
			free(tabla);
			return NULL;
		}

		tabla[i] = tabla[random];
		tabla[random] = aux;
	}
	return tabla;
}

/***************************************************/
/* Funcion: genera_permutaciones Fecha:: 21/09/2018*/
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que genera n_perms permutaciones        */
/* aleatorias de tamanio elementos                 */
/*                                                 */
/* Entrada:                                        */
/* int n_perms: Numero de permutaciones            */
/* int N: Numero de elementos de cada              */
/* permutacion                                     */
/* Salida:                                         */
/* int**: Array de punteros a enteros              */
/* que apuntan a cada una de las                   */
/* permutaciones                                   */
/* NULL en caso de error                           */
/***************************************************/
int** genera_permutaciones(int n_perms, int N)
{
	int** tablas;
	int i;

	if(N <= 0 || n_perms <= 0){
		return NULL;
	}

	tablas = (int**)malloc(n_perms*sizeof(int*));
	if(tablas == NULL){
		return NULL;
	}

	for(i = 0; i < n_perms; i++){
		tablas[i] = genera_perm(N);
		if(tablas[i] == NULL){
			for(i=i-1; i >= 0; i--){
				free(tablas[i]);
			}

			free(tablas);
			return NULL;
		}
	}

	return tablas;
}
