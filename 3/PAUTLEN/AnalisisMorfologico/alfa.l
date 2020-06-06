/*Programa que establece las reglas para detectar y tratar simbolos ALFA*/
/*Incorpora el token ENTER que permite detectar saltos de linea exclusivamente*/
/*Alba Ramos Pedroviejo*/
/*Andrea Salcedo Lopez*/

%{
  #include "tokens.h"

  #define ENTER -2
%}

%option noyywrap

%%
[0-9.%&¿?][a-zA-Z0-9]+  { return TOK_ERROR; }

main { return TOK_MAIN; }
int { return TOK_INT; }
boolean { return TOK_BOOLEAN; }
function { return TOK_FUNCTION; }
array { return TOK_ARRAY; }
if { return TOK_IF; }
else { return TOK_ELSE; }
while { return TOK_WHILE; }
scanf { return TOK_SCANF; }
printf { return TOK_PRINTF; }
return { return TOK_RETURN; }

; { return TOK_PUNTOYCOMA; }
, { return TOK_COMA; }
[{] { return TOK_LLAVEIZQUIERDA; }
[}] { return TOK_LLAVEDERECHA; }
[(] { return TOK_PARENTESISIZQUIERDO; }
[)] { return TOK_PARENTESISDERECHO; }
[[] { return TOK_CORCHETEIZQUIERDO; }
[]] { return TOK_CORCHETEDERECHO; }
[+] { return TOK_MAS; }
[-] { return TOK_MENOS; }
[/] { return TOK_DIVISION; }
[*] { return TOK_ASTERISCO; }
&& { return TOK_AND; }
[||] { return TOK_OR; }
[=] { return TOK_ASIGNACION; }
"!=" { return TOK_DISTINTO; }
"==" { return TOK_IGUAL; }
[!] { return TOK_NOT; }
"<=" {return TOK_MENORIGUAL; }
">=" { return TOK_MAYORIGUAL; }
[<] { return TOK_MENOR; }
[>] { return TOK_MAYOR; }

true { return TOK_TRUE; }
false { return TOK_FALSE; }

[\r\n] { return ENTER; }

"//"[^\n]*

[" "\t]+

[a-zA-Z0-9]{100,} { return TOK_ERROR; }
[a-zA-Z][a-zA-Z0-9]* { return TOK_IDENTIFICADOR; }

[0-9]+ { return TOK_CONSTANTE_ENTERA; }

[^.*$] { return TOK_ERROR; }

%%
