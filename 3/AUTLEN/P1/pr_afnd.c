/*
 * pr_afnd.c
 * Programa que prueba el funcionamiento de la libreria de transformacion para dos automatas diferentes.
 * Por defecto se prueba un caso, pero aparece comentado otro caso que se puede descomentar y probar.
 * @author Alba Ramos Pedroviejo
 * P24
 */

#include <stdlib.h>
#include <string.h>

#include "afnd.h"
#include "transforma.h"

int main(int argc, char ** argv){
	AFND * p_afnd, *p_afd;

	/*Codigo para obtener el automata de la diapositiva 40*/

	/*
	p_afnd= AFNDNuevo("af11", 3, 2);

	AFNDInsertaSimbolo(p_afnd,"0");
	AFNDInsertaSimbolo(p_afnd, "1");

	AFNDInsertaEstado(p_afnd, "q0",INICIAL);
	AFNDInsertaEstado(p_afnd, "q1", NORMAL);
	AFNDInsertaEstado(p_afnd, "q2", FINAL);

	AFNDInsertaTransicion(p_afnd, "q0", "0", "q1");
	AFNDInsertaTransicion(p_afnd, "q1", "1", "q2");
	AFNDInsertaTransicion(p_afnd, "q0", "0", "q0");
	AFNDInsertaTransicion(p_afnd, "q0", "1", "q0");
	AFNDCierraLTransicion(p_afnd);
	*/

		/*Codigo para obtener el automata del enunciado*/

	p_afnd= AFNDNuevo("af12", 6, 3);

	AFNDInsertaSimbolo(p_afnd,"+");
	AFNDInsertaSimbolo(p_afnd, "0");
	AFNDInsertaSimbolo(p_afnd,".");

	AFNDInsertaEstado(p_afnd, "q0",INICIAL);
	AFNDInsertaEstado(p_afnd, "q1", NORMAL);
	AFNDInsertaEstado(p_afnd, "q2", NORMAL);
	AFNDInsertaEstado(p_afnd, "q3", NORMAL);
	AFNDInsertaEstado(p_afnd, "q4", NORMAL);
	AFNDInsertaEstado(p_afnd, "q5", FINAL);

	AFNDInsertaTransicion(p_afnd, "q0", "+", "q1");
	AFNDInsertaTransicion(p_afnd, "q1", "0", "q1");
	AFNDInsertaTransicion(p_afnd, "q1", "0", "q4");
	AFNDInsertaTransicion(p_afnd, "q1", ".", "q2");
	AFNDInsertaTransicion(p_afnd, "q2", "0", "q3");
	AFNDInsertaTransicion(p_afnd, "q3", "0", "q3");
	AFNDInsertaTransicion(p_afnd, "q4", ".", "q3");
	AFNDInsertaLTransicion(p_afnd, "q0", "q1");
	AFNDInsertaLTransicion(p_afnd, "q3", "q5");
	AFNDCierraLTransicion(p_afnd);


	p_afd = transforma(p_afnd);

	AFNDImprime(stdout,p_afd);
	AFNDADot(p_afd);

	AFNDElimina(p_afnd);
	AFNDElimina(p_afd);


	return 0;
}
