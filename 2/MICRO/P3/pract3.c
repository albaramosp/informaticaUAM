/*********************************************************************
 * pract3.c
 *
 * Sistemas Basados en Microprocesador
 * 2018-2019
 * Practica 3
 * Codigos de Barras
 *
 *********************************************************************/
 
#include <stdio.h>
#include <stdlib.h>


/***** Declaracion de funciones *****/

/* Ejercicio 1 */

unsigned char computeControlDigit(char* barCodeASCII);
void createBarCode(int countryCode, unsigned int companyCode, unsigned long productCode, unsigned char controlDigit, unsigned char* out_barCodeASCII);
void decodeBarCode(unsigned char* in_barCodeASCII, unsigned int* countryCode, unsigned int* companyCode, unsigned long* productCode, unsigned char* controlDigit);



//////////////////////////////////////////////////////////////////////////
///// -------------------------- MAIN ------------------------------ /////
//////////////////////////////////////////////////////////////////////////
int main( void ){
	char barCodeStr[14],barCodeStrCorregido[14];
	unsigned char barCodeDigits[13];
	unsigned int  countryCode, companyCode;
	unsigned long productCode;
	unsigned char controlDigitCheck, controlDigit;

	printf("Introduzca nuevo codigo de barras de 13 digitos: ");
	scanf("%s", &barCodeStr);
	

		
	decodeBarCode(barCodeStr, &countryCode, &companyCode, &productCode, &controlDigit);
	printf("Codigo de barras leido:\n");
	printf("- Codigo de Pais - %u -\n",countryCode);
	printf("- Codigo de Empresa - %u -\n",companyCode);
	printf("- Codigo de Producto - %lu -\n",productCode);
	printf("- Codigo de Control - %u -\n",controlDigit);

	controlDigitCheck = computeControlDigit(barCodeStr);
	
	if(controlDigit != controlDigitCheck){
		printf("Error en codigo de control. Leido %u vs Calculado %u\n", controlDigit, controlDigitCheck);
		printf("Corrigiendo codigo de barras...\n");
		createBarCode(countryCode,companyCode,productCode,controlDigitCheck,barCodeStrCorregido);
		printf("Codigo de barras corregido es: %s\n",barCodeStrCorregido);
	}
	else{
		printf("Codigo de control %u es correcto para el codigo de barras %s\n", controlDigit, barCodeStr);
	}
	
	
	return 0;
}