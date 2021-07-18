/**
 * @brief Programa que cifra y descifra utilizando el cifrado de flujo RC4. Para cifrar espera 
 * recibir una cadena de texto o un fichero del que leerla, pero para descifrar la cadena debe contener
 * la representacion hexadecimal de lo que se cifró. 
 * El programa se llama con la siguiente estructura:
 * flujo {-k clave} [-i f ile in ] [-o f ile out ] 
 * -k cadena con la clave
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file flujo.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 13-10-2020
 * @copyright GNU Public License
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TXT 2048

unsigned char s[256]; /* Vector que sera permutado cada vez y se utilizara para la XOR con el mensaje */

void eliminarEspacios(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

void intercambiar(unsigned char *p1, unsigned char *p2){
	unsigned char t;
    
    t = *p1;
	*p1 = *p2;
	*p2 = t;
}


/*
 * Key-Scheduling Algorithm
 * 
 * 
 *
 */
void ksa(unsigned char *key, int n){
	int	i, j;

	for (i = 0; i < 256; i++){
		s[i] = i; /* Se inicializa como permutacion identidad */
	}

    /* Permutacion inicial del vector s */
    j = 0;
	for (i = 0; i < 256; i++){
		j = (j + s[i] + key[i % n]) % 256;
		intercambiar(&s[i], &s[j]);
	}
}

/*
 * Pseudo-Random Generation Algorithm
 */
void prga(char *text, int textlen){
    int i, j, cont;
    unsigned char k;

    i = 0;
    j = 0;

    printf("Flujo de claves: ");

    for (cont = 0; cont < textlen; cont++){
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		intercambiar(&s[i], &s[j]);
		k = s[(s[i] + s[j]) % 256];
        printf("%02x ", (unsigned char)k);
		
        text[cont] = text[cont] ^ k;
	}

    printf("\n");
}

int main(int argc,char ** argv){
    int textlen, n, i;
    FILE *input, *output;
    char *mensaje;
    unsigned char *k;
    char aux[3];
    int j=0;
    int num;

    if (argc < 4 || argc > 8){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("flujo {-C|-D} {-k clave} [-i f ile in ] [-o f ile out ]\n");

        printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");

		printf("-k cadena con clave\n");

		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
        n = strlen(argv[3]);
        if (n < 1 || n > 255){
            printf("ERROR: el tamaño de clave debe ser 1 <= k <= 255\n");
            return 1;
        }

        k = malloc(n * sizeof(char));
        memcpy(k, argv[3], n); /* Porque strcpy no vale para unsigned char */

        if(argc > 4 && strcmp("-i", argv[4]) == 0){
            input = fopen(argv[5], "r");
            if (input==NULL){
                printf("Error en apertura de fichero");
                exit(1);					
            }
            
            fseek(input, 0, SEEK_END);
            textlen = ftell(input);
            rewind(input);

            mensaje = malloc(textlen * sizeof(char));
            fread(mensaje, 1, textlen, input);
            fclose(input);
        }

        /* Si no se proporciona el parámetro -i, se procesará la entrada estándar */
        else {
            if (strcmp(argv[1], "-C") == 0){
                printf("\nIntroducir el mensaje a cifrar: ");
            } else if (strcmp(argv[1], "-D") == 0) {
                printf("\nIntroducir el mensaje a descifrar en formato hexadecimal y ");
                printf("con ceros rellenando (ej. en vez de 1A B 3D poner 1A 0B 3D): ");
            } else{
                printf("\nERROR: opcion desconocida\n");
                free(k);
                return 1;
            }
            scanf ("%m[^\n]%*c", &mensaje);
            textlen = strlen(mensaje);
        }

        eliminarEspacios(mensaje);
        textlen = strlen(mensaje);

        if (strcmp(argv[1], "-D") == 0) {
            for (i = 0; i < textlen; i+=2){
                sprintf(aux, "%c%c", mensaje[i], mensaje[i+1]);
                num = (int)strtol(aux, NULL, 16);
                mensaje[j] = num;
                j++;
            }

            mensaje[j] = '\0';
            textlen = strlen(mensaje);
        }
                
        ksa(k, n);
        prga(mensaje, textlen);
        
        /* Si no se proporciona el parámetro -o, se imprimira en la entrada estándar */
        if (argc == 4){
            printf("Resultado: ");
            for (i = 0; i < textlen; i++){
                printf("%02x ", (unsigned char)mensaje[i]);
            }
        } else if (argc > 4 && strcmp(argv[4], "-o") == 0) {
            output = fopen(argv[5], "w");
            for (i = 0; i < textlen; i++){
                fprintf(output, "%02x ", (unsigned char)mensaje[i]);
            }
            fclose(output);
        } else if (argc == 8){
            output = fopen(argv[7], "w");
            for (i = 0; i < textlen; i++){
                fprintf(output, "%02x ", (unsigned char)mensaje[i]);
            }
            fclose(output);
        } else {
            printf("Resultado: ");
            for (i = 0; i < textlen; i++){
                printf("%02x",(unsigned char)mensaje[i]);
            }
        }

        printf("\n");
        free(k);
        free(mensaje);
    }

	return 0;
}