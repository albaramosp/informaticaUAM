/*Programa que establece las reglas para detectar y tratar simbolos ALFA*/
/*Incorpora correcciones a la implementacion de la practica anterior.*/
/*Permite enlazar con un programa bison.*/
/*Alba Ramos Pedroviejo*/
/*Andrea Salcedo Lopez*/

%{
	#include "alfa.h"
	#include "y.tab.h"

	int fila=1,columna=1;
	int error=0;

	extern FILE *reglasSintactico;
%}

%option noyywrap

%%

main	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MAIN;}
int	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_INT;}
boolean	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_BOOLEAN;}
function	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_FUNCTION;}
array	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_ARRAY;}
if	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_IF;}
else	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_ELSE;}
while	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_WHILE;}
scanf	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_SCANF;}
printf	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_PRINTF;}
return	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_RETURN;}

;	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_PUNTOYCOMA;}
,	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_COMA;}
[{]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_LLAVEIZQUIERDA;}
[}]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_LLAVEDERECHA;}
[(]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_PARENTESISIZQUIERDO;}
[)]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_PARENTESISDERECHO;}
[[]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_CORCHETEIZQUIERDO;}
[]]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_CORCHETEDERECHO;}
[+]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MAS;}
[-]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MENOS;}
[/]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_DIVISION;}
[*]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_ASTERISCO;}
&&	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_AND;}
"||"	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_OR;}
[=]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_ASIGNACION;}
"!="	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_DISTINTO;}
"=="	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_IGUAL;}
[!]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_NOT;}
"<="	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MENORIGUAL;}
">="	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MAYORIGUAL;}
[>]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MAYOR;}
[<]	{columna+=yyleng; fprintf(reglasSintactico,";D:\t%s\n",yytext); return TOK_MENOR;}

true	{columna+=yyleng; fprintf(reglasSintactico, ";D:\t%s\n", yytext); return TOK_TRUE;}
false	{columna+=yyleng; fprintf(reglasSintactico, ";D:\t%s\n", yytext); return TOK_FALSE;}

[\r\n]	{fila++; columna=1;}

"//"[^\n]*	{columna+=yyleng;}

[" "\t]+	{columna+=yyleng;}

[a-zA-Z][a-zA-Z0-9]*	{
	columna+=yyleng;
	strcpy(yylval.atributos.lexema, yytext);

	if(yyleng<=100){
		fprintf(reglasSintactico, ";D:\t%s\n", yytext);
	} else{
		fprintf(stderr, "****Error en [lin %d, col %d]: identificador demasiado largo (%s)\n", fila, columna-yyleng, yytext);
		error=1;
		return TOK_ERROR;
	};

	return TOK_IDENTIFICADOR;
}

[0-9]+	{columna+=yyleng; yylval.atributos.valor_entero=atoi(yytext); fprintf(reglasSintactico, ";D:\t%s\n", yytext); return TOK_CONSTANTE_ENTERA;}

.	{
	error=1;
	columna+=yyleng;
	fprintf(stderr, "****Error en [lin %d, col %d]: simbolo no permitido (%s)\n", fila, columna-yyleng, yytext);
	return TOK_ERROR;
}
%%
