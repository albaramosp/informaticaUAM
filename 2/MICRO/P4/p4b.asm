;**************************************************************************
; 									SBM 
; Pareja numero: 5
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************

; DEFINICION DEL SEGMENTO DE DATOS 
DATOS SEGMENT 
	CLR_PANT 		DB 	1BH,"[2","J$",'$' 
	
	EJEMPLOCOD 		DB "Polibio$"
	EJEMPLODECOD 	DB "43423532213242$"
	
	TITULO 			DB 	1BH,"[1;1fEL POLIBIADOR TM, TU TRADUCTOR DE CONFIANZA$"
	AUTOR 			DB 	1BH,"[2;1fAlba Ramos, Arturo Morcillo$"
	GRUPO	 		DB 	1BH,"[3;1fGrupo 5", 10, 13, '$'
	
	;FILAS DE LA MATRIZ PARA IMPRIMIR. 10,13 ES UN \n
	FILA1 			DB 	1BH,"		5 6 7 8 9 A",10,13,'$'
	FILA2			DB 	1BH,"		B C D E F G",10,13,'$' 
	FILA3 			DB 	1BH,"		H I J K L M",10,13,'$' 
	FILA4 			DB 	1BH,"		N O P Q R S",10,13,'$' 
	FILA5 			DB 	1BH,"		T U V W X Y",10,13,'$' 
	FILA6 			DB 	1BH,"		Z 0 1 2 3 4",10,13,'$' 
	
	AUX				DB 	5 DUP (0) 
	
	INSTRC 	DB 	1BH,"[12;1fPrograma que prueba a codificar y decodificar un texto predeterminado.", 10, 13, "Ve pulsando intro para seguir.$"
	COD 	DB 	1BH,"[8;1fPara codificar el mensaje Polibio, pulsa intro: $"
	DECOD 	DB 	1BH,"[8;1fPara decodificar el mensaje 43423532213242, pulsa intro: $"
DATOS ENDS 


; DEFINICION DEL SEGMENTO DE PILA 
PILA    SEGMENT STACK "STACK" 
    DB   40H DUP (0) 
PILA ENDS 


; DEFINICION DEL SEGMENTO EXTRA
EXTRA     SEGMENT 
    RESULT    DW 0,0  
EXTRA ENDS 


; DEFINICION DEL SEGMENTO DE CODIGO
CODE    SEGMENT 
    ASSUME CS:CODE, DS:DATOS, ES: EXTRA, SS:PILA 

	; COMIENZO DEL PROCEDIMIENTO PRINCIPAL
	START PROC 
		;INICIALIZA LOS REGISTROS DE SEGMENTO CON SUS VALORES 
		MOV AX, DATOS 
		MOV DS, AX 

		MOV AX, PILA 
		MOV SS, AX 

		MOV AX, EXTRA 
		MOV ES, AX 

		; CARGA EL PUNTERO DE PILA CON EL VALOR MAS ALTO 
		MOV SP, 64 

		;COMIENZO DEL PROGRAMA 
		CALL MOSTRAR_MENU ;Mostramos la matriz de Polibio y una pequenya explicacion
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET COD
		INT 21H
		
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		mov dx, offset EJEMPLOCOD
		mov al, 0FFh	;TODO: quitar esto
		mov ah, 10h
		int 57h
		
		
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET DECOD
		INT 21H
		
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		mov dx, offset EJEMPLODECOD
		mov al, 00h
		mov ah, 11h
		int 57h
		
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		MOV AX, 4C00H 
		INT 21H
	START ENDP 
	
	;_______________________________________________________________ 
	; SUBRUTINA PARA MOSTRAR LA MATRIZ DE POLIBIO Y UNA INTRODUCCION
	;_______________________________________________________________ 
	MOSTRAR_MENU PROC NEAR
		push ax dx
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV DX,OFFSET TITULO		
		INT 21H
		
		MOV DX,OFFSET AUTOR		
		INT 21H
		
		MOV DX,OFFSET GRUPO		
		INT 21H
		
		MOV DX,OFFSET FILA1		
		INT 21H
		
		MOV DX,OFFSET FILA2		
		INT 21H
		
		MOV DX,OFFSET FILA3	
		INT 21H
		
		MOV DX,OFFSET FILA4		
		INT 21H
		
		MOV DX,OFFSET FILA5	
		INT 21H
		
		MOV DX,OFFSET FILA6	
		INT 21H
		
		MOV DX,OFFSET INSTRC	
		INT 21H
		
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		pop dx ax
		ret
	MOSTRAR_MENU ENDP

CODE ENDS 
; FIN DEL PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION 
END START 

