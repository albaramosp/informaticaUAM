/**
 * @brief Programa que obtiene la clave de un cifrado Vigenere mediante el metodo de indices de coincidencia.
 * El programa se llama con la siguiente estructura:
 * ./obtenerClave [-n clave ] [-i file in ] [-o file out ] 
 * -n tamaño clave 
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file obtenerClave.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 13-10-2020
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int textlen;
char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
float frecuenciasIdioma[26] = {
                                11.96, 0.92, 2.92, 6.87, 16.78, 
                                0.52, 0.73, 0.89, 4.15, 0.3, 
                                0, 8.37, 2.12, 7.01, 8.69, 2.77, 
                                1.53, 4.94, 7.88, 3.31, 4.80, 0.39, 
                                0, 0.06, 1.54, 0.15
                            };

void eliminarEspacios(char* s) {
    char *t1 = s, *t2 = s;
    char c;
    int i;

    for (i = 0; i < textlen; i++){
        c = *t1++;

        if (c >= 'A' && c <= 'Z'){
            *t2++ = c;
        }
    }

    textlen = t2-s;
}

float distanciaVectorial(float frecuenciasDesencriptado[26]){
    int i;
    float dist = 0;

    for (i = 0; i < 26; i++){
        dist += pow((frecuenciasDesencriptado[i] - frecuenciasIdioma[i]), 2);
    }

    return sqrt(dist);
}

char * obtenerClave(int n, char *texto){
    int i, j, k, l;
    int descifrado;
    char letra;
    char *clave;
    float frecuenciasDesencriptado[26];
    float distancia, maxDistancia;

    clave = malloc((n+1) * sizeof (char));

    /* 
     * Recorre cada vector y_i, tantos como tamaño tenga la clave (n).
     * Son los caracteres reorganizados por columnas del tamaño del bloque
     * que crea el algoritmo.
     */ 
    for (i = 0; i < n; i++){
        printf("\nBuscando k_%d...\n", i);
        maxDistancia = -1;

        /* Probamos cada letra del alfabeto como posible clave en cada uno de los vectores */
        for (k = 'A'; k <= 'Z'; k++){
            /* Inicializamos frecuencias para cada vector con cada prueba */
            for (l = 0; l < 26; l++){
                frecuenciasDesencriptado[l] = 0;
            }

            /* 
             * Recorre cada vector reagrupado por columnas y1, y2 ... yn,
             * donde cada uno ha sido cifrado con k1, k2 ... kn 
             */
            for (j = i; j < textlen; j += n){
                /* x_j = y_j - k_j (mod m) */
                descifrado = (texto[j] - 65) - (k - 65); /* Resta para tener las letras como A=0, B=1...*/
                
                if (descifrado >= 0){
                    letra = alfabeto[descifrado % 26];
                } else {
                    letra = alfabeto[(descifrado + 26) % 26];
                }

                frecuenciasDesencriptado[letra - 65] += 1; /* Vamos guardando cada frecuencia con desplazamiento */
            }
            
            /* 
             * Se deja preparado para hallar el IC(x_j) = n/l * sum (P_i * F_i+k_j)
             * con n = tam. texto y l = nº caracteres alfabeto.
             */
            for (l = 0; l < 26; l++){
                frecuenciasDesencriptado[l] /= (textlen / n); 
            }

            /* 
             * Para cada letra que probemos en la clave, calcularemos la distancia entre los vectores
             * de frecuencias obtenidas al desencriptar y de frecuencias del castellano.
             * Aquel vector de frecuencias obtenidas que más se ajuste al del castellano,
             * ese será el que indica que la letra elegida como clave es la más adecuada, porque tendrá
             * menor distancia.
             */
            distancia = distanciaVectorial(frecuenciasDesencriptado);
            printf("\tSi k_%d = %c, distancia obtenida = %lf y menor = %lf\n", i, k, distancia, maxDistancia);
            
            if (maxDistancia == -1 || maxDistancia > distancia){
                maxDistancia = distancia;
                clave[i] = k;
                printf("\t\t---> Tomamos k_%d = %c\n", i, k);
            }
        }

        /* Al final de recorrer todas las letras, tendremos la k_j más adecuada */
    }

    clave[n] = '\0';
    return clave;
}

int main(int argc, char *argv[]){
    char *mensaje;
	FILE *input = NULL;
	FILE *output = NULL;
	int n;
	char *clave;
    
	if (argc < 2 || argc > 7){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("Clave {-n clave} [-i f ile in ] [-o f ile out ]\n");
	
		printf("-n tamaño de la clave\n");
		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		n = atoi(argv[2]);

        if (n < 1 || n > 255){
                printf("ERROR: el tamaño de clave debe ser 1 <= k <= 255\n");
                return 1;
        }

        if(argc > 3 && strcmp("-i", argv[3]) == 0){
            input = fopen(argv[4], "r");
            if (input==NULL){
                printf("Error en apertura de fichero de entrada");
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
            printf("\nIntroducir el mensaje sobre el que obtener la clave: ");		
            scanf ("%m[^\n]%*c", &mensaje);
            textlen = strlen(mensaje);
        }

        eliminarEspacios(mensaje);
        clave = obtenerClave(n, mensaje); 
        printf("\n");
 	
	  	/*entrada por fichero y salida por pantalla*/	
		if (argc == 4){
		    printf("La clave es: %s\n", clave);
		/*entrada por teclado y salida por fichero*/
		} else if (argc > 3 && strcmp(argv[3], "-o") == 0) {
	 	    output = fopen(argv[4], "w");
		    fputs(clave, output);
		    fclose(output);
		/*entrada y salida por fichero*/
		} else if (argc == 7){
	 	    output = fopen(argv[6], "w");
	 	    fputs(clave, output);
		    fclose(output);
		/*salida por pantalla*/        
		} else {
		    printf("La clave es: %s\n", clave);
		}

		printf("\n");
		free(mensaje);
		free(clave);

    }
    return (0);
}