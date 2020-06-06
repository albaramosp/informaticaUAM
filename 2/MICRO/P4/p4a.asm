 ;**************************************************************************
; 									SBM 
; Pareja numero: 5
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************


; DEFINICION DEL SEGMENTO DE CODIGO
CODE SEGMENT 
    ASSUME CS:CODE
	
	org 256 ;Dejamos el espacio del PSP
	
	empezar:
	; COMIENZO DEL PROCEDIMIENTO PRINCIPAL
	START PROC 
		
		xor ax, ax
		
		;comprobacion de argumentos
		mov bx, 80h
		mov cl, BYTE PTR [bx] ;en esta posicion esta el numero de argumentos recibidos
		cmp cx, ax
		je estado
		
		mov bx, 83h
		mov ax, 69h ;comprobamos si el argumento es "/i"
		mov cl, BYTE PTR [bx] ;las posiciones 81h y 82h son, respectivamente, el ' ' y el '/', queremos ver la letra
		cmp cx, ax
		je instalar
		
		mov ax, 49h ;comprobamos si el argumento es "/I"
		cmp cx, ax
		je instalar
		
		mov ax, 64h ;comprobamos si el argumento es "/d"
		cmp cx, ax
		je desinstalar
		
		mov ax, 44h ;comprobamos si el argumento es "/D"
		cmp cx, ax
		je desinstalar
		
	estado:
		call mostrarEstado
		jmp finalizar
		
	instalar:
		call instalador
		jmp finalizar
	
	desinstalar:
		call desinstalador
		
	finalizar:
		MOV AX, 4C00H 
		INT 21H
	START ENDP 
	
	;Variables globales
	MATRIZ  DB '5','6','7','8','9','A',    'B','C','D','E','F','G',    'H','I','J','K','L','M',    'N','O','P','Q','R','S',    'T','U','V','W','X','Y',    'Z','0','1','2','3','4'	;Matriz con los valores ASCII, utilizada para la decodificacion.
	
	TEXTO DB 30 dup (0) ;cadena con el texto a imprimir tras codificar o decodificar
	
	CLR_PANT 		DB 	1BH,"[2","J$",'$' 
	
	TIULO 			DB 	1BH,"[1;1fEL POLIBIADOR TM, TU TRADUCTOR DE CONFIANZA$"
	AUTOR 			DB 	1BH,"[2;1fAlba Ramos, Arturo Morcillo$"
	GRUPO	 		DB 	1BH,"[3;1fGrupo 5 $"
	
	DRIVER_SI		DB 	1BH,"[5;1fEl driver ya esta instalado$"
	DRIVER_NO		DB 	1BH,"[5;1fEl driver no esta instalado$"
	DRIVER_YA		DB 	1BH,"[5;1fOperacion realizada con exito$"
	
	INSTR1			DB 	1BH,"[7;1fPara instalar el driver, adjuntar el argumento < /i> o < /I>$"
	INSTR2			DB 	1BH,"[8;1fPara desinstalar el driver, adjuntar el argumento < /d> o < /D>$"
	INSTR3			DB 	1BH,"[9;1fLa RSI codifica una cadena a Polibio si AH=10h, o la decodifica si AH=11h$"
	INSTR4			DB 	1BH,"[11;1fPulse intro para seguir$"


	
	
	;______________________________________________________________________________________
	;_______________________Rutina de servicio a la interrupcion 57h_______________________
	; Permite codificar o decodificar una cadena con formato Polibio, en funcion del 
	; argumento recibido en AH.
	;______________________________________________________________________________________
	rsi PROC FAR
		push bx ax
		
		cmp ah, 10h ;comprobar AH para ver si hay que codificar o decodificar
		je codificar ;AH = 10 --> codificar
		cmp ah, 11h
		jne acaba
		call decod ;AH = 11 --> decodificar
		jmp acaba 
		
	codificar:
		call cod
		
	acaba:
		pop ax bx
		iret
	rsi ENDP
	;______________________________________________________________________________________
	;_______________________Procedimiento de codificacion__________________________________
	; Rutina que recibe una cadena en DS:DX y muestra su cofidicacion en Polibio
	; El resultado se almacena en Texto
	;______________________________________________________________________________________
	COD PROC NEAR
		push bp
		mov bp, sp
		push ax bx si dx cx
		xor di, di
		
		
		mov si, dx	;Las cadenas estan en DS:DX
	
	codBucle:	
		xor bx, bx ;Cargamos 0 en los registros bx, cx y ax
		xor ax, ax
		xor cx, cx 
		mov cl, 6 ;guardamos la dimension de la matriz
		
		mov bl, ds:[si] ;bl almacena el caracter a codificar, codigo ASCII en hex
		
		cmp bx, 24h ;miramos si es el '$'
		je codFinal
		
		cmp bx, 3Ah ;miramos si es un numero
		jb codNumero
		
		cmp bx, 61h ;miramos si es una letra minuscula
		ja codMinuscula
		
		jmp codMayuscula ;si no es ninguno de los anteriores es una letra mayuscula
		
	codMinuscula:
		mov ax, 61h ;cargamos en ax el ASCII de la 'a'
		jmp codCalculoLetra
		
	codMayuscula: 
		mov ax, 41h ;cargamos en ax el ASCII de la 'A'
		
	codCalculoLetra:
		sub bx, ax ;orden importa para no tener resultados negativos
		mov ax, bx ;ax contiene la distancia entre el caracter y la A
		
		call algoritmo
		
		jmp codAlmacenar
		
	codNumero:
		mov ax, 35h
		cmp bx, ax ;comparamos nuestro caracter con el ascii del 5
		jb codMenorCinco ;numero entre 0 y 4
		jmp codMayorCinco ;numero entre 5 y 9
	
	codMenorCinco:
		mov ax, 0
		sub bx, ax 
		mov ax, bx ;ax contiene la distancia con el 0
		
		call algoritmo
		
		jmp codAlmacenar
		
	codMayorCinco:
		sub bx, ax 
		mov ax, bx ;ax contiene la diferencia con el 5
		add ax, 60
		
	codAlmacenar:
		mov bl, ah
		mov dl, al
		mov ah, 2
		int 21h
		mov dl, bl
		int 21h
		mov dl, ' '
		int 21h
		inc si 			  ;para acceder a la siguiente posicion
		jmp codBucle
		
	codFinal:
		
		
		pop cx dx si bx ax
		pop bp
		
		RET
	COD ENDP
	
	
	;______________________________________________________________________________________
	;_______________________Procedimiento para codificar un caracter_______________________
	; Devuelve en ax el valor decimal en ASCII del caracter codificado
	; Recibe en ax la distancia del caracter que quiere codificar, para aplicar el algoritmo
	;______________________________________________________________________________________
	algoritmo PROC NEAR ;REVISADO
		push bp
		mov bp, sp
		push dx
		
		;Al comienzo ax tiene la distancia
		;Queremos que en ah este la columna y en al la fila.
		;La columna sera el resto al dividir la distancia por 6 y 6 cuando el resto sea 0
		;La fila sera el cociente mas 2 en casos normales y mas 1 cuando el cociente sea 0
		
		xor dx,dx
		mov dl, 6
		div dl ;ah contiene el resto y al el cociente
		
		cmp ah, 0 ;comparacion necesaria con el 0
		inc al
		je resto0
		;Esto si el resto no es cero
		inc al
		jmp finalgoritmo
		
	resto0:
		add ah, 6
		
	finalgoritmo:
		add al,48 ;esto es para guardarlo en ASCII directamente
		add ah,48
		pop dx
		pop bp
		
		ret
	algoritmo ENDP
	
	
	;______________________________________________________________________________________
	;_______________________Procedimiento de decodificacion________________________________
	;______________________________________________________________________________________
	DECOD PROC NEAR
		push bp
		mov bp, sp
		push bx dx si di
		xor di, di
		mov si,dx
	
	decodBucle:	
		xor bx, bx ;Cargamos 0 en los registros bx y ax
		xor ax, ax
		
		mov al, ds:[si] ;al almacena el primer caracter a decodificar, codigo ASCII en hex
		cmp ax, 24h ;miramos si es el '$'
		je decodFinal
		sub ax, 48 ;ahora es un numero, no un ASCII
		
		;Si no es el final ax contiene las decenas
		inc si
		mov cl, al
		add bl, 10
		mul bl ;Ahora ax contiene las decenas bien escritas
		xor bx, bx
		mov bl, ds:[si] ;bl contiene las unidades
		sub bx, 48 ;ahora es un numero, no un ASCII
		add ax, bx ;ax contiene el numero. bl la columna y cl la fila
		
		;Comparacion si es un numero entre 5 y 9
		cmp ax, 11
		jb decodNumeroGrande ;es un numero entre 5-9
		
		cmp ax,61
		ja decodNumeroPequeno ;es un numero entre 0-4
		
		;es una letra
		mov al, cl ;ax tiene la fila
		cmp al,1
		je letraa
		;No es la letra A.
		;FORMULA: La letra es 65+[(fila-2)*6 + columna].... El 65 por el ascii de la a
		dec al
		dec al ;fila -= 2
		xor dx, dx
		add dl, 6
		mul dl ;ax = (fila-2)*6
		
		add al, bl ; ax = (fila-2)*6 + columna
		
		add al, 65 ;ax contiene la letra a almacenar en ascii
		
		jmp decodAlmacenar
	
	letraa:
		mov ax, 65 ;la letra es la A
		
	
		jmp decodAlmacenar
	
	decodNumeroGrande:
		;En este caso el numero es el numero de la columna +4
		add bl, 4
		mov al,bl
		
		add al, 48 ;Para que este en ascii
		
		
		jmp decodAlmacenar
	decodNumeroPequeno:
	
		;Si es un numero pequeño el numero es la culimna -2
		dec bl 
		dec bl 
		mov al, bl
		add al, 65
		
	decodAlmacenar:
		mov dl, al
		mov ah, 2
		int 21h
		inc si ;para acceder a la siguiente posicion
		jmp decodBucle
		
	decodFinal:
		
		
		
		pop di si dx bx
		pop bp
		
		RET
	DECOD ENDP 
	
	;______________________________________________________________________________________
	;_______________________Procedimiento de instalacion de rsi____________________________
	;______________________________________________________________________________________
	instalador PROC NEAR
		push ax es bx ds 
		
		xor ax, ax  ;el vector de interrupciones comienza en la direccion 0
		mov es, ax
		
		cmp es:[57h*4+2], ax ;comprobar que la rutina no este ya instalada, vemos que el segmento no sea 0
		jne fin ;si el segmento no es 0, ya hay algo ahi, asi que no lo volvemos a instalar
		
		mov ax, offset rsi ;offset donde esta la RSI
		mov bx, cs ;segmento en donde esta la RSI
		
		cli
		mov es:[57h*4], ax ;offset de la rsi
		mov es:[57h*4+2], bx ;segmento de la rsi
		sti
		
		mov bx, cs
		mov ds, bx ;Tenemos que tener DS apuntando a donde estan las variables que queremos imprimir
		
		MOV AH,9 ;Informamos de que el driver se ha instalado
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET DRIVER_YA
		INT 21H
		
		;mov ah, 
		mov dx, offset instalador ;offset de la posicion siguiente a la ultima que dejaremos residente
		int 27h ;acaba y deja residente el psp, la rsi y las variables globales
		
	fin:
		mov bx, cs
		mov ds, bx ;Tenemos que tener DS apuntando a donde estan las variables que queremos imprimir
		
		MOV AH,9 ;Informamos de que el driver ya estaba instalado
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET DRIVER_SI
		INT 21H
		
		pop ds bx es ax
		ret
	instalador ENDP
	
	
	;______________________________________________________________________________________
	;_______________________Procedimiento de desinstalacion de rsi_________________________
	;Permite liberar la memoria que se dejo residente, desinstalando la RSI de la int. 57h
	;______________________________________________________________________________________
	desinstalador PROC NEAR
		push ax bx cx dx ds es
		
		xor cx, cx
		xor bx, bx
		mov ds, cx ;apuntamos al segmento de vectores de interrupcion
		
		mov es, ds:[57h*4+2] ;segmento de la rsi
		mov bx, es
		
		cmp bx, cx ;comprobar si estaba instalada antes de desinstalar
		je imprime ;Si hay un cero de segmento, es que no esta instalada, entonces no hay que desinstalarla
		
		cli
		mov ds:[57h*4], cx
		mov ds:[57h*4+2], cx ;pone a 0 el vector de interrupcion de la 57h
		sti
		
		xor dx, dx
		mov dx, cs
		mov ds, dx ;Tenemos que tener DS apuntando a donde estan las variables que queremos imprimir
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET DRIVER_YA
		INT 21H
		
		mov ah, 49h ;liberar lo que haya en ES
		int 21h 	;libera el segmento de la rsi
		
		mov bx, es:[2Ch]   	 ;segmento del entorno del PSP de la rsi
		mov es, bx
		int 21h	 	;libera el segmento de variables de entorno de la rsi
		
		jmp finaliza
		
	imprime:
		mov ax, cs
		mov ds, ax ;Tenemos que tener DS apuntando a donde estan las variables que queremos imprimir
		xor ax, ax
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET DRIVER_NO
		INT 21H
		
	finaliza:
		pop es ds dx cx bx ax
		ret
	desinstalador ENDP
	;______________________________________________________________________________________
	;______________________________________________________________________________________
	;______________________________________________________________________________________
	
	;______________________________________________________________________________________
	;_______________________Procedimiento que muestra nuestra informacion__________________
	;______________________________________________________________________________________
	mostrarEstado PROC NEAR
		push ax es ds
		
		mov ax, cs
		mov ds, ax ;Tenemos que tener DS apuntando a donde estan las variables que queremos imprimir
		
		xor ax, ax
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET TIULO
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET AUTOR
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET GRUPO
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET INSTR1
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET INSTR2
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET INSTR3
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET INSTR4
		INT 21H
		
		xor ax, ax ;el vector de interrupciones comienza en la direccion 0
		mov es, ax
		
		cmp es:[57h*4+2], ax ;comprobar estado del driver
		jne driverSi
		jmp driverNo
		
	driverSi:
		MOV AH,9
		MOV DX, OFFSET DRIVER_SI
		INT 21H
		jmp acabar
	
	driverNo:
		MOV AH,9
		MOV DX, OFFSET DRIVER_NO
		INT 21H
	
	acabar:
		MOV AH,0AH ;Espera intro del usuario
		INT 21H
		
		MOV AH,9
		MOV DX, OFFSET CLR_PANT
		INT 21H
	
		pop ds es ax
		ret
	mostrarEstado ENDP
	
CODE ENDS 
; FIN DEL PROGRAMA INDICANDO DONDE COMIENZA LA EJECUCION 
END START 

