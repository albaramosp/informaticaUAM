
#include "funcionesDES.h"


int main(int argc, char *argv[]){
	FILE *input = NULL, *output = NULL; 
	uint64_t mensaje=0, res1=0, res2=0, res3=0, clave1=0, clave2=0, clave3=0, iv=0; /* Asi ya viene con padding incluido, porque ocupa 64b si o si */
	int cifrar=0, cont=0, textlen=0;
	char k1[16], k2[16], k3[16];
	char buf[16], *mensajeTotal;
    
	if (argc < 6 || argc > 10){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("/.TDEA-CBC {-C | -D -k clave -iv vectorincializacion} [-i f ilein] [-o f ileout]\n");
		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");
		printf("-k clave en hexa de 192 bits, con 24 de paridad\n");
		printf("-iv vector de incialización\n");
		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0)
			cifrar = 1;
		else 
			cifrar = 0;

		/* La clave tiene 192B = 48 caracteres */
		if(strlen(argv[3]) != 48){
			printf("\nERROR: la clave debe tener 48 caracteres hexa, o sea 192 bytes con 24 de paridad\n");
			exit(1);
		}

		/* Metemos la primera clave en hexadecimal, los 16 primeros chars de la clave recibida por argumento */
		strncpy(k1, argv[3], 16);
		sscanf(k1,"%lX", &clave1);
		printf("\nK1: %lx\n", clave1);

		/* Metemos la segunda clave en hexadecimal, los 16 siguientes chars de la clave recibida por argumento */
		strncpy(k2, argv[3]+16, 16);
		sscanf(k2,"%lX", &clave2);
		printf("\nK2: %lx\n", clave2);

		/* Metemos la tercera clave en hexadecimal, los 16 ultimos chars de la clave recibida por argumento */
		strncpy(k3, argv[3]+32, 16);
		sscanf(k3,"%lX", &clave3);
		printf("\nK3: %lx\n", clave3);

		/* Comprobar la paridad de las claves */
		if (findParity(clave1) == 0 || findParity(clave2) == 0 || findParity(clave3) == 0){
			printf("\nERROR: la clave no tiene paridad, debería tenerla\n");
			exit(1);
		}
		
		/*metemos IV en hexadecimal*/
		sscanf(argv[5],"%lX", (uint64_t*)&iv);

		if((argc > 6 && strcmp("-i", argv[6]) == 0) || argc == 10){
			if (argc > 6 && strcmp("-i", argv[6]) == 0)
				input = fopen(argv[7], "rb");
			if (argc == 10)
				input = fopen(argv[9], "rb");

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
					/* Modo CBC */
					mensaje ^= iv;

					/* Aplicamos 3 veces DES */
					mensaje = des(mensaje, clave1, cifrar);
					mensaje = des(mensaje, clave2, !cifrar);
					mensaje = des(mensaje, clave3, cifrar);
					
					/* Modo CBC */
					iv = mensaje;

					printf("\nResultado: %016lx", iv);
				} else {
					/* Aplicamos 3 veces DES */
					res1 = des(mensaje, clave3, cifrar);
					res2 = des(res1, clave2, !cifrar);
					res3 = des(res2, clave1, cifrar);
					
					/* Modo CBC */
					res3 ^= iv;
					iv = mensaje;

					printf("\nResultado: %016lx", res3);
				}
				
				printf("\nIr introduciendo caracteres en bloques de 16 caracteres y pulsar CTRL+C para finalizar: ");
			}
		} else {
			if(output == NULL)
				printf("\nResultado: ");
			while(cont < textlen){
				strncpy(buf, mensajeTotal+cont, 16);
				sscanf(buf,"%lx", (uint64_t*)&mensaje);
				cont += 16;

				if (cifrar == 1){
					/* Modo CBC */
					mensaje ^= iv;

					/* Aplicamos 3 veces DES */
					mensaje = des(mensaje, clave1, cifrar);
					mensaje = des(mensaje, clave2, !cifrar);
					mensaje = des(mensaje, clave3, cifrar);
					
					/* Modo CBC */
					iv = mensaje;
					
					if(output == NULL)
						printf("%016lx", iv);
					else
						fprintf(output, "%016lx", iv);
				} else {
					/* Aplicamos 3 veces DES */
					res1 = des(mensaje, clave3, cifrar);
					res2 = des(res1, clave2, !cifrar);
					res3 = des(res2, clave1, cifrar);
					
					/* Modo CBC */
					res3 ^= iv;
					iv = mensaje;

					if(output == NULL)
						printf("%016lx", res3);
					else
						fprintf(output, "%016lx", res3);
				}
			}

			printf("\n");
		}
		
		fclose(input);

		/*free(k);
		free(iv);*/
			
	}

	return 0;
}
