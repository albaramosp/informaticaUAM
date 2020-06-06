/**
 * @brief Libreria en la que se encuentran los prototipos de las funciones de una cola circular
 * @file queue.h
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */

#ifndef QUEUE_H
#define QUEUE_H
#include "types.h"

#define MAXQUEUE 10

typedef struct _Queue Queue;

/**------------------------------------------------------------------
Inserta un nuevo nodo en la cola, reservando memoria nueva para él y haciendo una copia del elemento recibido.
------------------------------------------------------------------*/
Status queue_insert(Queue *q, char c);

/**------------------------------------------------------------------
Extrae un elemento de la cola. Devuelve directamente el puntero al elemento (no hace copia).
------------------------------------------------------------------*/
char queue_extract(Queue *q);

void queue_setFront(Queue *q, int i);

void queue_setRear(Queue *q, int i);

#endif /* QUEUE_H */
