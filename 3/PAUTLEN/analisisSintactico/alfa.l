/*Programa que establece las reglas para detectar y tratar simbolos ALFA*/
/*Incorpora correcciones a la implementacion de la practica anterior.*/
/*Permite enlazar con un programa bison.*/
/*Alba Ramos Pedroviejo*/
/*Andrea Salcedo Lopez*/

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"

	int fila=1,columna=1;

	/*Si es 0, no ha habido error morfologico y permite mostrar los sintacticos*/
	/*Si es 1, no debe mostrar errores sintacticos porque ya ha habido morfologicos*/
	int errorMorfo = 0;

%}

%option noyywrap

%%
[0-9.%&¿?][a-zA-Z0-9]+	{
	columna+=yyleng;
	strcpy(yylval.cadena, yytext);
	errorMorfo=1;
	fprintf(stderr, "****Error en [lin %d, col %d]: simbolo no permitido (%s)\n", fila, columna-yyleng, yytext);
	return TOK_ERROR;
}

main	{columna+=yyleng; strcpy(yylval.cadena ,yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MAIN;}
int	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_INT;}
boolean	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_BOOLEAN;}
function	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_FUNCTION;}
array	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_ARRAY;}
if	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_IF;}
else	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_ELSE;}
while	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_WHILE;}
scanf	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_SCANF;}
printf	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_PRINTF;}
return	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_RETURN;}

;	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_PUNTOYCOMA;}
,	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_COMA;}
[{]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_LLAVEIZQUIERDA;}
[}]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_LLAVEDERECHA;}
[(]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_PARENTESISIZQUIERDO;}
[)]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_PARENTESISDERECHO;}
[[]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_CORCHETEIZQUIERDO;}
[]]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_CORCHETEDERECHO;}
[+]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MAS;}
[-]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MENOS;}
[/]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_DIVISION;}
[*]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_ASTERISCO;}
&&	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_AND;}
[||]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_OR;}
[=]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_ASIGNACION;}
"!="	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_DISTINTO;}
"=="	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_IGUAL;}
[!]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_NOT;}
"<="	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MENORIGUAL;}
">="	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MAYORIGUAL;}
[>]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MAYOR;}
[<]	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout,";D:\t%s\n",yytext); return TOK_MENOR;}

true	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout, ";D:\t%s\n", yytext); return TOK_TRUE;}
false	{columna+=yyleng; strcpy(yylval.cadena, yytext); fprintf(yyout, ";D:\t%s\n", yytext); return TOK_FALSE;}

[\r\n]	{fila++; columna=1;}

"//"[^\n]*	{columna+=yyleng;}

[" "\t]+	{columna+=yyleng;}

[a-zA-Z][a-zA-Z0-9]*	{
	columna+=yyleng;
	strcpy(yylval.cadena,yytext);

	if(yyleng<=100){
		fprintf(yyout, ";D:\t%s\n", yytext);
	} else{
		fprintf(stderr, "****Error en [lin %d, col %d]: identificador demasiado largo (%s)\n", fila, columna-yyleng, yytext);
		errorMorfo=1;
		return TOK_ERROR;
	};

	return TOK_IDENTIFICADOR;
}

[0-9]+	{columna+=yyleng; yylval.numero=atoi(yytext); fprintf(yyout, ";D:\t%s\n", yytext); return TOK_CONSTANTE_ENTERA;}

.	{
	errorMorfo=1;
	columna+=yyleng;
	strcpy(yylval.cadena,yytext);
	fprintf(stderr, "****Error en [lin %d, col %d]: simbolo no permitido (%s)\n", fila, columna-yyleng, yytext);
	return TOK_ERROR;
}
%%
