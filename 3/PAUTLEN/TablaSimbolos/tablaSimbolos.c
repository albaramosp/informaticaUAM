
/*
 * Fichero que implementa la tabla de símbolos del compilador de acuerdo
 * a las especificaciones de tablaSimbolos.h.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include "tablaSimbolos.h"

TABLA_SIMBOLOS *crearTablaSimbolos(){
  TABLA_SIMBOLOS *t;

  if( (t = calloc(1, sizeof( TABLA_SIMBOLOS ))) == NULL ) {
  	return NULL;
  }

  if ( (t->tablaGlobal = crear_tabla(MAX_TABLA)) == NULL ){
    return NULL;
  }

  return t;
}

void borrarTablaSimbolos(TABLA_SIMBOLOS *t){
  if (t != NULL){
    if (t->tablaLocal != NULL){
      liberar_tabla(t->tablaLocal);
    }

    if (t->tablaGlobal != NULL){
      liberar_tabla(t->tablaGlobal);
    }

    free(t);
  }
}

Status aperturaAmbitoLocal(TABLA_SIMBOLOS *t, const char *id, int tipo, int adic1, int adic2){
  if (insertar_simbolo(t->tablaGlobal, id, FUNCION, -1, tipo, -1, adic1, adic2) == ERROR){
    return ERROR;
  }

  if ( (t->tablaLocal = crear_tabla(MAX_TABLA)) == NULL ){
    return ERROR;
  }

  if (insertar_simbolo(t->tablaLocal, id, FUNCION, -1, tipo, -1, adic1, adic2) == ERROR){
    return ERROR;
  }

  return OK;
}

Status cierreAmbitoLocal(TABLA_SIMBOLOS *t){
  if (t->tablaLocal != NULL){
    liberar_tabla(t->tablaLocal);
    t->tablaLocal = NULL;

    return OK;
  }

  return ERROR;
}

Status insercion(TABLA_SIMBOLOS *t, char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adicional1, int adicional2){
  if(t->tablaLocal != NULL){
    return insertar_simbolo(t->tablaLocal, lexema, categoria, clase, tipo, -1, adicional1, adicional2);
  } else {
    return insertar_simbolo(t->tablaGlobal, lexema, categoria, clase, tipo, -1, adicional1, adicional2);
  }
}

int busqueda(TABLA_SIMBOLOS *t, char *id){
  INFO_SIMBOLO *res=NULL;

  if(t->tablaLocal != NULL){
    res = buscar_simbolo(t->tablaLocal, id);

    if (res != NULL){
      return res->tipo;
    }
  }

  res = buscar_simbolo(t->tablaGlobal, id);

  if (res != NULL){
    return res->tipo;
  }


  return -1;
}
