/*Programa que establece la gramatica de atributos necesaria para gestionar la correccion de programas en lenguaje ALFA*/
/*Incorpora atributos para manejar el analisis semantico del compilador.*/
/*Utiliza las funciones de generacion.h para ir traduciendo el programa de ALFA a NASM*/
/*Mantiene el analisis sintactico y las reglas de produccion de la practica anterior en el fichero reglasSintactico.txt*/
/*Alba Ramos Pedroviejo*/
/*Andrea Salcedo Lopez*/

%{
	#include "alfa.h"
	#include "generacion.h"

	void yyerror(char const *cadena);

	extern int yylex();
	extern int yyleng;

	extern FILE *yyin, *yyout, *reglasSintactico;
	extern int fila, columna;

	/*Gestion de errores morfologicos y semanticos*/
	extern int error;
	int errorSemantico=0;

	/*Ambitos del programa, tabla de simbolos*/
	TablaHash tablaGlobal = NULL;
	TablaHash tablaLocal = NULL;

	/* Fichero auxiliar para almacenar las reglas del analisis sintactico de la practica anterior */
	FILE *reglasSintactico;

	int etiqueta = 0; /*Gestion de bucles y condicionales*/

  	int abiertoAmbLocal=FALSE, en_funcion=FALSE, nretorno=0, llamada_dentro_funcion=0;

	char nombreFuncionActual[100];

	/*Guarda las ctes enteras en formato cadena para imprimirlas en fichero NASM*/
	char aux[15];

	/*Por defecto, clase vector. Asi evitamos el problema de escribir push en el .bss al declarar el tamaño de vectores*/
	/*Mas explicaciones en cada token*/
	int tipo_actual = INT, clase_actual = VECTOR, tamanio_vector_actual = 0;
	int pos_variable_local_actual = 0, num_variables_locales_actual = 0, num_param_actual = 0, pos_param_actual = 0;
	int en_explist = 0, num_parametros_llamada_actual = 0;
%}

%union{
	tipo_atributos atributos;
}

%token TOK_MAIN
%token TOK_INT
%token TOK_BOOLEAN
%token TOK_FUNCTION
%token TOK_ARRAY
%token TOK_IF
%token TOK_ELSE
%token TOK_WHILE
%token TOK_SCANF
%token TOK_PRINTF
%token TOK_RETURN

%token TOK_PUNTOYCOMA
%token TOK_COMA
%token TOK_LLAVEIZQUIERDA
%token TOK_LLAVEDERECHA
%token TOK_PARENTESISIZQUIERDO
%token TOK_PARENTESISDERECHO
%token TOK_CORCHETEIZQUIERDO
%token TOK_CORCHETEDERECHO
%token TOK_MAS
%token TOK_MENOS
%token TOK_DIVISION
%token TOK_ASTERISCO
%token TOK_AND
%token TOK_OR
%token TOK_ASIGNACION
%token TOK_DISTINTO
%token TOK_IGUAL
%token TOK_NOT
%token TOK_MENORIGUAL
%token TOK_MAYORIGUAL
%token TOK_MENOR
%token TOK_MAYOR

%token <atributos> TOK_TRUE
%token <atributos> TOK_FALSE
%token <atributos> TOK_IDENTIFICADOR
%token <atributos> TOK_CONSTANTE_ENTERA
%token TOK_ERROR

%type <atributos> clase exp clase_escalar clase_vector constante constante_logica constante_entera comparacion fn_name fn_declaration funcion identificadores identificador asignacion elemento_vector condicional if_exp if_exp_sentencias bucle while while_exp idf_llamada_funcion idpf

%left TOK_MAS TOK_MENOS TOK_OR
%left TOK_DIVISION TOK_ASTERISCO TOK_AND
%right TOK_NOT MENOSR
%start programa
%%

programa: abrir_amb_global TOK_MAIN TOK_LLAVEIZQUIERDA escribir_bss declaraciones escribir_data funciones escribir_main sentencias TOK_LLAVEDERECHA {
	fprintf(reglasSintactico,";R1:\t<programa> ::= main { <declaraciones> <funciones> <sentencias> }\n");
	fclose(reglasSintactico);
	escribir_fin(yyout);
	destruirTablaHash(tablaGlobal);
}
;

abrir_amb_global: {
	reglasSintactico=fopen("reglasSintactico.txt","w");

	if((tablaGlobal = crearTablaHash())==NULL){
			error=-1;
			errorSemantico=0;
			yyerror("Error al abrir ambito global.");
			return -1;
	}
}
;

escribir_bss:{
	escribir_cabecera_bss(yyout);
}
;

escribir_data:{
	clase_actual=1;
	escribir_subseccion_data(yyout);
	escribir_segmento_codigo(yyout);
}
;

escribir_main: {
	escribir_inicio_main(yyout);
}
;

declaraciones: declaracion {fprintf(reglasSintactico,";R2:\t<declaraciones> ::= <declaracion>\n");}
		| declaracion declaraciones {fprintf(reglasSintactico,";R3:\t<declaraciones> ::= <declaracion> <declaraciones>\n");}
;

declaracion: clase identificadores TOK_PUNTOYCOMA {
	$2.tipo=$1.tipo;
	fprintf(reglasSintactico,";R4:\t<declaracion> ::= <clase> <identificadores> ;\n");
}
;

clase: clase_escalar {
	/*Permite que en este caso si se escriban los push de las ctes enteras*/
	/*Valido para operaciones y para indexar en vectores*/
	clase_actual=ESCALAR;
	fprintf(reglasSintactico,";R5:\t<clase> ::= <clase_escalar>\n");
}
	| clase_vector {
		/*Al declarar vectores, no queremos hacer push de su tamanyo que es una cte entera*/
		clase_actual=VECTOR;
		fprintf(reglasSintactico,";R7:\t<clase> ::= <clase_vector>\n");
	}
;

clase_escalar: tipo {fprintf(reglasSintactico,";R9:\t<clase_escalar> ::= <tipo>\n");}
;

clase_vector: vector tipo TOK_CORCHETEIZQUIERDO constante_entera TOK_CORCHETEDERECHO {
	tamanio_vector_actual=$4.valor_entero;

	if(tamanio_vector_actual < 1 || tamanio_vector_actual > MAX_TAMANIO_VECTOR){
		error=-1;
	}

	fprintf(reglasSintactico,";R15:\t<clase_vector> ::= array <tipo> [ <constante_entera> ]\n");
}
;

vector: TOK_ARRAY{
	clase_actual = VECTOR;
}

tipo: TOK_INT{
	tipo_actual=INT;
	fprintf(reglasSintactico,";R10:\t<tipo> ::= int\n");
}
	|TOK_BOOLEAN{
		tipo_actual=BOOLEAN;
		fprintf(reglasSintactico,";R11:\t<tipo> ::= boolean\n");
	}
;

identificadores: identificador{
	$1.tipo=$$.tipo;
	fprintf(reglasSintactico,";R18:\t<identificadores> ::= <identificador>\n");
}| identificador TOK_COMA identificadores{
	$1.tipo=$$.tipo;$3.tipo=$$.tipo;
	fprintf(reglasSintactico,";R19:\t<identificadores> ::= <identificador> , <identificadores>\n");
}
;

funciones: funcion funciones{
	fprintf(reglasSintactico,";R20:\t<funciones> ::= <funcion> <funciones>\n");
}
	| /* VACIO */{
		fprintf(reglasSintactico,";R21:\t<funciones> ::=\n");
	}
;

funcion : fn_declaration sentencias TOK_LLAVEDERECHA {
	destruirTablaHash(tablaLocal);
	abiertoAmbLocal=FALSE;

	if (actualizarParamFuncion(tablaGlobal, $1.lexema, num_param_actual) == FALSE){
		error=-1;
		errorSemantico=0;
		yyerror("Error al actualizar parametros de funcion");
		return -1;
	}

	if(nretorno == 0){
		error=-1;
		errorSemantico=3;
		yyerror($1.lexema);
		return -1;
	}

	nretorno=0;
	en_funcion=FALSE;
	fprintf(reglasSintactico,";R22:\t<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }\n");
}
;

fn_name : TOK_FUNCTION tipo TOK_IDENTIFICADOR {
		if(abiertoAmbLocal==TRUE){
			error=-1;
			errorSemantico=0;
			yyerror("No se puede crear una funcion dentro de otra funcion");
			return -1;
		}

		if(insercion(tablaGlobal, $3.lexema, FUNCION, clase_actual,tipo_actual, tamanio_vector_actual, num_variables_locales_actual, pos_variable_local_actual,num_param_actual ,pos_param_actual) == FALSE){
			error=-1;
			errorSemantico=0;
			yyerror("Declaracion duplicada.");
			return -1;
		}

		if((tablaLocal = crearTablaHash()) == NULL){
			error=-1;
			errorSemantico=0;
			yyerror("Error al abrir el ambito local");
			return -1;
		}

		insercion(tablaLocal, $3.lexema, FUNCION, clase_actual, tipo_actual, tamanio_vector_actual, num_variables_locales_actual, pos_variable_local_actual,num_param_actual ,pos_param_actual);
		abiertoAmbLocal=TRUE;
		num_variables_locales_actual = 0;
		pos_variable_local_actual = 1;
		num_param_actual = 0;
		pos_param_actual = 0;
		strcpy($$.lexema,$3.lexema);
}
;

fn_declaration : fn_name TOK_PARENTESISIZQUIERDO parametros_funcion TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA declaraciones_funcion{
	if (actualizarParamFuncion(tablaLocal, $1.lexema, num_param_actual) == FALSE){
		error=-1;
		errorSemantico=0;
		yyerror("No se pueden actualizar los parametros de la funcion en el ambito local.");
		return -1;
	}

	en_funcion=TRUE;
	strcpy($$.lexema,$1.lexema);
	strcpy(nombreFuncionActual,$1.lexema);
	declararFuncion(yyout, $1.lexema,num_variables_locales_actual);
}
;

parametros_funcion: parametro_funcion resto_parametros_funcion {fprintf(reglasSintactico,";R23:\t<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>\n");}
			| /* VACIO */ {fprintf(reglasSintactico,";R24:\t<parametros_funcion> :=\n");}
;

resto_parametros_funcion: TOK_PUNTOYCOMA parametro_funcion resto_parametros_funcion {fprintf(reglasSintactico,";R25:\t<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>\n");}
			| /* VACIO */ {fprintf(reglasSintactico,";R26:\t<resto_parametros_funcion> ::=\n");}
;

parametro_funcion: tipo idpf {fprintf(reglasSintactico,";R27:\t<parametro_funcion> ::= <tipo> <identificador>\n");}
;

declaraciones_funcion: declaraciones {fprintf(reglasSintactico,";R28:\t<declaraciones_funcion> ::= <declaraciones>\n");}
			| /* VACIO */ {fprintf(reglasSintactico,";R29:\t<declaraciones_funcion> ::=\n");}
;

sentencias: sentencia {fprintf(reglasSintactico,";R30:\t<sentencias> ::= <sentencia>\n");}
	| sentencia sentencias {fprintf(reglasSintactico,";R31:\t<sentencias> ::= <sentencia> <sentencias>\n");}
;

sentencia: sentencia_simple TOK_PUNTOYCOMA {fprintf(reglasSintactico,";R32:\t<sentencia> ::= <sentencia_simple> ;\n");}
	| bloque {fprintf(reglasSintactico,";R33:\t<sentencia> ::= <bloque>\n");}
;

sentencia_simple: asignacion {fprintf(reglasSintactico,";R34:\t<sentencia_simple> ::= <asignacion>\n");}
		| lectura {fprintf(reglasSintactico,";R35:\t<sentencia_simple> ::= <lectura>\n");}
		| escritura {fprintf(reglasSintactico,";R36:\t<sentencia_simple> ::= <escritura>\n");}
		| retorno_funcion {fprintf(reglasSintactico,";R38:\t<sentencia_simple> ::= <retorno_funcion>\n");}
;

bloque: condicional {fprintf(reglasSintactico,";R40:\t<bloque> ::= <condicional>\n");}
	| bucle {fprintf(reglasSintactico,";R41:\t<bloque> ::= <bucle>\n");}
;

asignacion: TOK_IDENTIFICADOR TOK_ASIGNACION exp {
	if(abiertoAmbLocal==FALSE){
		if(!busqueda(tablaGlobal, $1.lexema)){
			error=-1;
			errorSemantico=1;
			yyerror($1.lexema);
			return -1;
		}

		if(getCategoria (tablaGlobal, $1.lexema) == FUNCION){
			error=-1;
			errorSemantico=0;
			yyerror("Asignacion incompatible.");
			return -1;
		}

		if(getClase (tablaGlobal, $1.lexema) != ESCALAR){
			error=-1;
			errorSemantico=0;
			yyerror("Asignacion incompatible.");
			return -1;
		}

		if(getTipo (tablaGlobal, $1.lexema) != $3.tipo){
			error=-1;
			errorSemantico=0;
			yyerror("Asignacion incompatible.");
			return -1;
		}

		asignar(yyout, $1.lexema, $3.es_direccion);
	} else{
		if(!busqueda(tablaLocal, $1.lexema)){
			if(!busqueda(tablaGlobal, $1.lexema)){
				error=-1;
				errorSemantico=1;
				yyerror($1.lexema);
				return -1;
			}

			if(getCategoria (tablaGlobal, $1.lexema) == FUNCION){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			if(getClase (tablaGlobal, $1.lexema) != ESCALAR){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			if(getTipo (tablaGlobal, $1.lexema) != $3.tipo){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			asignar(yyout, $1.lexema, $3.es_direccion);
		} else{
			if(getCategoria (tablaLocal, $1.lexema) == FUNCION){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			if(getClase (tablaLocal, $1.lexema) != ESCALAR){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			if(getTipo (tablaLocal, $1.lexema) != $3.tipo){
				error=-1;
				errorSemantico=0;
				yyerror("Asignacion incompatible.");
				return -1;
			}

			escribirVariableLocal(yyout, getPosVar(tablaLocal, $1.lexema));
			asignarDestinoEnPila(yyout, $3.es_direccion);
		}
	}

	fprintf(reglasSintactico,";R43:\t<asignacion> ::= <identificador> = <exp>\n");
}
	| elemento_vector TOK_ASIGNACION exp {
		if($3.tipo!=$1.tipo){
			error=-1;
			errorSemantico=0;
			yyerror("Asignacion incompatible.");
			return -1;
		}

		asignarElementoVector(yyout, $3.lexema, $3.es_direccion);

		fprintf(reglasSintactico,";R44:\t<asignacion> ::= <elemento_vector> = <exp>\n");
}
;

elemento_vector: TOK_IDENTIFICADOR TOK_CORCHETEIZQUIERDO exp TOK_CORCHETEDERECHO {
	if(abiertoAmbLocal==FALSE){
		if(!busqueda(tablaGlobal, $1.lexema)){
			error=-1;
			errorSemantico=1;
			yyerror($1.lexema);
			return -1;
		}

		if(getCategoria (tablaGlobal, $1.lexema) == FUNCION || getClase (tablaGlobal, $1.lexema) != VECTOR){
			error=-1;
			errorSemantico=0;
			yyerror("Intento de indexacion de una variable que no es de tipo vector.");
			return -1;
		}

		if($3.tipo != INT){
			error=-1;
			errorSemantico=0;
			yyerror("El indice en una operacion de indexacion tiene que ser de tipo entero.");
			return -1;
		}

		$$.tipo=getTipo (tablaGlobal, $1.lexema);
		$$.es_direccion=1;
		escribir_elemento_vector(yyout, $1.lexema, getTamano(tablaGlobal, $1.lexema), $3.es_direccion);

	} else{
			if(!busqueda(tablaLocal, $1.lexema)){
				if(!busqueda(tablaGlobal, $1.lexema)){
					error=-1;
					errorSemantico=1;
					yyerror($1.lexema);
					return -1;
				}

				if(getCategoria (tablaGlobal, $1.lexema) == FUNCION || getClase (tablaGlobal, $1.lexema) != VECTOR){
					error=-1;
					errorSemantico=1;
					yyerror("Intento de indexacion de una variable que no es de tipo vector.");
					return -1;
				}

				if($3.tipo != INT){
					error=-1;
					errorSemantico=0;
					yyerror("El indice en una operacion de indexacion tiene que ser de tipo entero.");
					return -1;
				}

				$$.tipo=getTipo (tablaGlobal, $1.lexema);
				$$.es_direccion=1;
				escribir_elemento_vector(yyout, $1.lexema, getTamano(tablaGlobal, $1.lexema), $3.es_direccion);
			 } else{
				if(getCategoria (tablaLocal, $1.lexema) == FUNCION || getClase (tablaLocal, $1.lexema) != VECTOR){
					error=-1;
					errorSemantico=0;
					yyerror("Intento de indexacion de una variable que no es de tipo vector.");
					return -1;
				}

				if($3.tipo != INT){
					error=-1;
					errorSemantico=0;
					yyerror("El indice en una operacion de indexacion tiene que ser de tipo entero.");
					return -1;
				}

				$$.tipo=getTipo (tablaLocal, $1.lexema);
				$$.es_direccion=1;
				escribir_elemento_vector(yyout, $1.lexema, getTamano(tablaLocal, $1.lexema), $3.es_direccion);
			}
	}

	fprintf(reglasSintactico,";R48:\t<elemento_vector> ::= <identificador> [ <exp> ]\n");
}
;

condicional: if_exp TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA sentencias TOK_LLAVEDERECHA {
	ifthen_fin(yyout, $1.etiqueta);
	fprintf(reglasSintactico,";R50:\t<condicional> ::= if ( <exp> ) { <sentencias> }\n");
}
	| if_exp_sentencias TOK_ELSE TOK_LLAVEIZQUIERDA sentencias  TOK_LLAVEDERECHA {
		ifthenelse_fin(yyout, $1.etiqueta);
		fprintf(reglasSintactico,";R51:\t<condicional> ::= if ( <exp> ) { <sentencias> } else { <sentencias> }\n");
	}
;

if_exp: TOK_IF TOK_PARENTESISIZQUIERDO exp {
	if($3.tipo!=BOOLEAN){
		error=-1;
		errorSemantico=0;
		yyerror("Condicional con condicion de tipo int.");
		return -1;
	}

	etiqueta++;
	$$.etiqueta=etiqueta; /*TODO: revisar esto porque la etiqueta++ estaba debajo de la herencia*/
	ifthen_inicio(yyout,$3.es_direccion,$$.etiqueta);
}
;

if_exp_sentencias : if_exp  TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA sentencias TOK_LLAVEDERECHA {
	$$.etiqueta=$1.etiqueta;
	ifthenelse_fin_then(yyout, $1.etiqueta);
}
;

bucle: while_exp sentencias TOK_LLAVEDERECHA {
	while_fin(yyout, $1.etiqueta);
	fprintf(reglasSintactico,";R52:\t<bucle> ::= while ( <exp> ) { <sentencias> }\n");
}
;

while: TOK_WHILE TOK_PARENTESISIZQUIERDO  {
	etiqueta++;
	$$.etiqueta=etiqueta; /*TODO: revisar esto porque la etiqueta++ estaba debajo de la herencia*/
	while_inicio(yyout, $$.etiqueta);
}
;

while_exp: while exp TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA {
	if($2.tipo!=BOOLEAN){
		error=-1;
		errorSemantico=0;
		yyerror("Bucle con condicion de tipo int.");
		return -1;
	}

	$$.etiqueta=$1.etiqueta;
	while_exp_pila(yyout, $2.es_direccion, $1.etiqueta);
}
;

lectura: TOK_SCANF TOK_IDENTIFICADOR {
	if(abiertoAmbLocal==FALSE){
		if(!busqueda(tablaGlobal, $2.lexema)){
			error=-1;
			errorSemantico=1;
			yyerror($2.lexema);
			return -1;
		}

		if(getCategoria (tablaGlobal, $2.lexema) == FUNCION){
			error=-1;
			errorSemantico=0;
			yyerror("La lectura no se puede guardar en una funcion.");
			return -1;
		}

		if(getClase (tablaGlobal, $2.lexema) != ESCALAR){
			error=-1;
			errorSemantico=0;
			yyerror("La lectura no se puede guardar en un vector.");
			return -1;
		}

		leer(yyout, $2.lexema, getTipo(tablaGlobal, $2.lexema));
	} else {
			if(!busqueda(tablaLocal, $2.lexema)){
				if(!busqueda(tablaGlobal, $2.lexema)){
					error=-1;
					errorSemantico=1;
					yyerror($2.lexema);
					return -1;
				}

				if(getCategoria (tablaGlobal, $2.lexema) == FUNCION){
					error=-1;
					errorSemantico=0;
					yyerror("La lectura no se puede guardar en una funcion.");
					return -1;
				}

				if(getClase (tablaGlobal, $2.lexema) != ESCALAR){
					error=-1;
					errorSemantico=0;
					yyerror("La lectura no se puede guardar en un vector.");
					return -1;
				}

				leer(yyout, $2.lexema, getTipo (tablaGlobal, $2.lexema));
			}else{
				if(getCategoria (tablaLocal, $2.lexema) == FUNCION){
					error=-1;
					errorSemantico=0;
					yyerror("La lectura no se puede guardar en una funcion.");
					return -1;
				}

				if(getClase (tablaLocal, $2.lexema) != ESCALAR){
					error=-1;
					errorSemantico=0;
					yyerror("La lectura no se puede guardar en un vector.");
					return -1;
				}

				leer(yyout, $2.lexema, getTipo(tablaLocal, $2.lexema));
		}
	}

	fprintf(reglasSintactico,";R54:\t<lectura> ::= scanf <identificador>\n");
}
;

escritura: TOK_PRINTF exp {
	escribir(yyout,$2.es_direccion,$2.tipo);
	fprintf(reglasSintactico,";fila %d\n",fila);
	fprintf(reglasSintactico,";R56:\t<escritura> ::= printf <exp>\n");
}
;

retorno_funcion: TOK_RETURN exp {
	if(en_funcion==FALSE){
		error=-1;
		errorSemantico=0;
		yyerror("Sentencia de retorno fuera del cuerpo de una función.");
		return -1;
	}if(getTipo (tablaGlobal, nombreFuncionActual) != $2.tipo){
		error=-1;
		errorSemantico=0;
		yyerror("Asignacion incompatible, la expresion de retorno tiene que tener el mismo tipo que la funcion.");
		return -1;
	}
	nretorno++;
	retornarFuncion(yyout, $2.es_direccion);
	fprintf(reglasSintactico,";R61:\t<retorno_funcion> ::= return <exp>\n");
}
;

exp: exp TOK_MAS exp {
	if($1.tipo == $3.tipo && $1.tipo == INT){
		$$.tipo=$1.tipo;
		$$.es_direccion=0;
		sumar(yyout, $1.es_direccion,$3.es_direccion);
	}else{
		error=-1;
		errorSemantico=0;
		yyerror("Operacion aritmetica con operandos boolean");
		return -1;
	}

	fprintf(reglasSintactico,";R72:\t<exp> ::= <exp> + <exp>\n");
}
	| exp TOK_MENOS exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=$1.tipo;
			$$.es_direccion=0;
			restar(yyout, $1.es_direccion,$3.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion aritmetica con operandos boolean");
			return -1;
		}

		fprintf(reglasSintactico,";R73:\t<exp> ::= <exp> - <exp>\n");
	}
	| exp TOK_DIVISION exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=$1.tipo;
			$$.es_direccion=0;
			dividir(yyout, $1.es_direccion,$3.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion aritmetica con operandos boolean");
			return -1;
		}

		fprintf(reglasSintactico,";R74:\t<exp> ::= <exp> / <exp>\n");
	}
	| exp TOK_ASTERISCO exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=$1.tipo;
			$$.es_direccion=0;
			multiplicar(yyout, $1.es_direccion,$3.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion aritmetica con operandos boolean");
			return -1;
		}

		fprintf(reglasSintactico,";R75:\t<exp> ::= <exp> * <exp>\n");
	}
	| TOK_MENOS exp %prec MENOSR {
		if($2.tipo == INT){
			$$.tipo=$2.tipo;
			$$.es_direccion=0;
			cambiar_signo(yyout,$2.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion aritmetica con operandos boolean");
			return -1;
		}

		fprintf(reglasSintactico,";R76:\t<exp> ::= - <exp>\n");
	}
	| exp TOK_AND exp {
		if($1.tipo==$3.tipo&&$1.tipo==BOOLEAN){
			$$.tipo=$1.tipo;
			$$.es_direccion=0;
			y(yyout,$1.es_direccion,$3.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion logica con operandos int.");
			return -1;
		}

		fprintf(reglasSintactico,";R77:\t<exp> ::= <exp> && <exp>\n");
	}
	| exp TOK_OR exp {
		if($1.tipo == $3.tipo && $1.tipo == BOOLEAN){
			$$.tipo=$1.tipo;
			$$.es_direccion=0;
			o(yyout,$1.es_direccion,$3.es_direccion);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion logica con operandos int.");
			return -1;
		}

		fprintf(reglasSintactico,";R78:\t<exp> ::= <exp> || <exp>\n");
	}
	| TOK_NOT exp {
		if($2.tipo == BOOLEAN){
			$$.tipo=$2.tipo;
			$$.es_direccion=0;
			etiqueta++;
			$$.etiqueta = etiqueta;
			no(yyout,$2.es_direccion, etiqueta);
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Operacion logica con operandos int.");
			return -1;
		}

		fprintf(reglasSintactico,";R79:\t<exp> ::= ! <exp>\n");
	}
	| TOK_IDENTIFICADOR {
		if(abiertoAmbLocal==FALSE){
			if(!busqueda(tablaGlobal, $1.lexema)){
				error=-1;
				errorSemantico=1;
				yyerror($1.lexema);
				return -1;
			}

			if(getCategoria (tablaGlobal, $1.lexema) == FUNCION){
				error=-1;
				errorSemantico=0;
				yyerror("Expresion identificador de categoria funcion.");
				return -1;
			}

			if(getClase (tablaGlobal, $1.lexema) != ESCALAR){
				error=-1;
				errorSemantico=0;

				if(en_explist==0){
					yyerror("Expresion identificador de clase vector.");
				}	else{
					yyerror("Parametro de funcion de tipo no escalar.");
				}

				return -1;
			}

			$$.tipo=getTipo (tablaGlobal, $1.lexema);
			$$.es_direccion=1;
			escribir_operando(yyout, $1.lexema, 1);

			if (en_explist == 1){
				printf("\nEntra");
				operandoEnPilaAArgumento(yyout, 1);
			}
		} else{
				if(!busqueda(tablaLocal, $1.lexema)){
					if(!busqueda(tablaGlobal, $1.lexema)){
						error=-1;
						errorSemantico=1;
						yyerror($1.lexema);
						return -1;
					}

					if(getCategoria (tablaGlobal, $1.lexema) == FUNCION){
						error=-1;
						errorSemantico=0;
						yyerror("Expresion identificador de categoria funcion.");
						return -1;
					}

					if(getClase (tablaGlobal, $1.lexema) != ESCALAR){
						error=-1;
						errorSemantico=0;
						if(en_explist==0)
							yyerror("Expresion identificador de clase vector.");
						else
							yyerror("Parametro de funcion de tipo no escalar.");
						return -1;
					}

					$$.tipo=getTipo (tablaGlobal, $1.lexema);
					$$.es_direccion=1;
					escribir_operando(yyout, $1.lexema, $1.es_direccion);

					if (en_explist == 1){
						printf("\nEntra");
						operandoEnPilaAArgumento(yyout, 1);
					}
			 } else{
					if(getCategoria (tablaLocal, $1.lexema) == FUNCION){
						error=-1;
						errorSemantico=0;
						yyerror("Expresion identificador de categoria funcion.");
						return -1;
					}

					if(getClase (tablaLocal, $1.lexema) != ESCALAR){
						error=-1;
						errorSemantico=0;
						yyerror("Expresion identificador de clase vector.");
						return -1;
					}

					$$.es_direccion=1;

					if (getCategoria(tablaLocal, $1.lexema) == PARAMETRO){
						escribirParametro(yyout, getPosParam(tablaLocal, $1.lexema), num_param_actual);

					} else {
						escribirVariableLocal(yyout, getPosVar(tablaLocal, $1.lexema));
					}

					$$.tipo=getTipo (tablaLocal, $1.lexema);
			}
		}

		fprintf(reglasSintactico,";R80:\t<exp> ::= <identificador>\n");}
	| constante {
		$$.tipo=$1.tipo;
		$$.es_direccion=$1.es_direccion;
		$$.valor_entero=$1.valor_entero;
		fprintf(reglasSintactico,";R81:\t<exp> ::= <constante>\n");
	}
	| TOK_PARENTESISIZQUIERDO exp TOK_PARENTESISDERECHO {
		$$.tipo=$2.tipo;
		$$.es_direccion=$2.es_direccion;
		$$.valor_entero=$2.valor_entero;
		fprintf(reglasSintactico,";R82:\t<exp> ::= ( <exp> )\n");
	}
	| TOK_PARENTESISIZQUIERDO comparacion TOK_PARENTESISDERECHO {
		$$.tipo=$2.tipo;
		$$.es_direccion=$2.es_direccion;
		$$.valor_entero=$2.valor_entero;
		fprintf(reglasSintactico,";R83:\t<exp> ::= ( <comparacion> )\n");
	}
	| elemento_vector {
		$$.tipo=$1.tipo;
		$$.es_direccion=$1.es_direccion;
		$$.valor_entero=$1.valor_entero;
		fprintf(reglasSintactico,";R85:\t<exp> ::= <elemento_vector>\n");
	}
	| idf_llamada_funcion TOK_PARENTESISIZQUIERDO lista_expresiones TOK_PARENTESISDERECHO {
		llamada_dentro_funcion=0;

		if(abiertoAmbLocal==FALSE){
			if(getNParams (tablaGlobal, $1.lexema) != num_parametros_llamada_actual){
				error=-1;
				errorSemantico=0;
				yyerror("Numero incorrecto de parametros en llamada a funcion.");
				return -1;
			}
		}	else{
			if(strcmp(nombreFuncionActual,$1.lexema)==0){
				if(getNParams (tablaLocal, $1.lexema) != num_parametros_llamada_actual){
					error=-1;
					errorSemantico=0;
					yyerror("Numero incorrecto de parametros en llamada a funcion.");
					return -1;
				}
			} else{
				if(getNParams (tablaGlobal, $1.lexema) != num_parametros_llamada_actual){
					error=-1;
					errorSemantico=0;
					yyerror("Numero incorrecto de parametros en llamada a funcion.");
					return -1;
				}
			}
		}

		en_explist=0;
		$$.tipo=getTipo (tablaGlobal, $1.lexema);
		$$.es_direccion=0;
		llamarFuncion(yyout, $1.lexema, num_param_actual);

		fprintf(reglasSintactico,";R88:\t<exp> ::= <identificador> ( <lista_expresiones> )\n");
}
;

idf_llamada_funcion: TOK_IDENTIFICADOR{
		llamada_dentro_funcion=0;

		if(!busqueda(tablaGlobal, $1.lexema)){
			error=-1;
			errorSemantico=1;
			yyerror($1.lexema);
			return -1;
		}

		if(getCategoria(tablaGlobal, $1.lexema) != FUNCION){
			error=-1;
			errorSemantico=0;
			yyerror("El identificador de llamada de funcion no es de categoria funcion.");
			return -1;
		}

		if(en_explist==1){
			error=-1;
			errorSemantico=0;
			yyerror("No esta permitido el uso de llamadas a funciones como parametros de otras funciones.");
			return -1;
		}

		if(strcmp(nombreFuncionActual,$1.lexema)!=0){
			llamada_dentro_funcion=1;
			printf("\nLlamada dentro de funcion activada");
		}

		en_explist=1;
		num_parametros_llamada_actual=0;
		strcpy($$.lexema,$1.lexema);
 }
;

lista_expresiones: exp resto_lista_expresiones {
	num_parametros_llamada_actual++;
	fprintf(reglasSintactico,";R89:\t<lista_expresiones> ::= <exp> <resto_lista_expresiones>\n");
}
		| /* VACIO */ {fprintf(reglasSintactico,";R90:\t<lista_expresiones> ::=\n");}
;

resto_lista_expresiones: TOK_COMA exp resto_lista_expresiones {
	num_parametros_llamada_actual++;
	fprintf(reglasSintactico,";R91:\t<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>\n");
}
			| /* VACIO */ {fprintf(reglasSintactico,";R92:\t<resto_lista_expresiones> ::=\n");}
;

comparacion: exp TOK_IGUAL exp {
	if($1.tipo == $3.tipo && $1.tipo == INT){
		$$.tipo=BOOLEAN;
		$$.es_direccion=0;
		igual(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
		etiqueta++;
	}else{
		error=-1;
		errorSemantico=0;
		yyerror("Comparacion con operandos boolean.");
		return -1;
	}
	fprintf(reglasSintactico,";R93:\t<comparacion> ::= <exp> == <exp>\n");
}
	| exp TOK_DISTINTO exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=BOOLEAN;
			$$.es_direccion=0;
			distinto(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
			etiqueta++;
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Comparacion con operandos boolean.");
			return -1;
		}
		fprintf(reglasSintactico,";R94:\t<comparacion> ::= <exp> != <exp>\n");
	}
	| exp TOK_MENORIGUAL exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=BOOLEAN;
			$$.es_direccion=0;
			menor_igual(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
			etiqueta++;
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Comparacion con operandos boolean.");
			return -1;
		}
		fprintf(reglasSintactico,";R95:\t<comparacion> ::= <exp> <= <exp>\n");
	}
	| exp TOK_MAYORIGUAL exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=BOOLEAN;
			$$.es_direccion=0;
			mayor_igual(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
			etiqueta++;
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Comparacion con operandos boolean.");
			return -1;
		}
		fprintf(reglasSintactico,";R96:\t<comparacion> ::= <exp> >= <exp>\n");
	}
	| exp TOK_MENOR exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=BOOLEAN;
			$$.es_direccion=0;
			menor(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
			etiqueta++;
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Comparacion con operandos boolean.");return -1;
		}
		fprintf(reglasSintactico,";R97:\t<comparacion> ::= <exp> < <exp>\n");
	}
	| exp TOK_MAYOR exp {
		if($1.tipo == $3.tipo && $1.tipo == INT){
			$$.tipo=BOOLEAN;
			$$.es_direccion=0;
			mayor(yyout, $1.es_direccion, $3.es_direccion, etiqueta);
			etiqueta++;
		}else{
			error=-1;
			errorSemantico=0;
			yyerror("Comparacion con operandos boolean.");
			return -1;
		}
		fprintf(reglasSintactico,";R98:\t<comparacion> ::= <exp> > <exp>\n");
	}
;

constante: constante_logica {
	$$.es_direccion=$1.es_direccion;
	$$.tipo=$1.tipo;
	fprintf(reglasSintactico,";R99:\t<constante> ::= <constante_logica>\n");
}
	| constante_entera {
		$$.tipo=$1.tipo;
		$$.es_direccion=$1.es_direccion;
		$$.valor_entero=$1.valor_entero;
		fprintf(reglasSintactico,";R100:\t<constante> ::= <constante_entera>\n");
	}
;

constante_logica: TOK_TRUE {
	$$.es_direccion=0;
	$$.tipo=BOOLEAN;
	escribir_operando(yyout, "1", 0);
	fprintf(reglasSintactico,";R102:\t<constante_logica> ::= true\n");
}
		| TOK_FALSE {
			$$.es_direccion=0;
			$$.tipo=BOOLEAN;
			escribir_operando(yyout, "0", 0);
			fprintf(reglasSintactico,";R103:\t<constante_logica> ::= false\n");
		}
;

constante_entera: TOK_CONSTANTE_ENTERA {
	$$.tipo=INT;
	$$.es_direccion=0;
	sprintf(aux, "%d", $1.valor_entero);

	/*Solo escribimos si no estamos declarando un vector, lo cual implica que estamos en .bss y no queremos imprimir nada de esto ahi*/
	if (clase_actual != VECTOR){
		escribir_operando(yyout, aux, 0);
	}

	fprintf(reglasSintactico,";R104:\t<constante_entera> ::= TOK_CONSTANTE_ENTERA\n");
}
;

idpf: TOK_IDENTIFICADOR {
	if(insercion(tablaLocal, $1.lexema, PARAMETRO, clase_actual, tipo_actual, tamanio_vector_actual, num_variables_locales_actual, pos_variable_local_actual,num_param_actual ,pos_param_actual) == FALSE){
		error=-1;
		errorSemantico=1;
		yyerror($1.lexema);
		return -1;
	}

	strcpy($$.lexema,$1.lexema);
	fprintf(reglasSintactico,";R108:\t<identificador> ::= TOK_IDENTIFICADOR\n");
	num_param_actual++;
	pos_param_actual++;
}
;

identificador: TOK_IDENTIFICADOR {
	if(error==-1){
		errorSemantico=2;
		yyerror($1.lexema);
		return -1;
	}

	if(abiertoAmbLocal==FALSE){
		if(insercion(tablaGlobal, $1.lexema, VARIABLE, clase_actual, tipo_actual, tamanio_vector_actual, num_variables_locales_actual, pos_variable_local_actual,num_param_actual ,pos_param_actual) == FALSE){
				error=-1;
				errorSemantico=0;
				yyerror("Declaracion duplicada.");
				return -1;
		}

		if (clase_actual == VECTOR){
			declarar_variable(yyout, $1.lexema, tipo_actual, tamanio_vector_actual);
		} else {
			declarar_variable(yyout, $1.lexema, tipo_actual, 1);
		}

  } else{
		if (!busqueda(tablaLocal, $1.lexema)){
			if(clase_actual == ESCALAR){
				insercion(tablaLocal, $1.lexema,VARIABLE,clase_actual,tipo_actual, tamanio_vector_actual, num_variables_locales_actual, pos_variable_local_actual,num_param_actual ,pos_param_actual);
				pos_variable_local_actual++;
				num_variables_locales_actual++;
			} else{
				error=-1;
				errorSemantico=0;
				yyerror("Variable local de tipo no escalar.");
				return -1;
			}
		} else{
				error=-1;
				errorSemantico=0;
				yyerror("Declaracion duplicada.");
				return -1;
		}
 }
  strcpy($$.lexema,$1.lexema);
	fprintf(reglasSintactico,";R108:\t<identificador> ::= TOK_IDENTIFICADOR\n");
 }
;
%%

void yyerror(char const *cadena){
	if(error == -1){
		switch (errorSemantico){
			case 0:
				fprintf(stderr, "****Error semantico en lin %d: %s\n", fila, cadena);
				break;

			case 1:
				fprintf(stderr, "****Error semantico en lin %d: Acceso a variable no declarada (%s).\n", fila, cadena);
				break;

			case 2:
				fprintf(stderr, "****Error semantico en lin %d: El tamanyo del vector <%s> excede los limites permitidos (1,64).\n", fila, cadena);
				break;

			default:
				fprintf(stderr, "****Error semantico en lin %d: Funcion <%s> sin sentencia de retorno.\n", fila, cadena);
				break;
		}
	}	else if(error == 0){
			fprintf(stderr, "**** Error sintactico en [lin %d, col %d]\n", fila, columna-yyleng);
	}

	error = 0;

	destruirTablaHash(tablaGlobal);

	if(reglasSintactico!=NULL)
		fclose(reglasSintactico);

	if(abiertoAmbLocal==TRUE)
		destruirTablaHash(tablaLocal);
}
