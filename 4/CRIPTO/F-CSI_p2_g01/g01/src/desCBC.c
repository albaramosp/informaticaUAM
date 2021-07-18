/**
 * @brief Programa que 
 * El programa se llama con la siguiente estructura:
 * /.desCBC {-C | -D -k clave -iv vectorincializacion} [-i f ilein] [-o f ileout]
 * -C el programa cifra
 * -D el programa descifra
 * -k clave de 64 bits: 56 bits + 8 bits de paridad
 * -iv vector de incialización
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file desCBC.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 29-10-2020
 * @copyright GNU Public License
 */

#include <time.h>
#include "funcionesDES.h"


int main(int argc, char *argv[]){
	FILE *input = NULL, *output = NULL; 
	uint64_t mensaje=0, clave=0, iv=0, aux=0; /* Asi ya viene con padding incluido, porque ocupa 64b si o si */
	int textlen=0, cifrar=0, cont=0, flag=0;
	char buf[16], *mensajeTotal;
    
	if (argc < 4 || argc > 10){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("/.desCBC {-C | -D -k clave -iv vectorincializacion} [-i f ilein] [-o f ileout]\n");
		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");
		printf("-k clave de 64 bits: 56 bits + 8 bits de paridad\n");
		printf("-iv vector de incialización\n");
		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
			cifrar = 1;

			if (strcmp(argv[2], "-k") == 0){
				/*metemos la clave en hexadecimal*/
				sscanf(argv[3],"%lX", &clave);

				/* Comprobar la paridad de la clave */
				if (findParity(clave) == 0){
					printf("\nERROR: la clave no tiene paridad, debería tenerla\n");
					exit(1);
				}
			} else {
				srand(time(NULL));

				while(flag == 0){
					clave = rand();
					clave <<= 32;
					clave |= rand();

					if (findParity(clave) == 1){
						flag = 1;
					}
				}

				printf("\nClave generada: %lx\n", clave);
			}
		} else {
			cifrar = 0;

			/*metemos la clave en hexadecimal*/
			sscanf(argv[3],"%lX", &clave);

			/* Comprobar la paridad de la clave */
			if (findParity(clave) == 0){
				printf("\nERROR: la clave no tiene paridad, debería tenerla\n");
				exit(1);
			}
		}
		
		/*metemos IV en hexadecimal*/
		if (strcmp(argv[2], "-k") == 0){
			sscanf(argv[5],"%lX", (uint64_t*)&iv);
		} else {
			sscanf(argv[3],"%lX", (uint64_t*)&iv);
		}

		if((argc == 8 && strcmp(argv[6], "-o") == 0) || argc == 10 || (argc == 6 && strcmp(argv[4], "-o") == 0)){
			if((argc == 8 && strcmp(argv[6], "-o") == 0))
				output = fopen(argv[7], "wb");
			if(argc == 10)
				output = fopen(argv[9], "wb");
			if((argc == 6 && strcmp(argv[4], "-o") == 0))
				output = fopen(argv[5], "wb");
		} 

		if((argc > 6 && strcmp("-i", argv[6]) == 0) || (argc > 4 && strcmp("-i", argv[4]) == 0)){
			if(argc > 6 && strcmp("-i", argv[6]) == 0)
				input = fopen(argv[7], "rb");
			
			if(argc > 4 && strcmp("-i", argv[4]) == 0)
				input = fopen(argv[5], "rb");
				
			if (input==NULL){
				printf("Error en apertura de fichero");
				exit(1);					
			}

			fseek(input, 0, SEEK_END);
            textlen = ftell(input);
            rewind(input);

			mensajeTotal = malloc(textlen * sizeof(char));
            fread(mensajeTotal, 1, textlen, input);
		}

		/*si no hay fichero se usa la entrada estandar*/
		if (input == NULL){
			input = stdin;
			printf("\nIr introduciendo caracteres en bloques de 16 caracteres y pulsar CTRL+C para finalizar: ");
			while(fread(&buf, 16*sizeof(char), 1, input) == 1){
				sscanf(buf,"%lx", (uint64_t*)&mensaje);

				if (cifrar == 1){
					mensaje ^= iv;
					iv = des(mensaje, clave, cifrar);
					printf("\nResultado: %016lx", iv);
				} else {
					aux = des(mensaje, clave, cifrar);
					aux ^= iv;
					iv = mensaje;
					printf("\nResultado: %016lx", aux);
				}
					
				printf("\nIr introduciendo caracteres en bloques de 16 caracteres y pulsar CTRL+C para finalizar: ");
			}
		} else {
			if(output == NULL)
				printf("\nResultado: ");
			while(cont < textlen){
				strncpy(buf, mensajeTotal+cont, 16);
				cont += 16;
				
				if (cont > textlen){
					strncpy(buf, mensajeTotal+(cont-16), (cont - textlen)); 
					printf("\nEste bloque tiene %d elementos", (cont - textlen));
					cont += 16;
					sscanf(buf,"%lx", (uint64_t*)&mensaje);
					printf("\nTienes: %lx", mensaje);
					mensaje <<= (64-(cont - textlen)); /* Padding por detras */
					printf("\nDesplazaste: %lx", mensaje);
				} else 
					sscanf(buf,"%lx", (uint64_t*)&mensaje);
				
				printf("\nHemos leido: %lx\n", mensaje);

				if (cifrar == 1){
					mensaje ^= iv;
					iv = des(mensaje, clave, cifrar);
					
					if(output == NULL)
						printf("%016lx", iv);
					else
						fprintf(output, "%016lx", iv);
				} else {
					aux = des(mensaje, clave, cifrar);
					aux ^= iv;
					iv = mensaje;

					if(output == NULL)
						printf("%016lx", aux);
					else
						fprintf(output, "%016lx", aux);
				}
			}

			printf("\n");
		}
		
		fclose(input);
		if (output != NULL)
			fclose(output);
			
	}

	return 0;
}
