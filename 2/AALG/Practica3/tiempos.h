/**
 *
 * Descripcion: Funciones de cabecera para ordenacion
 *
 * Fichero: tiempos.h
 * Autor: Carlos Aguirre.
 * Version: 1.0
 * Fecha: 15-09-2016
 *
 */

#ifndef TIEMPOS_H
#define TIEMPOS_H

/* constantes */

#ifndef ERR
  #define ERR -1
  #define OK (!(ERR))
#endif

#include "busqueda.h"

/* definiciones de tipos */
typedef struct tiempo {
  int N;           /* tamanio los elementos */
  int n_elems;     /* numero de elementos a promediar */
  double tiempo;   /* tiempo promedio */
  double medio_ob; /* numero premedio de veces que se ejecuta la OB */
  int min_ob;      /* minimo de ejecuciones de la OB */
  int max_ob;      /* maximo de ejecuciones de la OB */
} TIEMPO, *PTIEMPO;


/* Funciones */
short tiempo_medio_ordenacion(pfunc_busqueda metodo, int n_perms,int N, PTIEMPO ptiempo);
short genera_tiempos_ordenacion(pfunc_busqueda metodo, char* fichero, int num_min, int num_max, int incr, int n_perms);
short guarda_tabla_tiempos(char* fichero, PTIEMPO tiempo, int n_tiempos);
short genera_tiempos_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador,
                                int orden, char* fichero,
                                int num_min, int num_max,
                                int incr, int n_veces);

short tiempo_medio_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador,
                              int orden,
                              int N,
                              int n_veces,
                              PTIEMPO ptiempo);
#endif
