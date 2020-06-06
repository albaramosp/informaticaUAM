/**
 * @brief Libreria que contiene todas las constantes, tipos de datos y estructuras manejadas por la aplicacion.
 * @file simulador.h
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-04-2019
 */

#ifndef SRC_SIMULADOR_H_
#define SRC_SIMULADOR_H_

#include <stdbool.h>

#define N_EQUIPOS 3 /*!< Número de equipos*/
#define N_NAVES 3 /*!< Número de naves por equipo*/

/*** SCREEN ***/
extern char symbol_equipos[N_EQUIPOS]; /*!< Símbolos de los diferentes equipos en el mapa (mirar mapa.c)*/
#define MAPA_MAXX 10 /*!< Número de columnas del mapa*/
#define MAPA_MAXY 10 /*!< Número de filas del mapa*/
#define SCREEN_REFRESH 10000 /*!< Frequencia de refresco del mapa en el monitor*/
#define SYMB_VACIO '.' /*!< Símbolo para casilla vacia*/
#define SYMB_TOCADO '%' /*!< Símbolo para tocado*/
#define SYMB_DESTRUIDO 'X' /*!< Símbolo para destruido*/
#define SYMB_AGUA 'w' /*!< Símbolo para agua*/

/*** SIMULACION ***/
#define VIDA_MAX 5 /*!< Vida inicial de una nave*/
#define ATAQUE_ALCANCE 8 /*!< Distancia máxima de un ataque*/
#define ATAQUE_DANO 10 /*!< Daño de un ataque*/
#define MOVER_ALCANCE 1 /*!< Máximo de casillas a mover*/
#define TURNO_SECS 5 /*!< Segundos que dura un turno*/

/**
 * @enum Accion
 * @brief enumarado con las posibles acciones que se enviaran a los jefes, a las naves y al simulador.
 */
typedef enum {
	TURNO,
	ATACAR,
	FIN,
	MOVER,
	DESTRUIR
} Accion;

/**
 * @struct Mensaje
 * @brief Estructura que almacena los campos necesarios para gestionar los mensajes entre naves y
 * simulador, enviados a traves de cola de mensajes desde las naves. En el mensaje se almacenan la
 * accion a realizar por la nave y sus coordenadas de origen y destino en caso de movimiento,
 * o las coordenadas del atacante y el objetivo en caso de ataque.
 */
typedef struct {
	Accion accion;
	int naveOrigen;
	int equipoOrigen;
	int origenX;
	int origenY;
	int naveDest;
	int equipoDest;
	int destX;
	int destY;
} Mensaje;

/**
 * @struct Orden
 * @brief Estructura que almacena los campos necesarios para gestionar los mensajes entre el simulador y los jefes,
 * que incluye la orden que realizara el jefe y la nave destinataria en caso de que sea DESTRUIR
 */
typedef struct{
	Accion accion;
	int dest;
} Orden;


/*** MAPA ***/

/**
 * @struct tipo_nave
 * @brief Estructura que almacena la informacion sobre una nave
 */
typedef struct {
	/// Vida que le queda a la nave
	int vida;
	/// Columna en el mapa
	int posx;
	/// Fila en el mapa
	int posy;
	/// Equipo de la nave
	int equipo;
	/// Numero de la nave en el equipo
	int numNave;
	 /// Si la nave está viva o ha sido destruida
	bool viva;
} tipo_nave;

/**
 * @struct tipo_casilla
 * @brief Estructura que almacena la informacion sobre una casilla del mapa
 */
typedef struct {
	/// Símbolo que se mostrará en la pantalla para esta casilla
	char simbolo;
	/// Si está vacia = -1. Si no, número de equipo de la nave que está en la casilla
	int equipo;
	/// Número de nave en el equipo de la nave que está en la casilla
	int numNave;
} tipo_casilla;

/**
 * @struct tipo_mapa
 * @brief Estructura que almacena la informacion sobre el mapa. La estructura original proporcionada ha sido modificada
 * para incluir un contador de los mensajes leidos y enviados por la cola de mensajes, y de los equipos que se vayan
 * quedando sin naves. Esto facilita la reutilizacion de la zona de memoria compartida para el mapa, que permite
 * utilizar estos nuevos campos sin necesidad de crear mas memoria compartida.
 */
typedef struct {
	tipo_nave info_naves[N_EQUIPOS][N_NAVES];
	tipo_casilla casillas[MAPA_MAXY][MAPA_MAXX];
	/// Número de naves vivas en un equipo
	int num_naves[N_EQUIPOS];
	int contadorCola;
	int contadorPerdedores;
} tipo_mapa;

/**
 * @struct Tuberia
 * @brief Estructura que almacena tantos descriptores de ficheros como naves y jefes haya en la aplicacion. Se utilizara
 * para inicializar las tuberias.
 */
typedef struct{
	int fd [N_EQUIPOS + (N_NAVES*N_EQUIPOS)][2];
} Tuberia;


#define SHM_MAP_NAME "/shm_naves"

#endif /* SRC_SIMULADOR_H_ */
