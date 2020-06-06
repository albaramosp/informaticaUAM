/*
 * Fichero que implementa las funciones definidas en la librería hash.h
 * que permite crear y manipular tablas hash que sigan la estructura
 * necesaria para implementar la tabla de símbolos mediante estas tablas
 * hash, según las explicaciones vistas en las diapositivas de clase.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "hash.h"


/*Función que inicializa el campo info de un nodo */
INFO_SIMBOLO *crear_info_simbolo(const char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adicional1, int adicional2){
  INFO_SIMBOLO *info;

  info = (INFO_SIMBOLO *)calloc(1, sizeof(INFO_SIMBOLO));
  strcpy(info->lexema, lexema);
  info->categoria = categoria;
  info->tipo = tipo;
  info->clase = clase;
  info->tam = tam;
  info->adicional1 = adicional1;
  info->adicional2 = adicional2;

  return info;
}

/*Función que libera toda la información relacionada con un simbolo en los nodos de la tabla Hash*/
void liberar_info_simbolo(INFO_SIMBOLO *info){
  free(info);
  info = NULL;
}

NODO_HASH *crear_nodo(INFO_SIMBOLO *info){
  NODO_HASH *nodo = (NODO_HASH *)calloc(1, sizeof(NODO_HASH));

  nodo->info = crear_info_simbolo(info->lexema, info->categoria, info->clase, info->tipo, info->tam, info->adicional1, info->adicional2);
  /*nodo->info = info;*/
  nodo->siguiente = NULL;

  return nodo;
}

/*Función que libera la memoria reservada para un nodo*/
void liberar_nodo(NODO_HASH *nodo){
  /*Deberíamos poder liberar el info sin problema:C, MI NO ENTENDER ILLOOOO*/
  /*liberar_info_simbolo(nodo->info);*/
  free(nodo);
  nodo = NULL;
}

/*Función que inicializa la estructura de una tablas hash*/
TABLA_HASH *crear_tabla(int tam){
  TABLA_HASH *t = NULL;
  int i;

  if( tam < 1 ) return NULL;

  if( ( t = calloc(1, sizeof( TABLA_HASH )) ) == NULL ) {
  	return NULL;
  }

  if( ( t->tabla = calloc(tam, sizeof( NODO_HASH * )) ) == NULL ) {
  	return NULL;
  }

  for( i = 0; i < tam; i++ ) {
  	t->tabla[i] = NULL;
  }

  t->tam = tam;

  return t;
}

/*Funcion que libera toda la información relacionada con la tabla hash*/
void liberar_tabla(TABLA_HASH *th){
  int i;

  for (i = 0; i < th->tam; i++){
    liberar_nodo(th->tabla[i]);
  }

  free(th->tabla);

  free(th);
}

unsigned long hash(TABLA_HASH *th, const char *str){
  unsigned long int hashval = 1234;
  int i = 0;

  while( hashval < ULONG_MAX && i < strlen( str ) ) {
  	hashval = hashval << 8;
  	hashval += str[ i ];
  	i++;
  }

  return hashval % th->tam;
}

INFO_SIMBOLO *buscar_simbolo(TABLA_HASH *th, const char *lexema){
  unsigned long valor = 0;
  NODO_HASH *nodo;

  valor = hash(th, lexema);

  nodo = th->tabla[valor];

  while(nodo != NULL &&
        nodo->info != NULL &&
        strcmp(lexema, nodo->info->lexema) > 0) {

    nodo = nodo->siguiente;
  }

  if( nodo == NULL ||
      nodo->info == NULL ||
      strcmp(lexema, nodo->info->lexema) != 0) {
    return NULL;

  } else {
    return nodo->info;
  }
}

Status insertar_simbolo(TABLA_HASH *th, const char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adic1, int adic2){
  int valor = 0;
  NODO_HASH *siguiente = NULL;
  NODO_HASH *ultimo = NULL;

	valor = hash(th, lexema);
	siguiente = th->tabla[valor];

	while(siguiente != NULL &&
        siguiente->info != NULL &&
        strcmp(lexema, siguiente->info->lexema) > 0 ) {
		ultimo = siguiente;
		siguiente = siguiente->siguiente;
	}

	/*El lexema a insertar ya estaba en la tabla: insercion sin exito*/
	if( siguiente != NULL &&
      siguiente->info != NULL &&
      strcmp(lexema, siguiente->info->lexema) == 0 ) {

		return ERROR;

	/* El lexema no está, lo insertamos */
	} else {
    INFO_SIMBOLO *info = NULL;
    NODO_HASH *nuevo = NULL;

    info = crear_info_simbolo(lexema, categoria, clase, tipo, tam, adic1, adic2);
    nuevo = crear_nodo(info);
    liberar_info_simbolo(info);

		/* Estamos al principio de la tabla */
		if(siguiente == th->tabla[valor] ) {
			nuevo->siguiente = siguiente;
			th->tabla[valor] = nuevo;

		/* Estamos al final de la tabla */
  } else if ( siguiente == NULL ) {
			ultimo->siguiente = nuevo;

		/* Estamos por el medio de la tabla */
		} else  {
			nuevo->siguiente = siguiente;
			ultimo->siguiente = nuevo;
		}
	}

  return OK;
}

void borrar_simbolo(TABLA_HASH *th, const char *lexema){
  int valor = 0;
  NODO_HASH *siguiente = NULL;
  NODO_HASH *ultimo = NULL;

	valor = hash(th, lexema);
	siguiente = th->tabla[valor];

	while(siguiente != NULL &&
        siguiente->info != NULL &&
        strcmp(lexema, siguiente->info->lexema) > 0 ) {
		ultimo = siguiente;
		siguiente = siguiente->siguiente;
	}

	/*El lexema está en la tabla, podemos borrar*/
	if( siguiente != NULL &&
      siguiente->info != NULL &&
      strcmp(lexema, siguiente->info->lexema) == 0 ) {
        NODO_HASH *aux = siguiente;
        /*AQUI SI BORRAS EL SIMBOLO DEL NODO ANTES NO HAY NINGUN PROBLEMAA*/
        liberar_info_simbolo(siguiente->info);
        liberar_nodo(siguiente);

        /* Estamos al principio de la tabla */
    		if(aux == th->tabla[valor] ) {
    			th->tabla[valor] = aux->siguiente;

    		/* Estamos al final de la tabla */
      } else if (aux->siguiente == NULL) {
  			ultimo->siguiente = NULL;

  		/* Estamos por el medio de la tabla */
  		} else  {
        ultimo->siguiente = aux->siguiente;
  		}
	/* El lexema no está, así que no hay que borrar nada */
	}
}
