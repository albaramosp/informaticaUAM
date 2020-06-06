/**
 * Implementacion de funciones para escribir programas .asm desde C
 * @author: Andrea Salcedo Lopez
 * @author: Alba Ramos Pedroviejo
 * @date: 30/10/2019
*/
#include "generacion.h"

/*
Código para el principio de la sección .bss.
Con seguridad sabes que deberás reservar una variable entera para guardar el
puntero de pila extendido (esp). Se te sugiere el nombre __esp para esta variable.
*/
void escribir_cabecera_bss(FILE* fpasm){
  fprintf(fpasm, "\n\nsegment .bss\n\t__esp   resd 1");
}

void escribir_subseccion_data(FILE* fpasm){
  fprintf(fpasm, "\n\nsegment .data\n\t_div_zero db %c****Error de ejecucion: Division por cero.%c, 0", '"', '"');
  fprintf(fpasm, "\n\t_err_out_range db %c****Error de ejecucion: Indice fuera de rango.%c, 0", '"', '"');
}

/*
Para ser invocada en la sección .bss cada vez que se quiera declarar una
variable:
- El argumento nombre es el de la variable.
- tipo puede ser ENTERO o BOOLEANO (observa la declaración de las constantes
del principio del fichero).
- Esta misma función se invocará cuando en el compilador se declaren
vectores, por eso se adjunta un argumento final (tamano) que para esta
primera práctica siempre recibirá el valor 1.
*/
void declarar_variable(FILE* fpasm, char * nombre, int tipo, int tamano){
  fprintf(fpasm, "\n\t_%s  resd %d", nombre, tamano);
}

/*
Para escribir el comienzo del segmento .text, básicamente se indica que se
exporta la etiqueta main y que se usarán las funciones declaradas en la librería
alfalib.o
*/
void escribir_segmento_codigo(FILE* fpasm){
  fprintf(fpasm, "\n\nsegment .text\n\tglobal main\n\textern print_string, print_endofline, print_blank, scan_int, print_int, print_boolean, scan_boolean");
}

/*
En este punto se debe escribir, al menos, la etiqueta main y la sentencia que
guarda el puntero de pila en su variable (se recomienda usar __esp).
*/
void escribir_inicio_main(FILE* fpasm){
  fprintf(fpasm, "\n\nmain:\n\tmov dword [__esp], esp");
}

/*
Al final del programa se escribe:
- El código NASM para salir de manera controlada cuando se detecta un error
en tiempo de ejecución (cada error saltará a una etiqueta situada en esta
zona en la que se imprimirá el correspondiente mensaje y se saltará a la
zona de finalización del programa).
- En el final del programa se debe:
·Restaurar el valor del puntero de pila (a partir de su variable __esp)
·Salir del programa (ret).
*/
void escribir_fin(FILE* fpasm){
  fprintf(fpasm, "\n\tjmp _fin");
  fprintf(fpasm, "\n");

  fprintf(fpasm, "\n_err_div_0:");
  fprintf(fpasm, "\n\tpush dword _div_zero");
  fprintf(fpasm, "\n\tcall print_string");
  fprintf(fpasm, "\n\tcall print_endofline");
  fprintf(fpasm, "\n\tadd esp, 4");
  fprintf(fpasm, "\n\tjmp _fin");
  fprintf(fpasm, "\n");

  fprintf(fpasm, "\n_err_vector_range:");
  fprintf(fpasm, "\n\tpush dword _err_out_range");
  fprintf(fpasm, "\n\tcall print_string");
  fprintf(fpasm, "\n\tcall print_endofline");
  fprintf(fpasm, "\n\tadd esp, 4");
  fprintf(fpasm, "\n\tjmp _fin");
  fprintf(fpasm, "\n");

  fprintf(fpasm, "\n_fin:");
  fprintf(fpasm, "\n\tmov dword esp, [__esp]");
  fprintf(fpasm, "\n\tret");
}


/*
Función que debe ser invocada cuando se sabe un operando de una operación
aritmético-lógica y se necesita introducirlo en la pila.
- nombre es la cadena de caracteres del operando tal y como debería aparecer
en el fuente NASM
- es_variable indica si este operando es una variable (como por ejemplo b1)
con un 1 u otra cosa (como por ejemplo 34) con un 0. Recuerda que en el
primer caso internamente se representará como _b1 y, sin embargo, en el
segundo se representará tal y como esté en el argumento (34).
*/

/*escribriidentificador*/
void escribir_operando(FILE* fpasm, char* nombre, int es_variable){
  if (es_variable == 0){
    fprintf(fpasm, "\n\tpush dword %s", nombre);
  } else {
    fprintf(fpasm, "\n\tpush dword _%s", nombre);
  }

  fprintf(fpasm, "\n");
}

/*
- Genera el código para asignar valor a la variable de nombre nombre.
- Se toma el valor de la cima de la pila.
- El último argumento es el que indica si lo que hay en la cima de la pila es
una referencia (1) o ya un valor explícito (0).
*/
void asignar(FILE* fpasm, char* nombre, int es_variable){
  if (es_variable == 0){
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov [_%s], eax", nombre);
  } else {
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov _%s, eax", nombre);
  }

  fprintf(fpasm, "\n");
}

void asignarElementoVector(FILE* fpasm, char* nombre, int es_variable){
  fprintf(fpasm, "\n\tpop dword eax"); /*Valor*/
  fprintf(fpasm, "\n\tpop dword edx"); /*Posicion*/

  if (es_variable == 1){
  	fprintf(fpasm, "\n\tmov eax, [eax]");
  }

  fprintf(fpasm, "\n\tmov [edx], eax");

  fprintf(fpasm, "\n");
}

void sumar(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tadd eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void restar(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tsub eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void multiplicar(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\timul edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void dividir(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tmov edx, 0");
  fprintf(fpasm, "\n\tpop dword ecx"); /*segundo*/
  fprintf(fpasm, "\n\tpop dword eax"); /*primero*/

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcdq");

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword ecx, [ecx]");
  }

  fprintf(fpasm, "\n\tcmp ecx, 0");
  fprintf(fpasm, "\n\tje _err_div_0");
  fprintf(fpasm, "\n\tidiv ecx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void o(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tor eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void y(FILE* fpasm, int es_variable_1, int es_variable_2){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tand eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void cambiar_signo(FILE* fpasm, int es_variable){
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }
  fprintf(fpasm, "\n\tneg eax");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void no(FILE* fpasm, int es_variable, int cuantos_no){
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tor eax, eax");
  fprintf(fpasm, "\n\tjz _negar0_%d", cuantos_no);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_negacion_%d", cuantos_no);

  fprintf(fpasm, "\n\n_negar0_%d:", cuantos_no);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\n_fin_negacion_%d:", cuantos_no);
  fprintf(fpasm, "\n\tpush dword eax");

}

void igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tje near _igual0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_igualdad_%d", etiqueta);

  fprintf(fpasm, "\n\t_igual0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_igualdad_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");
}

void distinto(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tjne near _distinto0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_distinto_%d", etiqueta);

  fprintf(fpasm, "\n\t_distinto0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_distinto_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");

}

void menor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tjle near _menorigual0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_menorigual_%d", etiqueta);

  fprintf(fpasm, "\n\t_menorigual0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_menorigual_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");

}

void mayor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tjge near _mayorigual0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_mayorigual_%d", etiqueta);

  fprintf(fpasm, "\n\t_mayorigual0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_mayorigual_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");

}

void menor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tjl near _menor0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_menor_%d", etiqueta);

  fprintf(fpasm, "\n\t_menor0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_menor_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");

}

void mayor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){
  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable1 == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable2 == 1){
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tcmp eax, edx");
  fprintf(fpasm, "\n\tjg near _mayor0_%d", etiqueta);

  fprintf(fpasm, "\n\tmov dword eax, 0");
  fprintf(fpasm, "\n\tjmp near _fin_mayor_%d", etiqueta);

  fprintf(fpasm, "\n\t_mayor0_%d:", etiqueta);
  fprintf(fpasm, "\n\tmov dword eax, 1");

  fprintf(fpasm, "\n\t_fin_mayor_%d:", etiqueta);
  fprintf(fpasm, "\n\tpush dword eax");

}

void leer(FILE* fpasm, char* nombre, int tipo){
  fprintf(fpasm, "\n\tpush dword _%s", nombre);

  if (tipo == INT){
    fprintf(fpasm, "\n\tcall scan_int");
    fprintf(fpasm, "\n\tadd esp, 4");
  } else {
    fprintf(fpasm, "\n\tcall scan_boolean");
    fprintf(fpasm, "\n\tadd esp, 4");
  }

  fprintf(fpasm, "\n");
}

void escribir(FILE* fpasm, int es_variable, int tipo){
  if (es_variable != 0){
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov eax, [eax]");
    fprintf(fpasm, "\n\tpush dword eax");
  }

  if (tipo == INT){
    fprintf(fpasm, "\n\tcall print_int");
    fprintf(fpasm, "\n\tadd esp, 4");
  } else {
    fprintf(fpasm, "\n\tcall print_boolean");
    fprintf(fpasm, "\n\tadd esp, 4");
  }

  fprintf(fpasm, "\n\tcall print_endofline");

  fprintf(fpasm, "\n");
}

/*escribirif*/
void ifthenelse_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcmp eax, 0");
  fprintf(fpasm, "\n\tje near _fin_then_%d\n", etiqueta);
}

/*escribirif_exp*/
void ifthenelse_fin_then( FILE * fpasm, int etiqueta){
  fprintf(fpasm, "\n\tjmp near _continua_ifelse_%d", etiqueta);
  fprintf(fpasm, "\n\t_fin_then_%d:", etiqueta);
}
/*escribirelse*/
void ifthenelse_fin( FILE * fpasm, int etiqueta){
  fprintf(fpasm, "\n\t_continua_ifelse_%d:", etiqueta);
}

/*escribirif*/
void ifthen_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcmp eax, 0");
  fprintf(fpasm, "\n\tje near _fin_then_%d\n", etiqueta);
}

/*escribirifend*/
void ifthen_fin(FILE * fpasm, int etiqueta){
  fprintf(fpasm, "\n\t_fin_then_%d:", etiqueta);
}

void while_inicio(FILE * fpasm, int etiqueta){
  fprintf(fpasm, "\n\t_inicio_while_%d:", etiqueta);
}


void while_exp_pila (FILE * fpasm, int exp_es_variable, int etiqueta){

  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcmp eax, 0");
  fprintf(fpasm, "\n\tje near _fin_while_%d", etiqueta);
}

void while_fin( FILE * fpasm, int etiqueta){

  fprintf(fpasm, "\n\tjmp near _inicio_while_%d", etiqueta);
  fprintf(fpasm, "\n\t_fin_while_%d:", etiqueta);
}

void escribir_elemento_vector(FILE * fpasm,char * nombre_vector,
                              int tam_max, int exp_es_direccion){

  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_direccion == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcmp eax, 0");
  fprintf(fpasm, "\n\tjl near _err_vector_range");

  fprintf(fpasm, "\n\tcmp eax, %d", tam_max-1);
  fprintf(fpasm, "\n\tjg near _err_vector_range");

  fprintf(fpasm, "\n\tmov dword edx, _%s", nombre_vector);
  fprintf(fpasm, "\n\tlea eax, [edx + eax*4]");
  fprintf(fpasm, "\n\tpush dword eax");
}

void declararFuncion(FILE * fd_asm, char * nombre_funcion, int num_var_loc){

  fprintf(fd_asm, "\n\n_%s:", nombre_funcion);
  fprintf(fd_asm, "\n\tpush ebp");
  fprintf(fd_asm, "\n\tmov ebp, esp");
  fprintf(fd_asm, "\n\tsub esp, %d\n", 4*num_var_loc);
}

void retornarFuncion(FILE * fd_asm, int es_variable){

  fprintf(fd_asm, "\n\tpop dword eax");

  if (es_variable == 1){
    fprintf(fd_asm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fd_asm, "\n\tmov esp, ebp");
  fprintf(fd_asm, "\n\tpop ebp");
  fprintf(fd_asm, "\n\tret");
}

void escribirParametro(FILE* fpasm, int pos_parametro, int num_total_parametros){
  int d_ebp;

  d_ebp = 4*(1 + (num_total_parametros - pos_parametro));

  fprintf(fpasm, "\n\tlea eax, [ebp + %d]", d_ebp);

  fprintf(fpasm, "\n\tpush dword eax"); /*Añadidos corchetes de lea param*/


}

void escribirVariableLocal(FILE* fpasm, int posicion_variable_local){
  int d_ebp;

  d_ebp = 4*posicion_variable_local;

  fprintf(fpasm, "\n\tlea eax, [ebp - %d]", d_ebp);
  fprintf(fpasm, "\n\tpush dword eax");
}

/*asignarIdentLocal*/
void asignarDestinoEnPila(FILE* fpasm, int es_variable){

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tmov dword [edx], eax\n");
}

void operandoEnPilaAArgumento(FILE * fd_asm, int es_variable){
  if (es_variable == 1){
    fprintf(fd_asm, "\n\tpop dword eax");
    fprintf(fd_asm, "\n\tmov eax, [eax]");
    fprintf(fd_asm, "\n\tpush dword eax");
  }
}

void llamarFuncion(FILE * fd_asm, char * nombre_funcion, int num_argumentos){

  fprintf(fd_asm, "\n\tcall _%s", nombre_funcion);
  limpiarPila(fd_asm, num_argumentos);
  fprintf(fd_asm, "\n\tpush dword eax");
}

void limpiarPila(FILE * fd_asm, int num_argumentos){

  fprintf(fd_asm, "\n\tadd esp, %d", num_argumentos*4);
}
