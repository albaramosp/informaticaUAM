/**
 *
 * Descripcion: Funciones de cabecera para ordenacion
 *
 * Fichero: ordenacion.h
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2016
 *
 */

#ifndef ORDENACION_H
#define ORDENACION_H

/* constantes */

#ifndef ERR
  #define ERR -1
  #define OK (!(ERR))
#endif

/* definiciones de tipos */
typedef int (* pfunc_ordena)(int*, int, int);

/* Funciones */

int SelectSort(int* tabla, int ip, int iu);
int SelectSortInv(int* tabla, int ip, int iu);
int mergesort(int* tabla, int ip, int iu);
int merge(int* tabla, int ip, int iu, int imedio);
void swap (int* tabla, int actual, int min);
int partir(int* tabla, int ip, int iu,int *pos);
int quicksort(int* tabla, int ip, int iu);
int quicksort_src(int* tabla, int ip, int iu);
int medio(int *tabla, int ip, int iu,int *pos);


#endif
