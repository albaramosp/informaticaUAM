/**
 *
 * Descripcion: Implementacion de funciones de ordenacion
 *
 * Fichero: ordenacion.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2017
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include "ordenacion.h"

#define ERR -1

/***************************************************/
/* Funcion: SelectSort Fecha:28/09/2018            */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que ordena una tabla de enteros         */
/*de menor a mayor                                 */
/* utilizando el algoritmo SelectSort              */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int ip: índice del último elemento              */
/* Salida:                                         */
/* int: veces que se ejecuta la OB                 */
/* ERR (-1) en caso de error                       */
/***************************************************/
int SelectSort(int* tabla, int ip, int iu)
{
  int veces_OB = 0;
  int i, j, min;

  if(ip < 0 || iu < ip || !tabla){
    return ERR;
  }

  for(i = ip; i < iu; i++){
    min = i;
    /*para cada elemento i, comparamos con los siguientes para ver si hay alguno menor y guardar su indice*/
    for(j = i+1; j <= iu; j++){
      veces_OB++;
      if(tabla[j] < tabla[min]){
        min = j;
      }
    }
      swap(tabla, i, min);
  }

  return veces_OB;
}

/***************************************************/
/* Funcion: swap       Fecha:28/09/2018            */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que intercambia dos elementos de        */
/* una tabla                                       */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int actual: índice del elemento i-esimo         */
/* int min: índice del elemento menor              */
/* Salida:                                         */
/* void                                            */
/***************************************************/
void swap (int* tabla, int actual, int min){
  int aux;

  if(tabla == NULL || actual < 0 || min < 0){
	  return;
  }


  aux = tabla[actual];
  tabla[actual] = tabla[min];
  tabla[min] = aux;

}

/***************************************************/
/* Funcion: SelectSortInv Fecha:28/09/2018         */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que ordena una tabla de enteros         */
/* de mayor a menor                                */
/* utilizando el algoritmo SelectSort              */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int ip: índice del último elemento              */
/* Salida:                                         */
/* int: veces que se ejecuta la OB                 */
/* ERR (-1) en caso de error                       */
/***************************************************/
int SelectSortInv(int* tabla, int ip, int iu){
  int veces_OB = 0;
  int i, j, max;

  if(ip < 0 || iu < ip || !tabla){
    return ERR;
  }

  for(i = ip; i < iu; i++){
    max = i;
    /*para cada elemento i, comparamos con los siguientes para ver si hay alguno mayor y guardar su indice*/
    for(j = i+1; j <= iu; j++){
      veces_OB++;
      if(tabla[j] > tabla[max]){
        max = j;
      }
    }
      swap(tabla, i, max);
  }

  return veces_OB;
}
