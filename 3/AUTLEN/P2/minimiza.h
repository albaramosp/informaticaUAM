/*
 * minimiza.h
 * Libreria que contiene la definicion de estructuras y funciones necesarias para realizar la
 * minimización del AFD.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#ifndef MINIMIZA_H
#define MINIMIZA_H

#include "transforma.h"

/*
 * Tipos de celdas en la matriz de indistinguibilidad.
 */
enum Celda{INDIST, NADA};

/*
 * Estructura para almacenar los indices de estados que sean
 * indistinguibles. Como maximo, una clase contendra a todos
 * los estados del automata inicial.
 */
typedef struct {
  int *estados;
  int contEstados;
} Clase;

/*
 * Estructura para almacenar una serie de estados indistinguibles,
 * es decir, clases. Como maximo, puede haber tantas clases como estados
 * tenga el automata inicial.
 */
typedef struct {
  Clase *clases;
  int contClases;
} Conjunto;

/*
 * Función que obtiene el automata finito determinista minimo
 * equivalente al que se recibe por argumento.
 * @param origen AFND
 * @return AFD minimizado
 */
AFND *minimiza (AFND *origen);
#endif
