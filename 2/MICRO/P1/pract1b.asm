;**************************************************************************
; 									SBM 
; Pareja numero: 
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************

;_______________________________________________________________
; DEFINICION DEL SEGMENTO DE DATOS
DATOS SEGMENT

CONTADOR db ?
TOME dw 0CAFEH
TABLA100 db 100 dup (?)
ERROR1 db "Atención: Entrada de datos incorrecta"

DATOS ENDS

;_______________________________________________________________
; DEFINICION DEL SEGMENTO DE PILA
PILA SEGMENT STACK "STACK"
	DB 40H DUP (0)
PILA ENDS

;_______________________________________________________________
; DEFINICION DEL SEGMENTO DE CODIGO
CODE SEGMENT
ASSUME CS:CODE,DS:DATOS,SS:PILA

; COMIENZO DEL PROCEDIMIENTO PRINCIPAL (START)
START PROC FAR

	;movemos a AX DATOS y luego esto a DS
	MOV AX, DATOS 
	MOV DS, AX

	mov AL, ERROR1[2] ;la cadena ERROR1 es un array de caracteres, por eso el tercero esta en la posicion 2 del array 
	;'e' estara en la posicion 99d de la tabla, justo antes del caracter 'A' de ERROR1
	
	;el segmento de datos usa 3 bytes (CONTADOR y TOME) antes de empezar la tabla
	
	;en el depurador, podemos ir a la posicion ds:66h y en ella estara el caracter 'e'
	mov tabla100[63H], AL 

	;copiamos en formato Little Endian
	
	;estaran en las posiciones 35 y 36 de la tabla
	
	; en el depurador, podemos ir a la posicion ds:26 y ds:27 para comprobar los cambios
	mov dx, TOME
	mov tabla100[23H], dl ;FE
	mov tabla100[24H], dh ;CA

	mov CONTADOR, dh;el byte mas significativo de TOME (CA) esta en DH
	
	; FIN DEL PROGRAMA
	MOV AX, 4C00H
	INT 21H 


;FIN DE PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION
START ENDP

;FIN DEL SEGMENTO DE CODIGO
CODE ENDS

;FIN DE PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION
END START