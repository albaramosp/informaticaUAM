/**
 *
 * Descripcion: Implementación deFunciones de cabecera para Busqueda
 *
 * Fichero: ordenacion.h
 * Autor: Alba Ramos y Javier lozano
 * Version: 1.0
 * Fecha: 23-11-2018
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "busqueda.h"
#include <math.h>

/**
 *  Funciones de geracion de claves
 *
 *  Descripcion: Recibe el numero de claves que hay que generar
 *               en el parametro n_claves. Las claves generadas
 *               iran de 1 a max. Las claves se devuelven en
 *               el parametro claves que se debe reservar externamente
 *               a la funcion.
 */

/**
 *  Funcion: generador_claves_uniforme
 *               Esta fucnion genera todas las claves de 1 a max de forma
 *               secuencial. Si n_claves==max entonces se generan cada clave
 *               una unica vez.
 */
void generador_claves_uniforme(int *claves, int n_claves, int max)
{
  int i;

  for(i = 0; i < n_claves; i++) claves[i] = 1 + (i % max);

  return;
}


/***************************************************/
/* Funcion: ini_diccionario                        */
/* Autores:   Alba Ramos, Javier Lozano            */
/*                                                 */
/* Rutina que genera un diccionario                */
/* y devulve un puntero al mismo                   */
/*                                                 */
/* Entrada:                                        */
/* int tamanio: tamaño deñ diccionario             */
/* char orden: indica si el diccionario es         */
/* ordenado o no ordenado                          */
/* Salida: Puntero al diccionario                  */
/***************************************************/

PDICC ini_diccionario (int tamanio,char orden){
  PDICC dicc;

  if(tamanio < 1 || (orden != ORDENADO && orden != NO_ORDENADO)){
    return NULL;
  }

  dicc = (PDICC) calloc(1,sizeof(DICC));
  if (dicc == NULL){
    return NULL;
  }

  dicc->tamanio = tamanio;
  dicc->n_datos = 0;
  dicc->orden = orden;
  dicc->tabla = (int*) calloc(tamanio,sizeof(int));
  if(dicc->tabla == NULL){
    free(dicc);
    return NULL;
  }

  return dicc;
}

/***************************************************/
/* Funcion: libera_diccionario                     */
/* Autores:   Alba Ramos, Javier Lozano            */
/*                                                 */
/* Rutina que libera los recursos un diccionario   */
/* y devulve un puntero al mismo                   */
/*                                                 */
/* Entrada:                                        */
/* PDDIC tamanio: puntero al diccionario a liberar */
/***************************************************/

 void libera_diccionario(PDICC pdicc){
   if(pdicc == NULL){
     return;
   }

   if(pdicc->tabla != NULL){
     free(pdicc->tabla);
   }

   free(pdicc);
 }

 /***************************************************/
 /* Funcion: inserta_diccionario                    */
 /* Autores:   Alba Ramos, Javier Lozano            */
 /*                                                 */
 /* Rutina que inserta un elemento en               */
 /* un diccionario                                  */
 /*                                                 */
 /* Entrada:                                        */
 /* PDICC tamanio: puntero al diccionario a liberar */
 /* int clave: clave a introducir en diccionario    */
 /* Salida: operaciones basicas realizadas          */
 /***************************************************/

 int inserta_diccionario(PDICC pdicc,int clave){
   int i, obs;

   if(pdicc == NULL || clave < 0 || pdicc->tamanio == pdicc->n_datos){
     return ERR;
   }
   obs = 0;
   if(pdicc->orden == NO_ORDENADO){
     pdicc->tabla[pdicc->n_datos] = clave;
     pdicc->n_datos++;
   }
   else{
     pdicc->tabla[pdicc->n_datos] = clave;
     for(i = pdicc->n_datos-1; i>= 0 && pdicc->tabla[i] > clave; i--, obs++){
        pdicc->tabla[i+1] = pdicc->tabla[i];
     }

     pdicc->tabla[i+1] = clave;

     pdicc->n_datos++;

   }
   return obs;
 }

 /***************************************************/
 /* Funcion: insercion_masiva_diccionario           */
 /* Autores:   Alba Ramos, Javier Lozano            */
 /*                                                 */
 /* Rutina que inserta una coleccion de elementos   */
 /* en un diccionario                               */
 /*                                                 */
 /* Entrada:                                        */
 /* PDICC tamanio: puntero al diccionario a liberar */
 /* *int claves: tabla de claves a introducir       */
 /* int n_claves: numero de claves                  */
 /* Salida: operaciones basicas realizadas          */
 /***************************************************/

 int insercion_masiva_diccionario (PDICC pdicc,int *claves,int n_claves){
    int i, obs, aux;
    obs = 0;
    if( pdicc == NULL || !claves || n_claves <= 0 || n_claves>pdicc->tamanio-pdicc->n_datos){
      return ERR;
    }

    for (i = 0; i<n_claves; i++){
      if( (aux = inserta_diccionario(pdicc, claves[i])) == ERR){
        return ERR;
      }
      obs += aux;
    }
    return obs;
 }

 /**********************************************************/
 /* Funcion: busca_diccionario                             */
 /* Autores:   Alba Ramos, Javier Lozano                   */
 /*                                                        */
 /* Rutina que busca una clave en un diccionario           */
 /*                                                        */
 /* Entrada:                                               */
 /* PDICC tamanio: puntero al diccionario a liberar        */
 /* int clave: claves a introduci                          */
 /* int* ppos: devuelve la posicion en la que se           */
 /* la clve que se esta buscando                           */
 /* pfunc_busqueda metodo: metodo a usar para la busqueda  */
 /* Salida: operaciones basicas realizadas                 */
 /**********************************************************/

 int busca_diccionario(PDICC pdicc,int clave,int *ppos,pfunc_busqueda metodo){
   int obs;
   if( pdicc == NULL ||  clave < 0 || !ppos || !metodo || (metodo == bbin && pdicc->orden != ORDENADO)){
     return ERR;
   }


   if ((obs = metodo(pdicc->tabla,0,pdicc->n_datos-1,clave,ppos)) == ERR){
     return ERR;
   }
   return obs;
 }

/* Funciones de busqueda del TAD Diccionario */

 int bbin(int *tabla,int P,int U,int clave,int *ppos){
   int obs,m;
   obs =0;
   if( tabla == NULL || P < 0 || P > U || clave < 0 || !ppos ){
     return ERR;
   }

   while( P <= U ){
     m=(P+U)/2;
     obs++; /*la OB se realiza dos veces pero solo se debe contar una vez*/
     if(tabla[m] == clave){
       *ppos = m;
       /*obs++;*/
       return obs;
     } else if(tabla[m] > clave){
       U = m-1;
     }else {
       P=m+1;
     }
     /*obs++;*/
   }
   *ppos = NO_ENCONTRADO;
   return obs;
 }

 int blin(int *tabla,int P,int U,int clave,int *ppos){
   int obs=0, i;
   if( tabla == NULL || P < 0 || P > U || !ppos ){
     return ERR;
   }

   for ( i = P; i <= U; i++){
     obs++; /*vamos contando todas las OB incluida si la clave está en tabla[0]*/
     if(tabla[i] == clave){
       *ppos = i;
       return obs;
     }
   }
   *ppos = NO_ENCONTRADO;
   return obs;
 }
