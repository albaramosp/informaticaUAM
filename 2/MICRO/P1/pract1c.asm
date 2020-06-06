;**************************************************************************
; 									SBM 
; Pareja numero: 
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************

;_______________________________________________________________
; DEFINICION DEL SEGMENTO DE DATOS
DATOS SEGMENT
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

	;Ponemos los datos iniciales:
	MOV AX, 0535H
	MOV DS, AX ;DS = 0535H

	MOV AX, 0210H
	MOV BX, AX ;BX = 0210H

	MOV AX, 1011H
	MOV DI, AX; DI = 1011H

	;EL PRIMERO MUEVE LO QUE HAYA EN LA POSICION DE MEMORIA DS:[1234H] A AL
	
	;DS:[1234H] DEBERIA SER IGUAL A 5350H + 1234H = 6584H
	
	;si, con el depurador, vamos con goto a la posicion 0:6584, observaremos que, efectivamente, contiene BL (13h)
	MOV BL, 13H
	MOV DS:[1234H], BL
	MOV AL, DS:[1234H] ;AL DEBERIA SER IGUAL A 13

	;Reseteamos BX con su valor original
	MOV BX, 0210H 

	;EL SEGUNDO MUEVE LO QUE HAYA EN LA POSICION DE MEMORIA DS:[BX] A AX. POR TANTO AH = DS[BX]+1 AL = DS:[BX]
	
	;DS[BX] DEBERIA SER IGUAL A 5350H + 0210H = 5560H
	
	;si, con el depurador, vamos con goto a la posicion 0:5560, observaremos que contiene CD, y 0:5561 contiene AB
	
	;se guarda en formato Little Endian
	MOV DS:[BX], 0ABCDH
	MOV AX, [BX] ; AX = ABCD

	;EL TERCERO MUEVE LO QUE HAYA EN AL A LA POSICION DE MEMORIA DS:[DI]
	
	;DS;[DI] DEBERIA SER IGUAL A 5350H + 1011H = 6361H
	
	;si, con el depurador, vamos con goto a la posicion 0:6361, observaremos que contiene AL (12h)
	MOV AL, 12H
	MOV [DI], AL ;DS:[DI] = 12
	
	; FIN DEL PROGRAMA
	MOV AX, 4C00H
	INT 21H 

START ENDP

;FIN DEL SEGMENTO DE CODIGO
CODE ENDS

;FIN DE PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION
END START
