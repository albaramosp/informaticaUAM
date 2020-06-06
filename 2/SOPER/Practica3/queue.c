
/**
 * @brief Programa en el que se encuentran la funciones de una cola circular
 * @file queue.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "queue.h"

struct _Queue {
  char items [MAXQUEUE];
  int front;
  int rear;
};

/**------------------------------------------------------------------
Inserta un nuevo nodo en la cola, reservando memoria nueva para él y haciendo una copia del elemento recibido.
------------------------------------------------------------------*/
Status queue_insert(Queue *q, char c){
  if(!q) return ERROR;

  q->items[q->rear] = c;

  q->rear=(q->rear + 1) % MAXQUEUE;

  return OK;
}

/**------------------------------------------------------------------
Extrae un elemento de la cola. Devuelve directamente el puntero al elemento (no hace copia).
------------------------------------------------------------------*/
char queue_extract(Queue *q){

  char pe = '\0';

  pe = q->items[q->front];

  q->items[q->front]='\0';

  q->front = (q->front+1) % MAXQUEUE;

  return pe;
}

void queue_setFront(Queue *q, int i){
  if (!q){
    return;
  }

  q->front = i;
}

void queue_setRear(Queue *q, int i){
  if (!q){
    return;
  }

  q->rear = i;
}
