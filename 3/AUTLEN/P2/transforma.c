/*
 * transforma.c
 * Programa que imlementa las funciones de la libreria transforma.h para realizar la equivalencia
 * entre un AFND y un AFD.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transforma.h"


Estado ***matrizAFND;
AFND * p_afd = NULL; /*AFD equivalente*/
Celda **matrizAFD = NULL; /*Matriz con las transiciones del AFD*/
int contFilas = 1; /*Numero de filas que se van agregando a la matriz*/

/*De los nuevos estados, aquellos que se han ido analizando con el algoritmo de transformacion*/
Estado *analizados;
int contAnalizados = 0;


/*Nuevos estados que tendra el AFD*/
EstadoCompuesto *estadosAFD;
int contEstados = 0;

/*Estado desde el que se analizan transiciones en el AFD*/
EstadoCompuesto partida;

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
Estado* conexiones(AFND *a, int pos_estado, char *simbolo, int lambda){
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
 * Funcion que obtiene la matriz de transiciones de un AFND que se utilizara en el algoritmo de transformacion
 * @param p_afnd puntero al automata del que obtener las transiciones
 * @param filas numero de estados del AFND, es decir, filas de la matriz
 * @param cols numero de simbolos del AFND, es decir, columnas de la matriz sin incluir lambda
 */
void crearMatrizAFND(AFND *p_afnd, int filas, int cols){
  int i, j;
  Estado *e;

  matrizAFND = (Estado ***)calloc(filas, sizeof(Estado **));

  for (i = 0; i < filas; i++){
    matrizAFND[i] = (Estado **)calloc(cols, sizeof(Estado *));
  }

	for (i = 0; i < filas; i++){
		for (j = 0; j < cols-1; j++){
			e = conexiones(p_afnd, i, AFNDSimboloEn(p_afnd, j), 0);
      matrizAFND[i][j] = e;
		}

    e = conexiones(p_afnd, i, AFNDSimboloEn(p_afnd, j), 1);
    matrizAFND[i][j] = e;
	}
}

/*
 * Funcion que obtiene las transiciones lambda del estado inicial del AFND
 * @param p_afnd puntero al automata del que obtener las transiciones
 * @param filas numero de estados del AFND, es decir, filas de la matriz de transiciones
 * @param cols numero de simbolos del AFND, es decir, columnas de la matriz de transiciones
 */
void buscarLambdasIniciales(AFND *p_afnd, int filas, int cols){
  int k;

  if (matrizAFND[contFilas-1][cols-1] != NULL){
    for (k = 0; k < filas; k++){
    	if (strlen(matrizAFND[0][cols-1][k].nombre) > 0){
					strcat(partida.nombre, matrizAFND[contFilas-1][cols-1][k].nombre);
					partida.indices[partida.contInd] = matrizAFND[contFilas-1][cols-1][k].indice;
					partida.contInd++;
          strcat(analizados[contAnalizados].nombre, partida.nombre);

					if (AFNDTipoEstadoEn(p_afnd, matrizAFND[contFilas-1][cols-1][k].indice) == FINAL){
						partida.tipo = INICIAL_Y_FINAL;
					}

		      estadosAFD[contEstados] = partida;
			}
		}
	}
}

/*
 * Funcion que obtiene las transiciones desde un estado del AFD y comprueba el tipo de estado
 * @param p_afnd puntero al automata AFND del que obtener un estado
 * @param filas numero de estados del AFND, es decir, filas de su matriz de transiciones
 * @param fila fila de la matriz del AFD que estamos analizando
 * @param j columna de la matriz del AFD que estamos analizando
 * @param partidaAux estado compuesto donde ir guardando los destinos de las transiciones desde la celda
 */
void buscaTransicionesDesdeCelda(AFND *p_afnd, int fila, int j, int filas, EstadoCompuesto *partidaAux){
  int k;

  for (k = 0; k < filas; k++){
    if (strlen(matrizAFND[fila][j][k].nombre) > 0){
      strcat(partidaAux->nombre, matrizAFND[fila][j][k].nombre);
      /*Los nuevos indices de partida se van guardando tambien para el nuevo estado compuesto*/
      partidaAux->indices[partidaAux->contInd]= matrizAFND[fila][j][k].indice;
      partidaAux->contInd++;

      if (AFNDTipoEstadoEn(p_afnd, matrizAFND[fila][j][k].indice) == FINAL){
        printf("\nEs final");
        partidaAux->tipo = FINAL;
      }
    }
  }
}

/*
 * Funcion que obtiene las transiciones lambda desde un estado del AFD y comprueba el tipo de estado. Lo hace comprobando que no se haya
 * transitado ya en la misma partida.
 * @param p_afnd puntero al automata AFND del que obtener un estado
 * @param filas numero de estados del AFND, es decir, filas de su matriz de transiciones
 * @param cols numero de simbolos del AFND, es decir, columnas de la matriz de transiciones
 * @param fila fila de la matriz del AFD que estamos analizando
 * @param c columna de la matriz del AFD que estamos analizando
 * @param k estado que estamos analizando del estado compuesto
 * @param partidaAux estado compuesto donde ir guardando los destinos de las transiciones desde la celda
 */
void buscaTransicionesLambdaSinRepeticion(AFND *p_afnd, int fila, int c, int k, int filas, int cols, EstadoCompuesto *partidaAux){
  int x, y;

  for (x = 0; x < filas; x++){
    if (strlen(matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1][x].nombre) > 0){
      int flag = 0;

      /*Evitamos repetir indices si se transita con lambda al mismo sitio desde varios estados de la misma partida*/
      for (y = 0; y < partidaAux->contInd; y++){
        if (partidaAux->indices[y] == matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1][x].indice){
          flag = 1;
        }
      }

      if (flag == 0){
        strcat(partidaAux->nombre, matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1][x].nombre);
        partidaAux->indices[partida.contInd] = matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1][x].indice;
        partidaAux->contInd++;

        if (AFNDTipoEstadoEn(p_afnd, matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1][x].indice) == FINAL){
          partidaAux->tipo = FINAL;printf("\nEs final");
        }
      }

    }
  }
}

/*
 * Funcion que obtiene las transiciones desde un estado del AFD y comprueba el tipo de estado. Lo hace comprobando que no se haya
 * transitado ya en la misma partida.
 * @param p_afnd puntero al automata AFND del que obtener un estado
 * @param filas numero de estados del AFND, es decir, filas de su matriz de transiciones
 * @param cols numero de simbolos del AFND, es decir, columnas de la matriz de transiciones
 * @param fila fila de la matriz del AFD que estamos analizando
 * @param c columna de la matriz del AFD que estamos analizando
 * @param k estado que estamos analizando del estado compuesto
 * @param partidaAux estado compuesto donde ir guardando los destinos de las transiciones desde la celda
 */
void buscaTransicionesSinRepeticion(AFND *p_afnd, int fila, int c, int filas, int cols, EstadoCompuesto *partidaAux){
  int k, y;

  for (k = 0; k < filas; k++){
    if (strlen(matrizAFND[fila][c][k].nombre) > 0){
      int flag2 = 0;

      /*Evitamos repetir indices si se transita al mismo sitio desde varios estados de la misma partida*/
      for (y = 0; y < partidaAux->contInd; y++){
        if (partidaAux->indices[y] == matrizAFND[fila][c][k].indice){
          flag2 = 1;
        }
      }

      if (flag2 == 0){
        strcat(partidaAux->nombre, matrizAFND[fila][c][k].nombre);
        partidaAux->indices[partidaAux->contInd]= matrizAFND[fila][c][k].indice;
        partidaAux->contInd++;

        if (AFNDTipoEstadoEn(p_afnd, matrizAFND[fila][c][k].indice) == FINAL){
          partidaAux->tipo = FINAL;printf("\nEs final");
        }
      }

      /*Se buscan transiciones lambda*/
      if (matrizAFND[(matrizAFND[fila][c][k].indice)][cols-1] != NULL){
        buscaTransicionesLambdaSinRepeticion(p_afnd, fila, c, k, filas, cols, partidaAux);
      }
    }
  }
}

/*
 * Funcion que obtiene un automata finito determinista equivalente al no determinista que recibe
 * @param a puntero al automata del que obtener el equivalente
 * @return automata determinista equivalente
 */
AFND *transforma (AFND *p_afnd){
  int i, j, k, l;
  int a, b, c;
  int flag;
  char nombre[20] = "";

  Celda celda; /*Dados un estado y un simbolo del alfabeto, guarda el estado destino del AFD y si está visitado*/

  int inicial = AFNDIndiceEstadoInicial(p_afnd);

  /*Indicador de presencia de un estado en el AFD*/
  int present = 0;

  int filas = AFNDNumEstados(p_afnd);
  int cols = AFNDNumSimbolos(p_afnd)+1; /*Incluye columna de transiciones lambda*/

  /*Construcción de matriz con transiciones del AFND*/
  crearMatrizAFND(p_afnd, filas, cols);

	/*Inicialmente, la matriz del AFD equivalente tiene solo una fila, pues no sabemos cuantos estados va a tener*/
	matrizAFD = (Celda **)calloc(contFilas, sizeof(Celda*));
	matrizAFD[contFilas-1] = (Celda *)calloc(cols, sizeof(Celda));

  /*El estado inicial del AFD sera el mismo que el del AFND junto a las transiciones lambda, si las hubiera, desde dicho estado*/
  estadosAFD = (EstadoCompuesto *)calloc(contEstados+1, sizeof(EstadoCompuesto));
  analizados = (Estado *)calloc(contAnalizados+1, sizeof(Estado));
  strcpy(analizados[contAnalizados].nombre, AFNDNombreEstadoEn(p_afnd, inicial));
  strcpy(estadosAFD[contEstados].nombre, AFNDNombreEstadoEn(p_afnd, inicial));
	estadosAFD[contEstados].tipo = INICIAL;

  partida.contInd = 0;
  partida.tipo = INICIAL;

  strcpy(partida.nombre, estadosAFD[contEstados].nombre);
	partida.indices[partida.contInd] = inicial;
	partida.contInd++;

  /*Busca transiciones lambda*/
  buscarLambdasIniciales(p_afnd, filas, cols);

  /*Se redimensiona el conjunto de estados del automata cada vez que se añade un estado*/
  contEstados++;
  estadosAFD = (EstadoCompuesto *)realloc(estadosAFD, (contEstados+1)*sizeof(EstadoCompuesto));

  contAnalizados++;
  analizados = (Estado *)realloc(analizados, (contAnalizados+1)*sizeof(Estado));


	/*Se comienza a construir la primera fila de la matriz del AFD*/
	/*Para cada simbolo de entrada...*/
  for (j = 0; j < cols-1; j++){
		EstadoCompuesto partidaAux; /*Almacena un nuevo estado compuesto al que transita dado ese simbolo*/
    strcpy(partidaAux.nombre, "");
	  strcpy(nombre, "");
		partidaAux.contInd = 0;
		partidaAux.tipo = NORMAL;

    /*...se comprueba si hay transicion desde el estado de partida*/
    for (i = 0; i < partida.contInd; i++){
			int fila = partida.indices[i];

      /*Si la celda de la matriz del AFND no es NULL, indica que existe una transicion*/
			if (matrizAFND[fila][j] != NULL){
        /* Recorremos el conjunto de estados a los que se puede transitar.
         * Como mucho, su longitud sera igual al total de estados del AFD (igual al numero de filas).
         */
        buscaTransicionesDesdeCelda(p_afnd, fila, j, filas, &partidaAux);
			}
		}

		/*Tenemos un nuevo estado que añadir al automata y a la matriz*/
		/*Lo metemos en la matriz sin problemas*/
		celda.destino = partidaAux;
		celda.visitado = 0;
		matrizAFD[contFilas-1][j] = celda;

		/*Añadimos estado al AFD solo si no estaba ya*/
		for (l = 0; l < contEstados; l++){
			if (strcmp(celda.destino.nombre, estadosAFD[l].nombre) == 0){
				present = 1;
				break;
			}
		}

		if (present == 0){
			estadosAFD[contEstados] = partidaAux;
			contEstados++;
      estadosAFD = realloc(estadosAFD, (contEstados+1)*sizeof(EstadoCompuesto));
		}

		present = 0;
	}

	/*Marcamos las celdas que hayan sido visitadas*/
	for (i = 0; i < contFilas; i++){
		for (j = 0; j < cols-1; j++){
			if (strcmp(matrizAFD[i][j].destino.nombre, partida.nombre) == 0){
				matrizAFD[i][j].visitado = 1;
			}
		}
	}

	/* Con la primera fila hecha ya podemos recorrer iterativamente.
   * El algoritmo finaliza cuando todas las celdas de la matriz hayan sido visitadas.
   */
  do {
		flag = 0;
		for (i = 0; i < contFilas; i++){
			for (j = 0; j < cols-1; j++){
				if (matrizAFD[i][j].visitado == 0){
					flag = 1;
					contFilas++;
					matrizAFD = realloc(matrizAFD, contFilas*sizeof(Celda *));
					matrizAFD[contFilas-1] = (Celda *)malloc(cols*sizeof(Celda));
					partida = matrizAFD[i][j].destino; /*Analizaremos transiciones desde el estado de esa celda*/
          strcpy(analizados[contAnalizados].nombre, partida.nombre);
          contAnalizados++;
          analizados = (Estado *)realloc(analizados, (contAnalizados+1)*sizeof(Estado));

					/*Para cada simbolo de entrada...*/
				  for (c = 0; c < cols-1; c++){
						EstadoCompuesto partidaAux;
				    strcpy(partidaAux.nombre, "");
					  strcpy(nombre, "");
						partidaAux.contInd = 0;
						partidaAux.tipo = NORMAL;

				    /*...vemos si hay transicion desde alguno de los estados que componen el estado de partida*/
				    for (a = 0; a < partida.contInd; a++){
							int fila = partida.indices[a];

							if (matrizAFND[fila][c] != NULL){
				        buscaTransicionesSinRepeticion(p_afnd, fila, c, filas, cols, &partidaAux);
							}
						}

            /*Si no ha habido transiciones dado un simbolo, la celda se marca con -*/
						if (strlen(partidaAux.nombre) < 1){
							strcpy(partidaAux.nombre, "-");
						}

						/*Tenemos un nuevo estado que añadir al automata y a la matriz*/
						celda.destino = partidaAux;

            /*Las celdas sin transiciones se marcan como visitadas para que el algoritmo las ignore*/
						if (strcmp(celda.destino.nombre, "-") == 0){
							celda.visitado = 1;
						} else {
							celda.visitado = 0;
						}

						matrizAFD[contFilas-1][c] = celda;

						/*Añadimos estado al AFD solo si no estaba ya*/
						for (l = 0; l < contEstados; l++){
							if (strcmp(celda.destino.nombre, estadosAFD[l].nombre) == 0 || strcmp(celda.destino.nombre, "-") == 0){
								present = 1;
								break;
							}
						}

						if (present == 0){
							estadosAFD[contEstados] = partidaAux;
							contEstados++;
              estadosAFD = realloc(estadosAFD, (contEstados+1)*sizeof(EstadoCompuesto));
						}

						present = 0;

            /*Marcamos las celdas que hayan sido visitadas, es decir, coincidan con alguno de los estados ya analizados*/
            for (a = 0; a < contFilas; a++){
          		for (b = 0; b < c+1; b++){
          			for (k = 0; k < contAnalizados; k++){
          				if (strcmp(matrizAFD[a][b].destino.nombre, analizados[k].nombre) == 0){
          					matrizAFD[a][b].visitado = 1;
          				}
          			}
          		}
          	}
					}
				}
			}
		}
	}	while (flag != 0);

	/*Con el array de estados del AFD ya completo, rellenamos la estructura*/
	p_afd= AFNDNuevo("af11", contEstados, cols-1);

  for (i = 0; i < cols-1; i++){
    AFNDInsertaSimbolo(p_afd, AFNDSimboloEn(p_afnd, i));
  }

	for (i = 0; i < contEstados; i++){
		AFNDInsertaEstado(p_afd, estadosAFD[i].nombre, estadosAFD[i].tipo);
	}

  for (i = 0; i < contFilas; i++){
  	for (j = 0; j < cols-1; j++){
  		if (strcmp(matrizAFD[i][j].destino.nombre, "-") != 0){
  				AFNDInsertaTransicion(p_afd, estadosAFD[i].nombre, AFNDSimboloEn(p_afnd, j), matrizAFD[i][j].destino.nombre);
  		}
  	}
  }

  free(estadosAFD);
  free(analizados);

  for (i = 0; i < filas; i++){
    for (j = 0; j < cols; j++){
      free(matrizAFND[i][j]);
    }

    free(matrizAFND[i]);
  }

  for (i = 0; i < contFilas; i++){
    free(matrizAFD[i]);
  }

  free(matrizAFND);
  free(matrizAFD);

  return p_afd;
}
