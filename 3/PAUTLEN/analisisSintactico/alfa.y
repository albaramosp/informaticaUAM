/*Programa que establece las reglas para detectar y tratar simbolos ALFA*/
/*Permite que Bison genere un analizador sintactico valido para este lenguaje*/
/*Alba Ramos Pedroviejo*/
/*Andrea Salcedo Lopez*/

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	void yyerror(char *s);
	extern int yylex();

	extern FILE *yyin;
	extern FILE *yyout;
	extern int yyleng;
	extern int fila, columna, errorMorfo;
%}

/*Definicion del tipo de la variable yylval*/
%union{
	char cadena[1000];
	int numero;
}

/*Tokens, es decir, simbolos terminales, que vienen de alfa.l asociados a su tipo de yylval*/
%token <cadena> TOK_MAIN
%token <cadena> TOK_INT
%token <cadena> TOK_BOOLEAN
%token <cadena> TOK_ARRAY
%token <cadena> TOK_FUNCTION
%token <cadena> TOK_IF
%token <cadena> TOK_ELSE
%token <cadena> TOK_WHILE
%token <cadena> TOK_SCANF
%token <cadena> TOK_PRINTF
%token <cadena> TOK_RETURN

%token <cadena> TOK_PUNTOYCOMA
%token <cadena> TOK_COMA
%token <cadena> TOK_PARENTESISIZQUIERDO
%token <cadena> TOK_PARENTESISDERECHO
%token <cadena> TOK_CORCHETEIZQUIERDO
%token <cadena> TOK_CORCHETEDERECHO
%token <cadena> TOK_LLAVEIZQUIERDA
%token <cadena> TOK_LLAVEDERECHA
%token <cadena> TOK_ASIGNACION
%token <cadena> TOK_MAS
%token <cadena> TOK_MENOS
%token <cadena> TOK_DIVISION
%token <cadena> TOK_ASTERISCO
%token <cadena> TOK_AND
%token <cadena> TOK_OR
%token <cadena> TOK_NOT
%token <cadena> TOK_IGUAL
%token <cadena> TOK_DISTINTO
%token <cadena> TOK_MENORIGUAL
%token <cadena> TOK_MAYORIGUAL
%token <cadena> TOK_MENOR
%token <cadena> TOK_MAYOR

%token TOK_IDENTIFICADOR

%token <numero> TOK_CONSTANTE_ENTERA
%token <cadena> TOK_TRUE
%token <cadena> TOK_FALSE

%token TOK_ERROR

/*Precedencia de operadores: los que estan en la misma linea tienen misma preferencia*/
/*Los que se declaran antes indica que tienen menor preferencia que los de debajo*/
%left TOK_MAS TOK_MENOS TOK_OR
%left TOK_ASTERISCO TOK_DIVISION TOK_AND
%left TOK_NOT

/*Declaracion del axioma de la gramatica*/
%start programa

/*Regals de la gramatica*/
/*Los numeros de regla que faltan son aquellos que no se contemplan en la practica este curso*/
%%
programa: TOK_MAIN TOK_LLAVEIZQUIERDA declaraciones funciones sentencias TOK_LLAVEDERECHA {fprintf(yyout, ";R1:\t<programa> ::= main { <declaraciones> <funciones> <sentencias> }\n");}
;

declaraciones: declaracion {fprintf(yyout, ";R2:\t<declaraciones> ::= <declaracion>\n");}
		| declaracion declaraciones {fprintf(yyout, ";R3:\t<declaraciones> ::= <declaracion> <declaraciones>\n");}
;

declaracion: clase identificadores TOK_PUNTOYCOMA {fprintf(yyout, ";R4:\t<declaracion> ::= <clase> <identificadores> ;\n");}
;

clase: clase_escalar {fprintf(yyout, ";R5:\t<clase> ::= <clase_escalar>\n");}
	| clase_vector {fprintf(yyout, ";R7:\t<clase> ::= <clase_vector>\n");}
;

clase_escalar:tipo {fprintf(yyout, ";R9:\t<clase_escalar> ::= <tipo>\n");}
;

clase_vector: TOK_ARRAY tipo TOK_CORCHETEIZQUIERDO constante_entera TOK_CORCHETEDERECHO {fprintf(yyout, ";R15:\t<clase_vector> ::= array <tipo> [ <constante_entera> ]\n");}
;

tipo: TOK_INT {fprintf(yyout, ";R10:\t<tipo> ::= int\n");}
	|TOK_BOOLEAN {fprintf(yyout, ";R11:\t<tipo> ::= boolean\n");}
;

identificadores: identificador {fprintf(yyout, ";R18:\t<identificadores> ::= <identificador>\n");}
		| identificador TOK_COMA identificadores {fprintf(yyout, ";R19:\t<identificadores> ::= <identificador> , <identificadores>\n");}
;

funciones: funcion funciones {fprintf(yyout, ";R20:\t<funciones> ::= <funcion> <funciones>\n");}
	| /* lambda */ {fprintf(yyout, ";R21:\t<funciones> ::=\n");}
;

funcion: TOK_FUNCTION tipo identificador TOK_PARENTESISIZQUIERDO parametros_funcion TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA declaraciones_funcion sentencias TOK_LLAVEDERECHA {fprintf(yyout, ";R22:\t<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }\n");}
;

parametros_funcion: parametro_funcion resto_parametros_funcion {fprintf(yyout, ";R23:\t<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>\n");}
			| /* lambda */ {fprintf(yyout, ";R24:\t<parametros_funcion> :=\n");}
;

resto_parametros_funcion: TOK_PUNTOYCOMA parametro_funcion resto_parametros_funcion {fprintf(yyout, ";R25:\t<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>\n");}
			| /* lambda */ {fprintf(yyout, ";R26:\t<resto_parametros_funcion> ::=\n");}
;

parametro_funcion: tipo identificador {fprintf(yyout, ";R27:\t<parametro_funcion> ::= <tipo> <identificador>\n");}
;

declaraciones_funcion: declaraciones {fprintf(yyout, ";R28:\t<declaraciones_funcion> ::= <declaraciones>\n");}
			| /* lambda */ {fprintf(yyout, ";R29:\t<declaraciones_funcion> ::=\n");}
;

sentencias: sentencia {fprintf(yyout, ";R30:\t<sentencias> ::= <sentencia>\n");}
	| sentencia sentencias {fprintf(yyout, ";R31:\t<sentencias> ::= <sentencia> <sentencias>\n");}
;

sentencia: sentencia_simple TOK_PUNTOYCOMA {fprintf(yyout, ";R32:\t<sentencia> ::= <sentencia_simple> ;\n");}
	| bloque {fprintf(yyout, ";R33:\t<sentencia> ::= <bloque>\n");}
;

sentencia_simple: asignacion {fprintf(yyout, ";R34:\t<sentencia_simple> ::= <asignacion>\n");}
		| lectura {fprintf(yyout, ";R35:\t<sentencia_simple> ::= <lectura>\n");}
		| escritura {fprintf(yyout, ";R36:\t<sentencia_simple> ::= <escritura>\n");}
		| retorno_funcion {fprintf(yyout, ";R38:\t<sentencia_simple> ::= <retorno_funcion>\n");}
;

bloque: condicional {fprintf(yyout, ";R40:\t<bloque> ::= <condicional>\n");}
	| bucle {fprintf(yyout, ";R41:\t<bloque> ::= <bucle>\n");}
;

asignacion: identificador TOK_ASIGNACION exp {fprintf(yyout, ";R43:\t<asignacion> ::= <identificador> = <exp>\n");}
	| elemento_vector TOK_ASIGNACION exp {fprintf(yyout, ";R44:\t<asignacion> ::= <elemento_vector> = <exp>\n");}
;

elemento_vector: identificador TOK_CORCHETEIZQUIERDO exp TOK_CORCHETEDERECHO {fprintf(yyout, ";R48:\t<elemento_vector> ::= <identificador> [ <exp> ]\n");}
;

condicional: TOK_IF TOK_PARENTESISIZQUIERDO exp TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA sentencias TOK_LLAVEDERECHA {fprintf(yyout, ";R50:\t<condicional> ::= if ( <exp> ) { <sentencias> }\n");}
	|  TOK_IF TOK_PARENTESISIZQUIERDO exp TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA sentencias TOK_LLAVEDERECHA TOK_ELSE TOK_LLAVEIZQUIERDA sentencias  TOK_LLAVEDERECHA {fprintf(yyout, ";R51:\t<condicional> ::= if ( <exp> ) { <sentencias> } else { <sentencias> }\n");}
;

bucle: TOK_WHILE TOK_PARENTESISIZQUIERDO exp TOK_PARENTESISDERECHO TOK_LLAVEIZQUIERDA sentencias TOK_LLAVEDERECHA {fprintf(yyout, ";R52:\t<bucle> ::= while ( <exp> ) { <sentencias> }\n");}
;

lectura: TOK_SCANF identificador {fprintf(yyout, ";R54:\t<lectura> ::= scanf <identificador>\n");}
;

escritura: TOK_PRINTF exp {fprintf(yyout, ";R56:\t<escritura> ::= printf <exp>\n");}
;
retorno_funcion: TOK_RETURN exp {fprintf(yyout, ";R61:\t<retorno_funcion> ::= return <exp>\n");}
;

exp: exp TOK_MAS exp {fprintf(yyout, ";R72:\t<exp> ::= <exp> + <exp>\n");}
	| exp TOK_MENOS exp {fprintf(yyout, ";R73:\t<exp> ::= <exp> - <exp>\n");}
	| exp TOK_DIVISION exp {fprintf(yyout, ";R74:\t<exp> ::= <exp> / <exp>\n");}
	| exp TOK_ASTERISCO exp {fprintf(yyout, ";R75:\t<exp> ::= <exp> * <exp>\n");}
	| TOK_MENOS exp {fprintf(yyout, ";R76:\t<exp> ::= - <exp>\n");}
	| exp TOK_AND exp {fprintf(yyout, ";R77:\t<exp> ::= <exp> && <exp>\n");}
	| exp TOK_OR exp {fprintf(yyout, ";R78:\t<exp> ::= <exp> || <exp>\n");}
	| TOK_NOT exp {fprintf(yyout, ";R79:\t<exp> ::= ! <exp>\n");}
	| identificador {fprintf(yyout, ";R80:\t<exp> ::= <identificador>\n");}
	| constante {fprintf(yyout, ";R81:\t<exp> ::= <constante>\n");}
	| TOK_PARENTESISIZQUIERDO exp TOK_PARENTESISDERECHO {fprintf(yyout, ";R82:\t<exp> ::= ( <exp> )\n");}
	| TOK_PARENTESISIZQUIERDO comparacion TOK_PARENTESISDERECHO {fprintf(yyout, ";R83:\t<exp> ::= ( <comparacion> )\n");}
	| elemento_vector {fprintf(yyout, ";R85:\t<exp> ::= <elemento_vector>\n");}
	| identificador TOK_PARENTESISIZQUIERDO lista_expresiones TOK_PARENTESISDERECHO {fprintf(yyout, ";R88:\t<exp> ::= <identificador> ( <lista_expresiones> )\n");}
;

lista_expresiones: exp resto_lista_expresiones {fprintf(yyout, ";R89:\t<lista_expresiones> ::= <exp> <resto_lista_expresiones>\n");}
		| /* lambda */ {fprintf(yyout, ";R90:\t<lista_expresiones> ::=\n");}
;

resto_lista_expresiones: TOK_COMA exp resto_lista_expresiones {fprintf(yyout, ";R91:\t<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>\n");}
			| /* lambda */ {fprintf(yyout, ";R92:\t<resto_lista_expresiones> ::=\n");}
;

comparacion: exp TOK_IGUAL exp {fprintf(yyout, ";R93:\t<comparacion> ::= <exp> == <exp>\n");}
	| exp TOK_DISTINTO exp {fprintf(yyout, ";R94:\t<comparacion> ::= <exp> != <exp>\n");}
	| exp TOK_MENORIGUAL exp {fprintf(yyout, ";R95:\t<comparacion> ::= <exp> <= <exp>\n");}
	| exp TOK_MAYORIGUAL exp {fprintf(yyout, ";R96:\t<comparacion> ::= <exp> >= <exp>\n");}
	| exp TOK_MENOR exp {fprintf(yyout, ";R97:\t<comparacion> ::= <exp> < <exp>\n");}
	| exp TOK_MAYOR exp {fprintf(yyout, ";R98:\t<comparacion> ::= <exp> > <exp>\n");}
;

constante: constante_logica {fprintf(yyout, ";R99:\t<constante> ::= <constante_logica>\n");}
	| constante_entera {fprintf(yyout, ";R100:\t<constante> ::= <constante_entera>\n");}
;

constante_logica: TOK_TRUE {fprintf(yyout, ";R102:\t<constante_logica> ::= true\n");}
		| TOK_FALSE {fprintf(yyout, ";R103:\t<constante_logica> ::= false\n");}
;

constante_entera: TOK_CONSTANTE_ENTERA {fprintf(yyout, ";R104:\t<constante_entera> ::= TOK_CONSTANTE_ENTERA\n");}
;

identificador: TOK_IDENTIFICADOR {fprintf(yyout, ";R108:\t<identificador> ::= TOK_IDENTIFICADOR\n");}
;
%%

void yyerror (char *s){
	if(errorMorfo == 0){
		fprintf(stderr,"**** Error sintáctico en [lin %d, col %d]\n", fila, columna-yyleng);
	}

	errorMorfo = 0;
}
