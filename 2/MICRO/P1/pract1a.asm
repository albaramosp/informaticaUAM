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

	mov ax, 15H

	mov bx, 0BBH

	mov cx, 3412H

	mov dx, cx

	;primero se guarda la direccion del segmento en ax, y de ahi se pasa a ds
	
	;no podemos cargar en ds directamente la direccion del segmento
	mov ax, 6553H
	mov ds, ax
	mov bh, ds:[6H]
	mov bl, ds:[7H]

	mov ch, 12H ;damos este valor a ch para comprobar en el depurador que la operacion se realiza correctamente
	mov ax, 5000H
	mov ds, ax
	mov ds:[5H], ch

	mov ax, [SI]

	;en la pila, las direcciones van "al reves", por eso restamos 10, en vez de sumar, para obtener la direccion que esta por encima
	mov bx, ss:[bp-10]

	; FIN DEL PROGRAMA
	MOV AX, 4C00H
	INT 21H 

START ENDP

;FIN DEL SEGMENTO DE CODIGO
CODE ENDS

;FIN DE PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION
END START
