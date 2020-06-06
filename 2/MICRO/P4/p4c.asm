; DEFINICION DEL SEGMENTO DE DATOS 
DATOS SEGMENT 
	CLR_PANT 		DB 	1BH,"[2","J$",'$' 
	ENTEER			DB	"",10,13, '$'
	
	TITULO 			DB 	1BH,"[1;1fEL POLIBIADOR TM, TU TRADUCTOR DE CONFIANZA$"
	AUTOR 			DB 	1BH,"[2;1fAlba Ramos, Arturo Morcillo$"
	GRUPO	 		DB 	1BH,"[3;1fGrupo 5", 10, 13, '$'
	 
	AUX				DB 	20 DUP (0) 
	
	EJEMPLOCOD 		DB "Polibio$"
	
	CODI	DB	0
		
	
	
	INSTRC 	DB 	1BH,"[12;1fPrograma que codifica y decodifica tus cadenas.", 10, 13, "Introducir quit para salir, cod para codificar y decod para decodificar: ", 10, 13, '$'
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
		
		xor si, si ;indice de la cadena, caracter a mostrar
		
	bucle1:

		;COMIENZO DEL PROGRAMA 
		CALL MOSTRAR_MENU ;Mostramos la explicacion
		
		xor dx, dx
		xor ax, ax
		
	bucle:
		xor cx, cx	;contador de caracteres leidos
		mov cx, 1
		
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
	
		MOV AH,0AH			;ALMACENA LA CADENA TECLEADA EN AUX
		MOV DX,OFFSET AUX
		MOV AUX[0],21		;en aux[0] el maximo numero de caracteres que queremos leer, y en aux[1] los que se han leido	
		INT 21H
		
		;Comprobamos cadenas de control QUIT, COD Y DECOD
		inc cx
		cmp aux[2], 63h ;c
		je cod1
		
		cmp aux[2], 64h ;d
		je decod1
		
		cmp aux[2], 71h ;q
		je quit1
		
		JMP otro ;si ha introducido otras cosas
		
	quit1:
		inc cx
		cmp aux[3], 75h ;u 
		jne otro
		
	quit2:
		inc cx
		cmp aux[4], 69h ;i 
		jne otro
		
	quit3:
		inc cx
		cmp aux[5], 74h ;t 
		je quit
		jmp otro
		
	decod1:
		inc cx
		cmp aux[3], 65h ;e
		jne otro
		
	decod2:
		inc cx
		cmp aux[4], 63h ;c
		jne fin1
		
	decod3: 
		inc cx
		cmp aux[5], 6Fh ;o
	fin1:
		jne fin
		
	decod4:
		inc cx
		cmp aux[6], 64h ;d
		je decod
		jmp otro
		
	cod1: 
		inc cx
		cmp aux[3], 6Fh ;o
		jne otro
		
	cod2:
		inc cx
		cmp aux[4], 64h ;d
		je cod
		jmp otro
	
	quit:
		jmp fin
    
	decod:
		mov CODI, 0 ;indico decodificacion
		inc dx
		jmp bucle	;vamos almacenando cadenas hasta que pongan QUIT
		
	cod:
		mov CODI, 1 ;indico codificacion
		inc dx
		jmp bucle	;vamos almacenando cadenas hasta que pongan QUIT
		
	otro:
		cmp dx, 0 ;si la primera vez nos introducen una cadena erronea, acabamos
		je fin ;una vez hayan elegido opcion ya si que pueden meter cadenas distintas a las de control
		
		
		mov bx, 0
		mov bp, 0
		;xor dx, dx
		
		bucle2:
			inc bl
			inc bl
			
			mov dl, aux[bx]
			
			mov ds:EJEMPLOCOD[bp], dl
			
			dec bl
			dec bl
			
			inc bl
			inc bp
			
			
			cmp bl, aux[1]
			jne bucle2
			
			mov ds:EJEMPLOCOD[bp], '$'
			
			mov dx, offset EJEMPLOCOD
			xor di, di
			mov si, dx	;Las cadenas estan en DS:DX
			
			cmp CODI, 0
			je nocod
			
			mov ah, 10h
			jmp inicia
			
		nocod:
			mov ah, 11h
			
		inicia:
			mov al, 0FEh ;Indicamos que queremos procesar la cadena, esto lo usara la interrupcion 1Ch para arrancar
			
			procesa:
				cmp al, 0EFh
				jne procesa
				
			mov al, 0FEh
		jmp bucle	;vamos almacenando cadenas hasta que pongan QUIT
		
	fin:	
		MOV AX, 4C00H 
		INT 21H
	START ENDP 
	
	;_______________________________________________________________ 
	; SUBRUTINA PARA MOSTRAR LA INTRODUCCION
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

