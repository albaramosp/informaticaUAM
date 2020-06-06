/*
 * Librería que define las estructuras y funciones necesarias para el manejo de una
 * tabla hash para manejar datos de acuerdo a las explicaciones de las diapositivas
 * vistas en clase.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#define POS_INI_PARAMS 0
#define POS_INI_VARS_LOCALES 1

#define HASH_INI 5381
#define HASH_FACTOR 33

typedef enum {
  ERROR = 0,
  OK = 1
} Status;

typedef enum {
  VARIABLE,
  PARAMETRO,
  FUNCION
} Categoria;

typedef enum {
  ENTERO,
  BOOLEANO
} Tipo;

typedef enum {
  ESCALAR,
  VECTOR
} Clase;

typedef struct {
  char lexema[50];
  Categoria categoria;
  Tipo tipo;
  Clase clase;
  int tam;
  int adicional1;
  int adicional2;
} INFO_SIMBOLO;

typedef struct nodo_hash{
  INFO_SIMBOLO *info;
  struct nodo_hash *siguiente;
} NODO_HASH;

typedef struct {
  int tam;
  NODO_HASH **tabla;
} TABLA_HASH;

INFO_SIMBOLO *crear_info_simbolo(const char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adicional1, int adicional2);
void liberar_info_simbolo(INFO_SIMBOLO *info);
NODO_HASH *crear_nodo(INFO_SIMBOLO *info);
void liberar_nodo(NODO_HASH *nodo);
TABLA_HASH *crear_tabla(int tam);
void liberar_tabla(TABLA_HASH *th);
unsigned long hash(TABLA_HASH *th, const char *str);
INFO_SIMBOLO *buscar_simbolo(TABLA_HASH *th, const char *lexema);
Status insertar_simbolo(TABLA_HASH *th, const char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adic1, int adic2);
void borrar_simbolo(TABLA_HASH *th, const char *lexema);
