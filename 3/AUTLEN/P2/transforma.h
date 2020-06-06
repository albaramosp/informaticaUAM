/*
 * transforma.h
 * Libreria que contiene la definicion de estructuras y macros necesarias para realizar la funcionalidad
 * de transformacion del AFND.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#ifndef TRANSFORMA_H
#define TRANSFORMA_H

#include "afnd.h"

#define MAX_NAME 30
#define MAX_INDEX 30

/*
 * Estructura para almacenar la informacion de un estado simple.
 * Estos estados estan formados por un nombre y un indice de estado.
 * Esta estructura se utilizara cuando un estado sea individual, es decir, no este compuesto de otros.
 */
typedef struct  {
  char nombre[MAX_NAME];
  int indice;
} Estado;

/*
 * Estructura para almacenar la informacion de un estado compuesto de otros estados.
 * El nombre de estos estados es una cadena con el nombre de todos los estados de los que se compone.
 * El estado tambien almacena los indices de los estados que lo componen.
 * Esta estructura permite agrupar en un estado determinista varios estados a la vez.
 * En este tipo de estados, al analizar las transiciones desde ellos, habra que recorrer todos los indices para ver
 * todas las posibilidades desde todos los estados que lo componen.
 */
typedef struct  {
  char nombre[MAX_NAME];
  int indices[MAX_INDEX];
  int contInd; /**< numero de indices de los que se compone el estado */
  int tipo; /**< tipo del estado: final, inicial, normal o inicial_final */
} EstadoCompuesto;

/*
 * Estructura para almacenar la informacion de una celda de la matriz determinista.
 * Permite conocer si una celda, formada por uno o varios estados (es decir, un estado compuesto),
 * ha sido visitada ya o no. Esto facilita el algoritmo que recorre los estados del AFD y que termina
 * cuando todas las celdas han sido visitadas.
 */
typedef struct  {
  EstadoCompuesto destino; /**< estado de destino al que se transita desde la celda */
  int visitado; /**< indica si la celda esta ya visitada, 1, o no, 0 */
} Celda;

/*
 * Funcion que obtiene un array de cadenas con los nombres de los estados de un automata
 * @param a puntero al automata del que obtener los nombres de estados
 * @return array de cadenas con los nombres de los estados
 */
char ** nombreEstados(AFND * a);

/*
 * Funcion que obtiene un array de estados a los que un automata transita dado un estado de partida y un simbolo de entrada
 * @param a puntero al automata del que obtener las transiciones
 * @param pos_estado indice del estado desde el que obtener transiciones
 * @param simbolo simbolo a partir del cual ver si se transita
 * @param lambda entero que, si vale 1, indica que se quieren obtener las transiciones lambda y, si vale 0, las normales
 * @return array de estados a los que se transita desde ese estado ante ese simbolo
 */
Estado* conexiones(AFND *a, int pos_estado, char *simbolo, int lambda);

/*
 * Funcion que obtiene un automata finito determinista equivalente al no determinista que recibe
 * @param a puntero al automata del que obtener el equivalente
 * @return automata determinista equivalente
 */
AFND *transforma (AFND *p_afnd);
#endif
