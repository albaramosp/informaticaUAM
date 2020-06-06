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
  printf("\n\nEscribir segmento BSS:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  }

  fprintf(fpasm, "\n\nsegment .bss\n\t__esp   resd 1");
}

/*
Declaración (con directiva db) de las variables que contienen el texto de los
mensajes para la identificación de errores en tiempo de ejecución.
En este punto, al menos, debes ser capaz de detectar la división por 0.
*/
void escribir_subseccion_data(FILE* fpasm){
  printf("\n\nEscribir segmento DATA:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  }

  fprintf(fpasm, "\n\nsegment .data\n\t_div_zero db %cError, division por cero%c, 0", '"', '"');
  fprintf(fpasm, "\n\t_err_out_range db %cError, indice fuera de rango%c, 0", '"', '"');
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
  printf("\n\nDeclara variable en BSS:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre){
      printf("\n\nError: nombre de la variable nulo\n");
      return;
  } else if (tipo != BOOLEANO && tipo != ENTERO){
    printf("\n\nError: tipo de datos %d no reconocido\n", tipo);
    return;
  } else if (tamano < 1) {
    printf("\n\nError: tamano igual a 0 o menor\n");
    return;
  }

  //TODO: preguntar si el TIPO sirve para algo, en el enunciado dice que siempre usaremos enteros (resd, dd)
  fprintf(fpasm, "\n\t_%s  resd %d", nombre, tamano);
}

/*
Para escribir el comienzo del segmento .text, básicamente se indica que se
exporta la etiqueta main y que se usarán las funciones declaradas en la librería
alfalib.o
*/
void escribir_segmento_codigo(FILE* fpasm){
  printf("\n\nDeclarar seg. codigo y su inicio:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  }

  //Librerias de alfalib
  fprintf(fpasm, "\n\nsegment .text\n\tglobal main\n\textern print_string, print_endofline, print_blank, scan_int, print_int, print_boolean, scan_boolean");
}

/*
En este punto se debe escribir, al menos, la etiqueta main y la sentencia que
guarda el puntero de pila en su variable (se recomienda usar __esp).
*/
void escribir_inicio_main(FILE* fpasm){
  printf("\n\nEscribir inicio del main:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  }

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
  printf("\n\nEscribir fin:\n");
  if (!fpasm){
    printf("\n\nError el fichero es nuloooooooo\n");
    return;
  }

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
void escribir_operando(FILE* fpasm, char* nombre, int es_variable){
  printf("\n\nEscribir operando:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre){
    printf("\n\nError: nombre nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1, vale %d\n", es_variable);
    return;
  }

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
  printf("\n\nAsignar:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre){
    printf("\n\nError: nombre nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1, vale %d\n", es_variable);
    return;
  }

  if (es_variable == 0){
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov [_%s], eax", nombre);
  } else {
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov _%s, eax", nombre);
  }

  fprintf(fpasm, "\n");
}

/* FUNCIONES ARITMÉTICO-LÓGICAS BINARIAS */
/*
En todas ellas se realiza la operación como se ha resumido anteriormente:
- Se extrae de la pila los operandos
- Se realiza la operación
- Se guarda el resultado en la pila
Los dos últimos argumentos indican respectivamente si lo que hay en la pila es
una referencia a un valor o un valor explícito.
Deben tenerse en cuenta las peculiaridades de cada operación. En este sentido
sí hay que mencionar explícitamente que, en el caso de la división, se debe
controlar si el divisor es “0” y en ese caso se debe saltar a la rutina de error
controlado (restaurando el puntero de pila en ese caso y comprobando en el retorno
que no se produce “Segmentation Fault”)
*/
void sumar(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nSumar:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tadd eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void restar(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nRestar:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tsub eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void multiplicar(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nMultiplicar:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\timul edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void dividir(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nDividir:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tmov edx, 0"); //Dividendo en edx:eax
  fprintf(fpasm, "\n\tpop dword ecx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword ecx, [ecx]");
  }

  fprintf(fpasm, "\n\tcmp ecx, 0"); //Si el divisor es 0, salto al final y controlo el error de ejecucion
  fprintf(fpasm, "\n\tje _err_div_0");
  fprintf(fpasm, "\n\tidiv ecx"); //edx:eax / ecx
  fprintf(fpasm, "\n\tpush dword eax"); //cociente en eax
  fprintf(fpasm, "\n");
}

void o(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nO (OR):\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tor eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

void y(FILE* fpasm, int es_variable_1, int es_variable_2){
  printf("\n\nY (AND):\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable_1 != 0 && es_variable_1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable_2 != 0 && es_variable_2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx");
  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable_1 == 1){ //eax, op1
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  if (es_variable_2 == 1){ //edx, op2
    fprintf(fpasm, "\n\tmov dword edx, [edx]");
  }

  fprintf(fpasm, "\n\tand eax, edx");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}

/*
Función aritmética de cambio de signo.
Es análoga a las binarias, excepto que sólo requiere de un acceso a la pila ya
que sólo usa un operando.
*/
void cambiar_signo(FILE* fpasm, int es_variable){
  printf("\n\nNegacion:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword eax");

  if (es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }
  fprintf(fpasm, "\n\tneg eax");
  fprintf(fpasm, "\n\tpush dword eax");
  fprintf(fpasm, "\n");
}


/*
Función monádica lógica de negación. No hay un código de operación de la ALU
que realice esta operación por lo que se debe codificar un algoritmo que, si
encuentra en la cima de la pila un 0 deja en la cima un 1 y al contrario.
El último argumento es el valor de etiqueta que corresponde (sin lugar a dudas,
la implementación del algoritmo requerirá etiquetas). Véase en los ejemplos de
programa principal como puede gestionarse el número de etiquetas cuantos_no.
*/
void no(FILE* fpasm, int es_variable, int cuantos_no){
  printf("\n\nNO:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0){
    printf("\n\nError: es_variable no es ni 0 ni 1\n");
    return;
  } else if (cuantos_no < 0){
    printf("\n\nError: cuantos_no fuera de rango\n");
    return;
  }

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

/* FUNCIONES COMPARATIVAS */
/*
Todas estas funciones reciben como argumento si los elementos a comparar son o
no variables. El resultado de las operaciones, que siempre será un booleano (“1”
si se cumple la comparación y “0” si no se cumple), se deja en la pila como en el
resto de operaciones. Se deben usar etiquetas para poder gestionar los saltos
necesarios para implementar las comparaciones.
*/

void igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta){
  printf("\n\nIgual:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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
  printf("\n\nDistinto:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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
  printf("\n\nMenor_Igual:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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
  printf("\n\nMayor_Igual:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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
  printf("\n\nMenor:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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
  printf("\n\nMayor:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable1 != 0 && es_variable1 != 1){
    printf("\n\nError: es_variable1 no es ni 0 ni 1\n");
    return;
  } else if (es_variable2 != 0 && es_variable2 != 1){
    printf("\n\nError: es_variable2 no es ni 0 ni 1\n");
    return;
  }

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

/* FUNCIONES DE ESCRITURA Y LECTURA */
/*
Se necesita saber el tipo de datos que se va a procesar (ENTERO o BOOLEANO) ya
que hay diferentes funciones de librería para la lectura (idem. escritura) de cada
tipo.
Se deben insertar en la pila los argumentos necesarios, realizar la llamada
(call) a la función de librería correspondiente y limpiar la pila.
*/
void leer(FILE* fpasm, char* nombre, int tipo){
  printf("\n\nLeer:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre){
    printf("\n\nError: nombre nulo\n");
    return;
  } else if (tipo != ENTERO && tipo != BOOLEANO){
    printf("\n\nError: tipo de datos no reconocido\n");
    return;
  }

  fprintf(fpasm, "\n\tpush dword _%s", nombre); //apilar la direccion donde queremos guardar el valor leido

  if (tipo == ENTERO){
    fprintf(fpasm, "\n\tcall scan_int");
    fprintf(fpasm, "\n\tadd esp, 4");
  } else {
    fprintf(fpasm, "\n\tcall scan_boolean");
    fprintf(fpasm, "\n\tadd esp, 4");
  }

  fprintf(fpasm, "\n");
}

void escribir(FILE* fpasm, int es_variable, int tipo){
  printf("\n\nEscribir:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1\n");
    return;
  } else if (tipo != ENTERO && tipo != BOOLEANO){
    printf("\n\nError: tipo de datos no reconocido\n");
    return;
  }

  if (es_variable != 0){
    fprintf(fpasm, "\n\tpop dword eax");
    fprintf(fpasm, "\n\tmov eax, [eax]");
    fprintf(fpasm, "\n\tpush dword eax");
  }

  if (tipo == ENTERO){
    fprintf(fpasm, "\n\tcall print_int");
    fprintf(fpasm, "\n\tadd esp, 4");
  } else {
    fprintf(fpasm, "\n\tcall print_boolean");
    fprintf(fpasm, "\n\tadd esp, 4");
  }

  fprintf(fpasm, "\n\tcall print_endofline");

  fprintf(fpasm, "\n");
}


/* ESTRUCTURAS DE ETIQUETAS
cmp cosas
je/jg... _then1
jmp _else1

_then1:
  code
  jmp _continue1

_else1:
  code

_continue1:
  code


cmp cosas
je/jg... _while0
jmp _continue0

_while0:
  code
  condicion de salida _continue0
  jmp _while0

_continue0:
  code

*/

void ifthenelse_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
  printf("\n\nITE_inicio:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (exp_es_variable != 0 && exp_es_variable != 1){
    printf("\n\nError: exp_es_variable no es ni 0 ni 1\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\txor eax, eax");
  fprintf(fpasm, "\n\tjz near _fin_then_%d", etiqueta);
}

void ifthenelse_fin_then( FILE * fpasm, int etiqueta){
  printf("\n\nITE_fin_then:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\tjmp near _continua_ifelse_%d", etiqueta);
  fprintf(fpasm, "\n\t_fin_then_%d:", etiqueta);
}

void ifthenelse_fin( FILE * fpasm, int etiqueta){
  printf("\n\nITE_fin:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\t_continua_ifelse_%d:", etiqueta);
}

void ifthen_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
  printf("\n\nIT_inicio:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (exp_es_variable != 0 && exp_es_variable != 1){
    printf("\n\nError: exp_es_variable no es ni 0 ni 1\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\txor eax, eax");
  fprintf(fpasm, "\n\tjz near _fin_then_%d", etiqueta);
}

void ifthen_fin(FILE * fpasm, int etiqueta){
  printf("\n\nIT_fin:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\t_fin_then_%d:", etiqueta);
}

void while_inicio(FILE * fpasm, int etiqueta){
  printf("\n\nW_inicio:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\t_inicio_while_%d:", etiqueta);
}


void while_exp_pila (FILE * fpasm, int exp_es_variable, int etiqueta){
  printf("\n\nW_pila:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (exp_es_variable != 0 && exp_es_variable != 1){
    printf("\n\nError: exp_es_variable no es ni 0 ni 1\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword eax");

  if (exp_es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tcmp eax, 0");
  fprintf(fpasm, "\n\tje near _fin_while_%d", etiqueta);
}

void while_fin( FILE * fpasm, int etiqueta){
  printf("\n\nW_fin:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (etiqueta < 0){
    printf("\n\nError: etiqueta fuera de rango\n");
    return;
  }

  fprintf(fpasm, "\n\tjmp near _inicio_while_%d", etiqueta);
  fprintf(fpasm, "\n\t_fin_while_%d:", etiqueta);
}

void escribir_elemento_vector(FILE * fpasm,char * nombre_vector,
                              int tam_max, int exp_es_direccion){
  printf("\n\nVector:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre_vector){
    printf("\n\nError: nombre de vector nulo\n");
    return;
  } else if (tam_max < 0){
    printf("\n\nError: tamanyo maximo de vector negativo\n");
    return;
  } else if (exp_es_direccion != 0 && exp_es_direccion != 1){
    printf("\n\nError: direccion no es ni cero ni uno\n");
    return;
  }

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
  printf("\n\ndeclaraFunct:\n");
  if (!fd_asm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre_funcion){
    printf("\n\nError: nombre de funcion nulo\n");
    return;
  } else if (num_var_loc < 0){
    printf("\n\nError: numero de variables negativo\n");
    return;
  }

  fprintf(fd_asm, "\n\n_%s:", nombre_funcion);
  fprintf(fd_asm, "\n\tpush ebp");
  fprintf(fd_asm, "\n\tmov ebp, esp");
  fprintf(fd_asm, "\n\tsub esp, %d", 4*num_var_loc);
}

void retornarFuncion(FILE * fd_asm, int es_variable){
  printf("\n\nretFunct:\n");
  if (!fd_asm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1\n");
    return;
  }

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

  printf("\n\nwriteParam:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (pos_parametro > num_total_parametros){
    printf("\n\nError: la posicion que quieres es negativa segun los parametros que tienes\n");
    return;
  }

  d_ebp = 4*(1 + (num_total_parametros - pos_parametro));

  fprintf(fpasm, "\n\tlea eax, [ebp + %d]", d_ebp);
  fprintf(fpasm, "\n\tpush dword eax");
}

void escribirVariableLocal(FILE* fpasm, int posicion_variable_local){
  int d_ebp;

  printf("\n\nwriteLocal:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  }

  d_ebp = 4*posicion_variable_local;

  fprintf(fpasm, "\n\tlea eax, [ebp - %d]", d_ebp);
  fprintf(fpasm, "\n\tpush dword eax");
}

void asignarDestinoEnPila(FILE* fpasm, int es_variable){
  printf("\n\nDestPila:\n");
  if (!fpasm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni cero ni uno\n");
    return;
  }

  fprintf(fpasm, "\n\tpop dword edx"); //Destino
  fprintf(fpasm, "\n\tpop dword eax"); //Valor

  if (es_variable == 1){
    fprintf(fpasm, "\n\tmov dword eax, [eax]");
  }

  fprintf(fpasm, "\n\tmov dword [edx], eax");
}

void operandoEnPilaAArgumento(FILE * fd_asm, int es_variable){
  printf("\n\nopPilaArg:\n");
  if (!fd_asm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (es_variable != 0 && es_variable != 1){
    printf("\n\nError: es_variable no es ni 0 ni 1\n");
    return;
  }

  if (es_variable == 1){
    fprintf(fd_asm, "\n\tpop dword eax");
    fprintf(fd_asm, "\n\tmov eax, [eax]");
    fprintf(fd_asm, "\n\tpush dword eax");
  }
}

void llamarFuncion(FILE * fd_asm, char * nombre_funcion, int num_argumentos){
  printf("\n\ncallFunct:\n");
  if (!fd_asm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (!nombre_funcion){
    printf("\n\nError: nombre de funcion nulo\n");
    return;
  } else if (num_argumentos < 0){
    printf("\n\nError: numero de args negativo\n");
    return;
  }

  fprintf(fd_asm, "\n\tcall _%s", nombre_funcion);
  limpiarPila(fd_asm, num_argumentos);
  fprintf(fd_asm, "\n\tpush dword eax");
}

void limpiarPila(FILE * fd_asm, int num_argumentos){
  printf("\n\nlimpiaPila:\n");
  if (!fd_asm){
    printf("\n\nError: fichero nulo\n");
    return;
  } else if (num_argumentos < 0){
    printf("\n\nError: numero de args negativo\n");
    return;
  }

  fprintf(fd_asm, "\n\tadd esp, %d", num_argumentos*4);
}
