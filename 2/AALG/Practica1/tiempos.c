/**
 *
 * Descripcion: Implementacion de funciones de tiempo
 *
 * Fichero: tiempos.c
 * Autor: Carlos Aguirre Maeso
 * Version: 1.0
 * Fecha: 16-09-2017
 *
 */

#include "tiempos.h"
#include "ordenacion.h"
#include "permutaciones.h"
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>


/***************************************************/
/* Funcion: tiempo_medio_ordenacion                */
/* Fecha:   05/10/2018                             */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que almacena los tiempos de ejecución   */
/* de un algoritmo sobre una permutación           */
/*                                                 */
/* Entrada:                                        */
/* pfunc_ordena metodo: algoritmo de ordenacion    */
/* que se va a utilizar                            */
/* int n_perms: Numero de permutaciones            */
/* int N: Numero de elementos de cada              */
/* permutacion                                     */
/* PTIEMPO ptiempo: estructura para guardar        */
/* informacion del tiempo de ejecucion             */
/* Salida:                                         */
/* OK en caso de  que vaya todo bien               */
/* ERR(-1) en caso de error                        */
/***************************************************/
short tiempo_medio_ordenacion(pfunc_ordena metodo,
                              int n_perms,
                              int N,
                              PTIEMPO ptiempo)
{
  int ob_min, ob_max, i, veces_ob;
  int** perms = NULL;
  int* perm_actual = NULL;
  double t0, t1, ob_prom;

  if(n_perms < 1 || N < 1 || metodo==NULL || ptiempo==NULL){
    return ERR;
  }

  ob_min = INT_MAX;
  ob_max = INT_MIN;
  ob_prom = 0.0;


  perms = genera_permutaciones(n_perms, N);
  if(perms == NULL){
    return ERR;
  }

  t0 = clock ();
  for(i = 0; i < n_perms; i++){
    perm_actual = perms[i];
    veces_ob = metodo(perm_actual, 0, N-1);
    if(veces_ob < 0){
      for (i=0; i<n_perms; i++){
        if (perms[i]){
          free(perms[i]);
        }
      }

			free(perms);
			return ERR;
    }

    ob_prom += veces_ob;
    if(veces_ob > ob_max){
      ob_max = veces_ob;
    }
    else if(veces_ob < ob_min){
      ob_min = veces_ob;
    }

    free(perms[i]);
  }

  t1 = clock();

  ptiempo->max_ob = ob_max;
  ptiempo->min_ob = ob_min;
  ptiempo->tiempo = ((t1-t0)/CLOCKS_PER_SEC)/n_perms;
  ptiempo->medio_ob = ob_prom/n_perms;
  ptiempo->N = N;
  ptiempo->n_elems = n_perms;

  free(perms);
  return OK;
}


/***************************************************/
/* Funcion: genera_tiempos_ordenacion              */
/* Fecha:   05/10/2018                             */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que genera un array con los tiempos     */
/* de cada una de las ejecuciones del algoritmo de */
/* ordenacion pasado por argumento                 */
/*                                                 */
/* Entrada:                                        */
/* pfunc_ordena metodo: algoritmo de ordenacion    */
/* que se va a utilizar                            */
/* int n_perms: Numero de permutaciones            */
/* int incr: incremento de N                       */
/* int num_max: cota superior de N                 */
/* int num_min: primer numero que tomara N         */
/* char *fichero: nombre del fichero donde guardar */
/* los datos de los tiempos                        */
/* Salida:                                         */
/* OK en caso de  que vaya todo bien               */
/* ERR(-1) en caso de error                        */
/***************************************************/
short genera_tiempos_ordenacion(pfunc_ordena metodo, char* fichero,
                                int num_min, int num_max,
                                int incr, int n_perms)
{
  PTIEMPO tiempos=NULL;
  int i, N;
  int vueltas;

  if(metodo == NULL || fichero == NULL || num_min < 0 || num_max < 1 || incr < 1 || n_perms < 1){
    return ERR;
  }

  vueltas = ((num_max - num_min)/incr)+1;

  tiempos = (PTIEMPO)calloc(vueltas,sizeof(TIEMPO));
  if(tiempos == NULL){
    return ERR;
  }

  for(i = 0, N=num_min; i < vueltas && N<=num_max; i++, N+=incr){

    if(tiempo_medio_ordenacion(metodo, n_perms, N, &tiempos[i]) == ERR){
      free(tiempos);
      return ERR;
    }

  }

  if(guarda_tabla_tiempos(fichero, tiempos, vueltas) == ERR){
    free(tiempos);
    return ERR;
  }

  free(tiempos);

  return OK;
}


/***************************************************/
/* Funcion: guarda_tabla_tiempos Fecha:            */
/* Fecha:   05/10/2018                             */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que escribe en un fichero los tiempos   */
/* guardados en el array que se recibe             */
/*                                                 */
/* Entrada:                                        */
/* PTIEMPO ptiempo: array de tiempos a imprimir    */
/* char *fichero: nombre del fichero donde guardar */
/* int n_tiempos: numero de elementos del array    */
/* Salida:                                         */
/* OK en caso de  que vaya todo bien               */
/* ERR(-1) en caso de error                        */
/***************************************************/
short guarda_tabla_tiempos(char* fichero, PTIEMPO tiempo, int n_tiempos)
{
    FILE* f;
    int i;

    if(fichero == NULL || tiempo == NULL || n_tiempos < 1){
      return ERR;
    }

    f = fopen(fichero, "w");
    if(f == NULL){
      return ERR;
    }

    fprintf(f, " N \t Tiempo \t OB promedio \t OB maximo \t OB minimo \n");
    for(i = 0; i < n_tiempos; i++){
      fprintf(f, " %d \t %f \t %.0f \t\t %d \t\t %d \n", tiempo[i].N, tiempo[i].tiempo, tiempo[i].medio_ob, tiempo[i].max_ob, tiempo[i].min_ob);
    }

    fclose(f);

    return OK;
}
