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

/***************************************************/
/* Funcion: medio Fecha:02/11/2018                 */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que devuelve el elemento medio de       */
/* una tabla                                       */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int ip: índice del último elemento              */
/* int* pos: la posición del pivote                */
/* Salida:                                         */
/* operaciones básicas realizadas                  */
/* ERR (-1) en caso de error                       */
/***************************************************/
int medio(int *tabla, int ip, int iu,int *pos){
  if(tabla == NULL || ip < 0 || iu < 0 || pos == NULL){
    return ERR;
  }

  *pos = ip; /*posicion del pivote*/

  return 0;
}


/***************************************************/
/* Funcion: partir Fecha:02/11/2018                */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que reordena la tabla, dejando a la     */
/* izquierda del elemento pivote los elementos     */
/* menores, y a la derecha los mayores             */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int ip: índice del último elemento              */
/* int* pos: la posición del pivote                */
/* Salida:                                         */
/* posición del elemento medio de la tabla         */
/* ERR (-1) en caso de error                       */
/***************************************************/
int partir(int* tabla, int ip, int iu,int *pos) {
  int nummedio, k, i, obs;

  if(tabla == NULL || ip < 0 || iu < 0 || pos == NULL){
    return ERR;
  }

  obs = medio(tabla, ip, iu, pos);
  if(obs == ERR){
    return ERR;
  }

  k = tabla[*pos];/*pivote*/

  swap(tabla, ip, *pos);

  nummedio = ip;

  for(i = ip+1; i <= iu; i++, obs++){
    if (tabla[i] < k){
      nummedio++;
      swap(tabla, i, nummedio);
    }
  }

  swap(tabla, ip, nummedio);

	*pos = nummedio;

  return obs;

}


/***************************************************/
/* Funcion: quicksort Fecha:02/11/2018             */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que ordena una tabla de enteros         */
/*de menor a mayor                                 */
/* utilizando el algoritmo QuickSort               */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int iu: índice del último elemento              */
/* Salida:                                         */
/* numero de operaciones basicas que realiza el    */
/* algoritmo                                       */
/* ERR (-1) en caso de error                       */
/***************************************************/
int quicksort(int* tabla, int ip, int iu){
  int obs = 0, aux;
  int pos;

  if(tabla == NULL || ip < 0 || iu < 0 || ip > iu){
    return ERR;
  }


  if(ip == iu){
    return 0;
  }
  aux = partir(tabla, ip, iu,&pos);
	if(aux == ERR){
		return ERR;
	}

  obs += aux;

  if ( ip < pos-1){
	  aux = quicksort(tabla, ip, pos-1);
	  if(aux == ERR){
		  return ERR;
	  }
    obs += aux;
  }
   if ( pos+1 < iu){
	   aux = quicksort(tabla,pos+1, iu);
	   if(aux == ERR){
		   return ERR;
	   }
    obs += aux;
  }
  return obs;
}

/***************************************************/
/* Funcion: quicksort_src Fecha:13/11/2018         */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que ordena una tabla de enteros         */
/*de menor a mayor                                 */
/* utilizando el algoritmo QuickSort al que le 	   */
/* eliminamos la recursión de cola				         */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int iu: índice del último elemento              */
/* Salida:                                         */
/* numero de operaciones basicas que realiza el    */
/* algoritmo                                       */
/* ERR (-1) en caso de error                       */
/***************************************************/
int quicksort_src(int* tabla, int ip, int iu){
  int obs, aux;
  int pos;

  obs=0;

  if(tabla == NULL || ip < 0 || iu < 0 || ip > iu){
    return ERR;
  }


  if(ip == iu){
    return 0;
  }


  while(ip < iu){
    aux= partir(tabla, ip, iu,&pos);
  	if(aux == ERR){
  		return ERR;
  	}
    obs += aux;

    if(ip < pos-1){
      aux = quicksort_src(tabla, ip, pos-1);
  	  if(aux == ERR){
  		  return ERR;
  	  }

      obs += aux;
    }

    ip = pos+1;
  }

  return obs;
}

/***************************************************/
/* Funcion: MergeSort Fecha:26/10/2018             */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que ordena una tabla de enteros         */
/*de menor a mayor                                 */
/* utilizando el algoritmo MergeSort               */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int iu: índice del último elemento              */
/* Salida:                                         */
/* numero de operaciones basicas que realiza el    */
/* algoritmo                                       */
/* ERR (-1) en caso de error                       */
/***************************************************/
int mergesort(int* tabla, int ip, int iu){
  int mitad;
  int obs = 0;
  int aux = 0;

  if(tabla == NULL || ip > iu || ip < 0 || iu < 0){
    return ERR;
  }

  else if(ip == iu){
    return 0;
  }

  else{
    mitad = (ip + iu) / 2;
    if( (aux = mergesort(tabla, ip, mitad)) == ERR){
      return ERR;
    }
    obs += aux;

    if( (aux = mergesort(tabla, mitad+1, iu)) == ERR){
      return ERR;
    }
    obs += aux;
  }

  if( (aux = merge(tabla, ip, iu, mitad)) == ERR){
    return ERR;
  }
  obs += aux;

  return obs;
}

/***************************************************/
/* Funcion: merge Fecha:26/10/2018                 */
/* Autores: Alba Ramos, Javier Lozano              */
/*                                                 */
/* Funcion que combina subtablas en otra grande    */
/* para el algoritmo MergeSort                     */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: tabla de enteros                    */
/* int ip: índice del primer elemento              */
/* int ip: índice del último elemento              */
/* Salida:                                         */
/* OK(1) si todo salió bien                        */
/* ERR (-1) en caso de error                       */
/***************************************************/
int merge(int* tabla, int ip, int iu, int imedio){
  int *aux;
  int i, j, k;
  int obs = 0;

  if(tabla == NULL || ip < 0 || iu < 0){
    return ERR;
  }

  aux = (int *)calloc(iu-ip+1, sizeof(int));
  if(aux == NULL){
    return ERR;
  }

  for(i = ip, j = imedio+1, k = 0; i <= imedio && j <= iu; k++){
    if(tabla[i] < tabla[j]){
      aux[k] = tabla[i];
      i++;
    }
    else{
      aux[k] = tabla[j];
      j++;
    }
  }

  obs = k;

  if(i > imedio){
    for(; j <= iu; j++, k++){
      aux[k] = tabla[j];
    }
  }
  else if(j > iu){
    for(; i <= imedio; i++, k++){
      aux[k] = tabla[i];
    }
  }

  for(i = ip, k = 0; i <= iu; i++, k++){
    tabla[i] = aux[k];
  }

  free(aux);

  return obs;
}


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
