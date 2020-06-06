/*
 * minimiza.c
 * Programa que implementa las funciones de la libreria minimiza.h para
 * construir el AFD minimo equivalente al original. Si bien un AFD no es
 * lo mismo que un AFND, la estructura de ambos es la misma y, por tanto,
 * se reutiliza.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minimiza.h"

/*Maxima longitud que puede tener un nombre de estado*/
#define LEN 100

/*
 * Indices para bucles. Se ponen globales por eficiencia, ya que
 * van a ser usados frecuentemente a lo largo de todo el codigo.
 */
int i, j, k, l, m;

AFND *minimo;

/*
 * Matriz de transiciones del AFD inicial. Para construirla
 * se han reutilizado funciones desarrolladas en la P1.
 */
Estado ***transicionesIniciales;
Estado *e; /*Estado que se guarda en cada celda de la matriz*/
int tam, filas, cols;

/*Matriz de indistinguibilidad del algoritmo de minimizacion*/
int **matriz;

/*Conjuntos del algoritmo*/
Conjunto conjuntoActual;
Conjunto conjuntoPrevio;

/*Parejas de estados necesarios para hacer las parejas del algoritmo*/
int e1;
int e2;

/*Variables para comprobar si las parejas de estados transitan a la misma clase*/
int contMismaClase;
int n, clase;

/*Numero de estados accesibles del AFD minimo*/
int contAccesibles = 0;

/*
 * Funcion que obtiene un array de cadenas con los nombres de los estados de un automata
 * @param a puntero al automata del que obtener los nombres de estados
 * @return array de cadenas con los nombres de los estados
 */
char ** nombreEstados(AFND * a){
  int nEstados;
  char **estados;
  int i;

  nEstados = AFNDNumEstados(a);
  estados = (char **)calloc(nEstados, sizeof(char*));

  for (i = 0; i < nEstados; i++){
    estados[i] = (char *)calloc(MAX_NAME, sizeof(char));
    strcpy(estados[i], AFNDNombreEstadoEn(a, i));
  }

  return estados;
}

/*
 * Funcion que obtiene un array de estados a los que un automata transita dado un estado de partida y un simbolo de entrada
 * @param a puntero al automata del que obtener las transiciones
 * @param pos_estado indice del estado desde el que obtener transiciones
 * @param simbolo simbolo a partir del cual ver si se transita
 * @param lambda entero que, si vale 1, indica que se quieren obtener las transiciones lambda y, si vale 0, las normales
 * @return array de estados a los que se transita desde ese estado ante ese simbolo
 */
Estado* getConexiones(AFND *a, int pos_estado, char *simbolo, int lambda){
  int j;
  int cont = 0;
  Estado *conexiones;
  int numEstados = AFNDNumEstados(a);
  int indiceSimbolo;
  char **estados = nombreEstados(a);


  if (lambda == 0){
    indiceSimbolo = AFNDIndiceDeSimbolo(a, simbolo);
  }

  conexiones = (Estado*)calloc(numEstados, sizeof(Estado));

  for (j = 0; j < numEstados; j++){
    int indiceDest = AFNDIndiceDeEstado(a, estados[j]);

    if (lambda == 0){
      if (AFNDTransicionIndicesEstadoiSimboloEstadof(a, pos_estado, indiceSimbolo, indiceDest) == 1){
        strcpy(conexiones[cont].nombre, estados[j]);
        conexiones[cont].indice = indiceDest;
        cont++;
      }
    } else {
      if (AFNDCierreLTransicionIJ(a, pos_estado, indiceDest) == 1
          && strcmp(AFNDNombreEstadoEn(a, pos_estado), AFNDNombreEstadoEn(a, indiceDest)) != 0){
          strcpy(conexiones[cont].nombre, estados[j]);
          conexiones[cont].indice = indiceDest;
          cont++;
      }
    }
  }

  for (j = 0; j < numEstados; j++){
    free(estados[j]);
  }

  free(estados);

  if (cont == 0){
    free(conexiones);
    return NULL;
  }

  return conexiones;
}

/*
 * Funcion que genera la matriz de transiciones de un AFD.
 * @param origen AFD del que sacar las transiciones
 */
void generaMatrizTransiciones(AFND *origen){
  transicionesIniciales = (Estado ***)calloc(filas, sizeof(Estado **));

  for (i = 0; i < filas; i++){
    transicionesIniciales[i] = (Estado **)calloc(cols, sizeof(Estado *));
  }

	for (i = 0; i < filas; i++){
		for (j = 0; j < cols; j++){
			e = getConexiones(origen, i, AFNDSimboloEn(origen, j), 0);
      transicionesIniciales[i][j] = e;
		}
	}
}

/*
 * Funcion que libera los recursos asocaidos a una matriz de transiciones.
 */
void liberaMatrizTransiciones(){
  for (i = 0; i < filas; i++){
    for (j = 0; j < cols; j++){
      free(transicionesIniciales[i][j]);
    }

    free(transicionesIniciales[i]);
  }
  free(transicionesIniciales);
}

/*
 * Funcion que genera una matriz de estados indistinguibles.
 */
void generaMatrizIndist(){
  matriz = (int **)calloc(tam, sizeof(int *));

  for (i = 0; i < tam; i++){
    matriz[i] = (int *)calloc(tam, sizeof(int));
  }

  for (i = 0; i < tam; i++){
    for (j = 0; j < tam; j++){
      matriz[i][j] = NADA;
    }
  }
}

/*
 * Funcion que libera una matriz de estados indistinguibles.
 */
void liberaMatrizIndist(){
  for (i = 0; i < tam; i++){
    free(matriz[i]);
  }

  free(matriz);
}

/*
 * Funcion que genera un conjunto de clases. Como maximo, tendra tantas
 * clases como estados tenga el AFD, las cuales tendran como maximo tantos
 * estados como tenga el AFD. el conjunto se devuelve porque puede asociarse
 * a distintos conjuntos en cada momento, no a una variable sola.
 * @return conjunto creado
 */
Conjunto reservaConjunto(){
  Conjunto conjunto;

  /*Como mucho habrá tantas clases como estados*/
  conjunto.clases = (Clase *)calloc(tam, sizeof(Clase));
  conjunto.contClases = 0;

  /*Y en cada clase como mucho habrá todos los estados*/
  for (i = 0; i < tam; i++){
    conjunto.clases[i].estados = (int *)calloc(tam, sizeof(int));
    conjunto.clases[i].contEstados = 0;
  }

  return conjunto;
}

/*
 * Función que copia el contenido de un conjunto en otro.
 * @param origen conjunto a copiar
 * @return conjunto copia del original
 */
Conjunto copiarConjunto(Conjunto origen){
  Conjunto conjunto = reservaConjunto();

  for (i = 0; i < origen.contClases; i++){
    for (j = 0; j < origen.clases[i].contEstados; j++){
      conjunto.clases[i].estados[j] = origen.clases[i].estados[j];
    }

    conjunto.clases[i].contEstados = origen.clases[i].contEstados;
  }

  conjunto.contClases = origen.contClases;

  return conjunto;
}

/*
 * Funcion que libera los recursos asociados a un conjunto.
 * @param conjunto a liberar
 */
void liberaConjunto(Conjunto conjunto){
  if (conjunto.clases != NULL){
    for (i = 0; i < tam; i++){
      if (conjunto.clases[i].estados != NULL){
        free(conjunto.clases[i].estados);
      }

    }

    free(conjunto.clases);
  }

}

/*
 * Funcion que rellena el conjunto actual inicial con dos
 * clases: estados finales - o iniciales y finales - y resto.
 * @param origen automata original para ver el tipo de estado
 */
void rellenaConjuntoInicial(AFND *origen){
  conjuntoActual.contClases = 2;

  for (i = 0; i < tam; i++){
    if (AFNDTipoEstadoEn(origen, i) == FINAL || AFNDTipoEstadoEn(origen, i) == INICIAL_Y_FINAL){
      conjuntoActual.clases[0].estados[conjuntoActual.clases[0].contEstados] = i;
      conjuntoActual.clases[0].contEstados++;
    } else {
      conjuntoActual.clases[1].estados[conjuntoActual.clases[1].contEstados] = i;
      conjuntoActual.clases[1].contEstados++;
    }
  }
}

/*
 * Funcion que comprueba a dónde transita cada estado de una pareja con cada símbolo.
 * @param destinos array donde guardar los destinos de cada miembro de la pareja
 */
void verDestinosPareja(int **destinos){
  /*vemos las transiciones del primer miembro*/
  for (m = 0; m < cols; m++){
    if (transicionesIniciales[e1][m] != NULL){
      destinos[0][m] = transicionesIniciales[e1][m][0].indice;
    }
  }

  /*vemos las transiciones del segundo miembro*/
  for (m = 0; m < cols; m++){
    if (transicionesIniciales[e2][m] != NULL){
      destinos[1][m] = transicionesIniciales[e2][m][0].indice;
    }
  }
}

/*
 * Funcion que comprueba si dos estados son indistinguibles. Para ello,
 * comprueba si, para cada simbolo, ambos transitan a la misma clase.
 * @param destinos array con los destinos de cada miembro de la pareja
 */
int isParejaIndist(int **destinos){
  /*Vemos si coinciden las clases de ambos para cada simbolo*/
  contMismaClase = 0;

  for (m = 0; m < cols; m++){
    clase=-1;

    for (n = 0; n < conjuntoActual.contClases; n++){
      for (l = 0; l < tam; l++){
        if (destinos[0][m] == conjuntoActual.clases[n].estados[l]){
          clase = n;
          contMismaClase++;
          break;
        }
      }
      if (clase != -1){
        break;
      }
    }

    if (clase != -1){
      for (l = 0; l < tam; l++){
        if (destinos[1][m] == conjuntoActual.clases[clase].estados[l]){
          contMismaClase++;
          break;
        }
      }
    }
  }

  if (contMismaClase == cols*2){
    return 1;
  }

  return 0;
}

/*
 * Funcion que comprueba si un estado esta contenido en la clase actual
 * de un conjunto, es decir, en la mas recientemente creada.
 * @param estado indice a comprobar
 */
int presenteEnClaseActual(int estado){
  int present = 0;

  for (k = 0; k < conjuntoActual.clases[conjuntoActual.contClases].contEstados; k++){
    if (conjuntoActual.clases[conjuntoActual.contClases].estados[k] == estado){
      present = 1;
    }
  }

  return present;
}

/*
 * Funcion que comprueba si un estado esta contenido en alguna clase
 * del conjunto actual.
 * @param estado indice a comprobar
 */
int presenteEnAlgunaClase(int estado){
  int present = 0;

  for (k = 0; k < conjuntoActual.contClases; k++){
    for (l = 0; l < conjuntoActual.clases[k].contEstados; l++){

      if (conjuntoActual.clases[k].estados[l] == estado){
        present = 1;
        break;
      }
    }

    if (present == 1){
      break;
    }
  }

  return present;
}

/*
 * Funcion que comprueba si, en alguna clase del conjunto previo al actual,
 * ha quedado algun estado sin aparecer en el actual. Si ocurre, es porque el numero
 * de estados de la clase era impar y no todos los estados formaron una clase aparte,
 * sobrando uno que se queda en la misma. En este caso, el estado sobrante se añade
 * a una nueva clase en el conjunto actual.
 */
void verEstadosSobrantes(){
  int present = 0;

  for (i = 0; i < conjuntoPrevio.contClases; i++){
    for (j = 0; j < conjuntoPrevio.clases[i].contEstados; j++){
      present = 0;
      /*Cada estado de cada clase del conjunto previo debe estar en el conjunto actual*/
      for (k = 0; k < conjuntoActual.contClases; k++){
        for (m = 0; m < conjuntoActual.clases[k].contEstados; m++){
          if (conjuntoActual.clases[k].estados[m] == conjuntoPrevio.clases[i].estados[j]){
            present = 1;
            break;
          }
        }

        if (present == 1){
          break;
        }
      }

      if (present == 0){
        /*Lo insertamos en una nueva clase*/
        conjuntoActual.clases[conjuntoActual.contClases].estados[0] = conjuntoPrevio.clases[i].estados[j];
        conjuntoActual.clases[conjuntoActual.contClases].contEstados++;
        conjuntoActual.contClases++;
      }
    }
  }


}

/*
 * Funcion que genera el conjunto actual nuevo a partir de la matriz de indistinguibles.
 * Recorre la matriz por columnas y cada columna que tenga alguna celda marcada sera una nueva
 * clase, salvo que dicha celda este previamente en otra clase de este conjunto.
 */
void crearConjuntoNuevo(){
  /*Recorremos la matriz por columnas*/
  for (j = 0; j < tam-1; j++){
    for (i = 1; i < tam; i++){

      if (matriz[i][j] == INDIST){

        /*Si la pareja no estaba ya en otra clase, se añade*/
        if (presenteEnAlgunaClase(j) == 0 && presenteEnAlgunaClase(i) == 0){

          conjuntoActual.clases[conjuntoActual.contClases].estados[conjuntoActual.clases[conjuntoActual.contClases].contEstados] = i;
          conjuntoActual.clases[conjuntoActual.contClases].contEstados++;

          /*Se comprueba que no repitamos la columna en la clase*/
          if (presenteEnClaseActual(j) == 0){
            conjuntoActual.clases[conjuntoActual.contClases].estados[conjuntoActual.clases[conjuntoActual.contClases].contEstados] = j;
            conjuntoActual.clases[conjuntoActual.contClases].contEstados++;
          }
        }
      }
    }

    /*Si la clase para esta columna tiene estados, creamos nueva para la siguiente, sino reutilizamos*/
    if (conjuntoActual.clases[conjuntoActual.contClases].contEstados > 0){
      conjuntoActual.contClases++;
    }
  }

  /*Comprobamos si nos han quedado estados solitarios en el conjunto previo*/
  verEstadosSobrantes();
}

/*
 * Funcion que comprueba si dos conjuntos coinciden. Para ello, deben coincidir en numero de clases
 * y todos los estados de todas las clases de uno deben estar presentes en el otro, sin importar
 * que aparezcan en otro orden.
 * @param c1 conjunto a comparar
 * @param c2 conjunto a comparar
 */
int coincidenConjuntos(Conjunto c1, Conjunto c2){
  int coinciden = 0;
  int coincidenciaEstados = 0;
  int coincidenciaClases = 0;
  int contClasesCoincidentes = 0;
  int contEstadosCoincidentes = 0;

  /*Primero deben tener el mismo numero de clases ambos*/
  if (c1.contClases == c2.contClases){

    /*Cada clase del uno debe estar en el dos*/
    for (i = 0; i < c1.contClases; i++){
      coincidenciaClases = 0;

      /*Si las clases coinciden en numero de estados pasamos a analizar su contenido*/
      for (j = 0; j < c2.contClases && coincidenciaClases == 0; j++){
        if (c1.clases[i].contEstados == c2.clases[j].contEstados){
          contEstadosCoincidentes = 0;

          /*Para cada estado de la primera clase...*/
          for (k = 0; k < c1.clases[i].contEstados; k++){
            coincidenciaEstados = 0;

            /*...vemos si coincide con alguno de las clases del otro conjunto que tengan ese mismo numero de estados*/
            for (m = 0; m < c2.clases[j].contEstados; m++){

              if (c1.clases[i].estados[k] == c2.clases[j].estados[m]){
                coincidenciaEstados = 1;
                break;
              }
            }

            if (coincidenciaEstados == 1){
              contEstadosCoincidentes++;
            }
          }

          if (contEstadosCoincidentes == c1.clases[i].contEstados){
            contClasesCoincidentes++;
            coincidenciaClases = 1;
          }
        }
      }
    }

    if (contClasesCoincidentes == c1.contClases){
      coinciden = 1;
    }

  } else {
    coinciden = 0;
  }

  return coinciden;
}

/*
 * Funcion que obtiene los estados accesibles de un automata. Para ello,
 * para cada estado del AFD, recorre todos los estados a ver si desde alguno,
 * con algun simbolo, se transita hacia este, en cuyo caso sera accesible.
 * @param origen automata del que ver sus estados accesibles
 */
int * getEstadosAccesibles(AFND *origen){
  int *accesibles = (int *)calloc(tam, sizeof(int));
  int present = 0;

  for (i = 0; i < tam; i++){
    for (j = 0; j < tam; j++){
      for (k = 0; k < cols; k++){
        if (AFNDTransicionIndicesEstadoiSimboloEstadof(origen, i, k, j) == 1){
          present = 0;
          for (l = 0; l < contAccesibles; l++){
            if (accesibles[l] == j){
              present = 1;
            }
          }

          if (present == 0){
            accesibles[contAccesibles] = j;
            contAccesibles++;
          }
        }
      }
    }
  }

  return accesibles;
}

/*
 * Funcion que elimina estados inaccesibles de un conjunto. Para ello,
 * para cada estado de cada clase del conjunto se comprueba que este en
 * el array de estados accesibles. Excepcionalmente, puede ocurrir que no
 * este en ese conjunto pero sea accesible si se trata del estado inicial.
 * @param origen automata del que ver los tipos de los estados
 * @param conjunto del que eliminar inaccesibles
 * @param accesibles array de estados accesibles
 */
void eliminaEstadosInaccesibles(AFND *origen, Conjunto conjunto, int *accesibles){
  int present = 0;

  /*Para cada estado de cada clase vemos si es accesible*/
  for (i = 0; i < conjunto.contClases; i++){
    for (j = 0; j < conjunto.clases[i].contEstados; j++){
      present = 0;

      if (AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[j]) == INICIAL || AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[j]) == INICIAL_Y_FINAL){
        present = 1;
      } else {
        for (k = 0; k < contAccesibles; k++){
          if (conjunto.clases[i].estados[j] == accesibles[k]){
            present = 1;
            break;
          }
        }
      }


      if (present == 0){
        conjunto.clases[i].estados[j] = -1;
        conjunto.clases[i].contEstados--;
      }
    }
  }
}

/*
 * Funcion que inserta los mismos simbolos del AFD original en el minimo.
 * @param origen automata del que ver los simbolos
 */
void insertaSimbolos(AFND *origen){
  for (i = 0; i < cols; i++){
    AFNDInsertaSimbolo(minimo, AFNDSimboloEn(origen, i));
  }
}

/*
 * Funcion que devuelve el numero de estados que tendra un AFD minimo. Este
 * numero es el numero de clases que tenga el conjunto final. Sin embargo,
 * puede ocurrir que una clase este presente y no contenga estados, debido a que
 * al eliminar los inaccesibles una clase se quedo vacia. Para estos casos,
 * tenemos que comprobar que todos los estados de la clase sean distintos de -1.
 * @param conjunto conjunto final a partir del cual se cuentan las clases
 */
int numEstados(Conjunto conjunto){
  int incrementa = 0;
  int nEstados = 0;

  for (i = 0; i < conjunto.contClases; i++){
    for (j = 0; j < conjunto.clases[i].contEstados; j++){
      if (conjunto.clases[i].estados[j] != -1){
        incrementa = 1;
      }
    }

    if (incrementa == 1){
      nEstados++;
      incrementa = 0;
    }
  }

  return nEstados;
}

/*
 * Funcion que inserta los estados a un AFD minimo. Estos seran las clases del conjunto,
 * comprobando que no sean -1 los estados, por lo que se ha explicado anteriormente.
 * El nombre del estado correspondiente a una clase es la concatenacion de los nombres de
 * todos los estados que la componen, y el tipo se obtiene analizando el tipo de todos ellos.
 * @param origen automata del que ver los nombres y tipos de los estados
 * @param conjunto del que obtener los estados del AFD minimo
 */
void insertaEstados(AFND *origen, Conjunto conjunto){
  for (i = 0; i < conjunto.contClases; i++){
    char estado[LEN] = "";
    int tipo = NORMAL;
    int incrementa = 0;

    for (j = 0; j < conjunto.clases[i].contEstados; j++){
      if (conjunto.clases[i].estados[j] != -1){
        strcat(estado, AFNDNombreEstadoEn(origen, conjunto.clases[i].estados[j]));

        if (AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[j]) == INICIAL){
          tipo = INICIAL;
        }

        if (AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[j]) == INICIAL_Y_FINAL){
          tipo = INICIAL_Y_FINAL;
        }

        if (AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[j]) == FINAL){
          int fin = 0;

          for (k = 0; k < conjunto.clases[i].contEstados; k++){
            if (conjunto.clases[i].estados[k] != -1){
              if (AFNDTipoEstadoEn(origen, conjunto.clases[i].estados[k]) == INICIAL_Y_FINAL){
                tipo = INICIAL_Y_FINAL;
                fin = 1;
                break;
              }
            }
          }

          if (fin == 0){
            tipo = FINAL;
          }

        }
        incrementa = 1;
      }
    }

    if (incrementa == 1){
      strcat(estado, "\0");
      AFNDInsertaEstado(minimo, estado, tipo);
      incrementa = 0;
    }
  }
}

/*
 * Funcion que inserta las transiciones del AFD minimo a partir de sus estados.
 * Estos estados se componen de los estados de las clases del conjunto final.
 * Se analiza cada uno y se mira a donde transita en la matriz de transiciones
 * creada inicialmente.
 * @param conjunto conjunto para ver los indices de cada estado a analizar
 */
void insertaTransiciones(Conjunto conjunto){
  Estado *destino;
  int indiceClaseOri;

  for (i = 0; i < conjunto.contClases; i++){
    for (j = 0; j < cols; j++){
      /*Basta mirar transiciones del primer estado de cada clase, pues los demas son indistinguibles, es decir, el mismo*/
      destino = transicionesIniciales[conjunto.clases[i].estados[0]][j];
      indiceClaseOri = i;

      if (destino != NULL){
        for (k = 0; k < conjunto.contClases; k++){
          /*Transiciones de cada estado del minimo: ver subestados*/
          if (strstr(AFNDNombreEstadoEn(minimo, k), destino->nombre)){
            /*El indice de las clases coincide con el indice de los estados en el nuevo automata*/
            /*Buscar a que clase transita dentro del conjunto, mediante su indice, no su nombre*/
            AFNDInsertaTransicion(minimo, AFNDNombreEstadoEn(minimo, indiceClaseOri), AFNDSimboloEn(minimo, j), AFNDNombreEstadoEn(minimo, k));
          }
        }
      }
    }
  }
}

/*
 * Funcion minimiza un AFD, agrupando los estados indistinguibles
 * y eliminando los inaccesibles.
 * @param automata a minimizar
 * @return automata mínimo
 */
AFND *minimiza (AFND *origen){
  int *accesibles;
  int nEstados;
  int continuar = 1;
  int **destinos;

  destinos = (int **)calloc(2, sizeof(int *));

  for (i = 0; i < 2; i++){
    destinos[i] = (int *)calloc(AFNDNumSimbolos(origen), sizeof(int));
  }

  tam = AFNDNumEstados(origen);
  cols = AFNDNumSimbolos(origen);
  filas = tam;

  generaMatrizTransiciones(origen);

  /*Construir matriz del algoritmo*/
  generaMatrizIndist();

  /*Rellenamos Q/E0: finales y resto*/
  conjuntoActual = reservaConjunto();
  rellenaConjuntoInicial(origen);

  while(continuar == 1){
    /*Marcar matriz de indistinguibilidad*/
    /*Cada clase hay que ver si tiene más de un estado*/
    for (k = 0; k < conjuntoActual.contClases; k++){
      if (conjuntoActual.clases[k].contEstados == 1){
        matriz[conjuntoActual.clases[k].estados[0]][conjuntoActual.clases[k].estados[0]] = INDIST;

      } else {
        /*En este caso hay que hacer parejas*/


        for (i = 1; i <= conjuntoActual.clases[k].contEstados; i++){
          for (j = 0; j < i; j++){
            /*i y j son una pareja*/
            e1 = conjuntoActual.clases[k].estados[i];
            e2 = conjuntoActual.clases[k].estados[j];

            if (e2 > e1){
              e2 = conjuntoActual.clases[k].estados[i];
              e1 = conjuntoActual.clases[k].estados[j];
            }
            contMismaClase = 0;

            verDestinosPareja(destinos);

            if (isParejaIndist(destinos) == 1){
              matriz[e1][e2] = INDIST;
            }
          }
        }
      }
    }

    /*Ya tenemos la matriz marcada por el conjunto actual*/
    /*Guardamos el conjunto que acabamos de usar, ya que va a haber que comparar*/
    liberaConjunto(conjuntoPrevio);
    conjuntoPrevio = copiarConjunto(conjuntoActual);

    /*Vamos a crear el nuevo conjunto actual*/
    liberaConjunto(conjuntoActual);

    conjuntoActual = reservaConjunto();

    crearConjuntoNuevo();

    /*Vemos si hay que continuar o no*/
    if (coincidenConjuntos(conjuntoActual, conjuntoPrevio) == 1){
      continuar = 0;
    } else {
      continuar = 1;

      /*Dejamos la matriz vacía para empezar a marcar de nuevo en la siguiente vuelta*/
      liberaMatrizIndist();
      generaMatrizIndist();
    }
  }

  /*Eliminamos estados inaccesibles del conjunto*/
  accesibles = getEstadosAccesibles(origen);
  eliminaEstadosInaccesibles(origen, conjuntoActual, accesibles);

  /*Añadimos nombres de estados al automata, concatenando los nombres de los de cada clase*/
  /*Comprobar siempre la longitud de contEstados ahora que hemos eliminado inaccesibles*/
  /*Comprobar valores -1 en array de estados ahora que hemos eliminado inaccesibles*/


  /*Contamos estados*/
  nEstados = numEstados(conjuntoActual);


  minimo = AFNDNuevo("af11", nEstados, AFNDNumSimbolos(origen));

  insertaSimbolos(origen);

  insertaEstados(origen, conjuntoActual);

  insertaTransiciones(conjuntoActual);

  /*Liberar recursos*/
  liberaMatrizIndist();
  liberaConjunto(conjuntoActual);
  liberaConjunto(conjuntoPrevio);
  liberaMatrizTransiciones();
  free(accesibles);

  for (i = 0; i < 2; i++){
    free(destinos[i]);
  }

  free(destinos);

  return minimo;
}
