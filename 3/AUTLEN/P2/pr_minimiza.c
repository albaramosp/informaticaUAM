/*
 * pr_minimiza.c
 * Programa que prueba el funcionamiento de la libreria de minimización de automatas.
 * Proporciona 3 pruebas que pueden comentarse para probar cada una.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#include <stdlib.h>
#include <string.h>

#include "minimiza.h"

int main(int argc, char ** argv){
	/*
	AFND * original, *minimo;

	original= AFNDNuevo("af11", 2, 2);

	AFNDInsertaSimbolo(original, "0");
	AFNDInsertaSimbolo(original, "1");

	AFNDInsertaEstado(original, "DC",INICIAL_Y_FINAL);
	AFNDInsertaEstado(original, "E", NORMAL);


	AFNDInsertaTransicion(original, "DC", "0", "DC");
	AFNDInsertaTransicion(original, "DC", "1", "E");

	AFNDInsertaTransicion(original, "E", "0", "DC");
	AFNDInsertaTransicion(original, "E", "1", "E");

	AFNDCierraLTransicion(original);


	minimo = minimiza(original);

	AFNDImprime(stdout,minimo);
	AFNDADot(minimo);

	AFNDElimina(original);
	AFNDElimina(minimo);
	*/
	/*
	AFND * original, *minimo;

	original= AFNDNuevo("af11", 5, 3);

	AFNDInsertaSimbolo(original, "a");
	AFNDInsertaSimbolo(original, "b");
	AFNDInsertaSimbolo(original,"c");

	AFNDInsertaEstado(original, "A",INICIAL_Y_FINAL);
	AFNDInsertaEstado(original, "B",FINAL);
	AFNDInsertaEstado(original, "C",FINAL);
	AFNDInsertaEstado(original, "D",FINAL);
	AFNDInsertaEstado(original, "E", NORMAL);

	AFNDInsertaTransicion(original, "A", "a", "B");
	AFNDInsertaTransicion(original, "A", "c", "B");
	AFNDInsertaTransicion(original, "A", "b", "C");

	AFNDInsertaTransicion(original, "B", "a", "B");
	AFNDInsertaTransicion(original, "B", "c", "B");
	AFNDInsertaTransicion(original, "B", "b", "C");

	AFNDInsertaTransicion(original, "C", "a", "B");
	AFNDInsertaTransicion(original, "C", "c", "B");
	AFNDInsertaTransicion(original, "C", "b", "D");

	AFNDInsertaTransicion(original, "D", "a", "E");
	AFNDInsertaTransicion(original, "D", "c", "E");
	AFNDInsertaTransicion(original, "D", "b", "E");

	AFNDInsertaTransicion(original, "E", "a", "E");
	AFNDInsertaTransicion(original, "E", "c", "E");
	AFNDInsertaTransicion(original, "E", "b", "E");

	AFNDCierraLTransicion(original);


	minimo = minimiza(original);

	AFNDImprime(stdout,minimo);
	AFNDADot(minimo);

	AFNDElimina(original);
	AFNDElimina(minimo);
*/
	/*
	AFND * original, *minimo;

	original= AFNDNuevo("af12", 3, 2);

	AFNDInsertaSimbolo(original, "0");
	AFNDInsertaSimbolo(original,"1");

	AFNDInsertaEstado(original, "C",INICIAL_Y_FINAL);
	AFNDInsertaEstado(original, "D", INICIAL);
	AFNDInsertaEstado(original, "E", NORMAL);

	AFNDInsertaTransicion(original, "C", "0", "D");
	AFNDInsertaTransicion(original, "C", "1", "E");
	AFNDInsertaTransicion(original, "D", "0", "D");
	AFNDInsertaTransicion(original, "D", "1", "E");
	AFNDInsertaTransicion(original, "E", "0", "C");
	AFNDInsertaTransicion(original, "E", "1", "E");
	AFNDCierraLTransicion(original);


	minimo = minimiza(original);

	AFNDImprime(stdout,minimo);
	AFNDADot(minimo);

	AFNDElimina(original);
	AFNDElimina(minimo);
	*/

	/*
	AFND * original, *minimo;

	original= AFNDNuevo("af12", 8, 2);

	AFNDInsertaSimbolo(original, "0");
	AFNDInsertaSimbolo(original,"1");

	AFNDInsertaEstado(original, "A",INICIAL);
	AFNDInsertaEstado(original, "B", NORMAL);
	AFNDInsertaEstado(original, "C", FINAL);
	AFNDInsertaEstado(original, "D", NORMAL);
	AFNDInsertaEstado(original, "E", NORMAL);
	AFNDInsertaEstado(original, "F", NORMAL);
	AFNDInsertaEstado(original, "G", NORMAL);
	AFNDInsertaEstado(original, "H", NORMAL);

	AFNDInsertaTransicion(original, "A", "0", "B");
	AFNDInsertaTransicion(original, "A", "1", "F");
	AFNDInsertaTransicion(original, "B", "0", "G");
	AFNDInsertaTransicion(original, "B", "1", "C");
	AFNDInsertaTransicion(original, "C", "0", "A");
	AFNDInsertaTransicion(original, "C", "1", "C");
	AFNDInsertaTransicion(original, "D", "0", "C");
	AFNDInsertaTransicion(original, "D", "1", "G");
	AFNDInsertaTransicion(original, "E", "0", "H");
	AFNDInsertaTransicion(original, "E", "1", "F");
	AFNDInsertaTransicion(original, "F", "0", "C");
	AFNDInsertaTransicion(original, "F", "1", "G");
	AFNDInsertaTransicion(original, "G", "0", "G");
	AFNDInsertaTransicion(original, "G", "1", "E");
	AFNDInsertaTransicion(original, "H", "0", "G");
	AFNDInsertaTransicion(original, "H", "1", "C");
	AFNDCierraLTransicion(original);


	minimo = minimiza(original);

	AFNDImprime(stdout,minimo);
	AFNDADot(minimo);

	AFNDElimina(original);
	AFNDElimina(minimo);
	*/



	AFND * p_afnd;
	AFND * p_afnd_min;
	p_afnd = AFNDNuevo("af1",12,2);
	AFNDInsertaSimbolo(p_afnd,"0");
	AFNDInsertaSimbolo(p_afnd,"1");
	AFNDInsertaEstado(p_afnd,"q0",INICIAL_Y_FINAL);
	AFNDInsertaEstado(p_afnd,"q1",NORMAL);
	AFNDInsertaEstado(p_afnd,"q2",NORMAL);
	AFNDInsertaEstado(p_afnd,"q3",NORMAL);
	AFNDInsertaEstado(p_afnd,"q4",FINAL);
	AFNDInsertaEstado(p_afnd,"q5",NORMAL);
	AFNDInsertaEstado(p_afnd,"q6",NORMAL);
	AFNDInsertaEstado(p_afnd,"q7",NORMAL);
	AFNDInsertaEstado(p_afnd,"q8",FINAL);
	AFNDInsertaEstado(p_afnd,"q9",NORMAL);
	AFNDInsertaEstado(p_afnd,"q10",NORMAL);
	AFNDInsertaEstado(p_afnd,"q11",NORMAL);
	AFNDInsertaTransicion(p_afnd, "q0", "0", "q1");
	AFNDInsertaTransicion(p_afnd, "q0", "1", "q1");
	AFNDInsertaTransicion(p_afnd, "q1", "0", "q2");
	AFNDInsertaTransicion(p_afnd, "q1", "1", "q2");
	AFNDInsertaTransicion(p_afnd, "q2", "0", "q3");
	AFNDInsertaTransicion(p_afnd, "q2", "1", "q3");
	AFNDInsertaTransicion(p_afnd, "q3", "0", "q4");
	AFNDInsertaTransicion(p_afnd, "q3", "1", "q4");
	AFNDInsertaTransicion(p_afnd, "q4", "0", "q5");
	AFNDInsertaTransicion(p_afnd, "q4", "1", "q5");
	AFNDInsertaTransicion(p_afnd, "q5", "0", "q6");
	AFNDInsertaTransicion(p_afnd, "q5", "1", "q6");
	AFNDInsertaTransicion(p_afnd, "q6", "0", "q7");
	AFNDInsertaTransicion(p_afnd, "q6", "1", "q7");
	AFNDInsertaTransicion(p_afnd, "q7", "0", "q8");
	AFNDInsertaTransicion(p_afnd, "q7", "1", "q8");
	AFNDInsertaTransicion(p_afnd, "q8", "0", "q9");
	AFNDInsertaTransicion(p_afnd, "q8", "1", "q9");
	AFNDInsertaTransicion(p_afnd, "q9", "0", "q10");
	AFNDInsertaTransicion(p_afnd, "q9", "1", "q10");
	AFNDInsertaTransicion(p_afnd, "q10", "0", "q11");
	AFNDInsertaTransicion(p_afnd, "q10", "1", "q11");
	AFNDInsertaTransicion(p_afnd, "q11", "0", "q0");
	AFNDInsertaTransicion(p_afnd, "q11", "1", "q0");
	p_afnd_min = minimiza(p_afnd);
	AFNDImprime(stdout,p_afnd_min);
	AFNDADot(p_afnd_min);
	AFNDElimina(p_afnd);
	AFNDElimina(p_afnd_min);


	return 0;
}
