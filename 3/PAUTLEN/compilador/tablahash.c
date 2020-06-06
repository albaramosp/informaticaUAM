/*
 * Implementacion de funciones de la tabla hash.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include "tablahash.h"

TablaHash crearTablaHash(){
    int i;
    TablaHash t=(TablaHash)malloc(TAMANO_HASH*sizeof(t[0]));

    if(!t)
        return t;
    for(i=0; i<TAMANO_HASH; i++)
        t[i]=NULL;

    return t;
}

void destruirTablaHash(TablaHash t){
    int i;
    
    for(i=0; i<TAMANO_HASH; i++){
        if(t[i] != NULL)
          free(t[i]);
    }

    free(t);
    t=NULL;
}

int hash(char *lexema){
   int valor;
   unsigned char *c;

   for (c=(unsigned char*)lexema, valor=0; *c; c++)
   	  valor += (int)*c;

   return (valor%TAMANO_HASH);
}

int getPosicion(TablaHash t, char *lexema){
   int ini, i, aux;

   ini=hash(lexema);

   for (i=0; i<TAMANO_HASH; i++) {
      aux=(ini+i)%TAMANO_HASH;
      if (t[aux] == NULL)
         return aux;

      if (!strcmp(t[aux]->lexema, lexema))
         return aux;
   }

   return ini;
}

int busqueda(TablaHash t, char *lexema){
   int pos=getPosicion(t, lexema);

   if (t[pos]==NULL)
      return 0;

   else
      return(!strcmp(t[pos]->lexema, lexema));
}

BOOL actualizarParamFuncion(TablaHash t, char *lexema, int numero_param){
  int pos;

  if (busqueda(t, lexema)){
    pos=getPosicion(t, lexema);
    t[pos]->numero_parametros=numero_param;

    return TRUE;
  }

  return FALSE;
}

BOOL insercion(TablaHash t, char *lexema, int categoria, int clase, int tipo, int tamano, int numero_variables_locales, int posicion_variable_local, int numero_parametros, int posicion_parametros){
   int pos;

   if (!busqueda(t, lexema)) {
       pos=getPosicion(t, lexema);
       if (t[pos]==NULL ) {
          t[pos]=(nodo*)malloc(sizeof(t[pos][0]));
          strcpy(t[pos]->lexema,lexema);
          t[pos]->categoria=categoria;
          t[pos]->clase=clase;
          t[pos]->tipo=tipo;
          t[pos]->tamano=tamano;
          t[pos]->numero_variables_locales=numero_variables_locales;
          t[pos]->posicion_variable_local=posicion_variable_local;
          t[pos]->numero_parametros=numero_parametros;
          t[pos]->posicion_parametros=posicion_parametros;
          return TRUE;
       } else {
          return FALSE;
       }
   }

   return FALSE;
}



int getCategoria(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->categoria;
}

int getTipo(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->tipo;
}

int getClase(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->clase;
}

int getTamano(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->tamano;
}

int getNParams(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->numero_parametros;
}

int getPosParam(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->posicion_parametros;
}

int getPosVar(TablaHash t, char *lexema){
   return  t[getPosicion(t, lexema)]->posicion_variable_local;
}
