;**************************************************************************
; 									SBM 
; Pareja numero: 5
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************

; DEFINICION DEL SEGMENTO DE CODIGO COMO PUBLICO
_TEXT SEGMENT BYTE PUBLIC 'CODE'
    ASSUME CS:_TEXT

	; COMIENZO DEL PROCEDIMIENTO PRINCIPAL
	_MAIN PROC 
		;COMIENZO DEL PROGRAMA 
		PUBLIC _createBarCode
		

		;__________________________________________________________________________
		; SUBRUTINA PARA GENERAR UN CODIGO DE BARRAS EN FORMATO ASCII,
		; UTILIZANDO LOS VALORES RECIBIDOS POR PARAMETRO EN LAS VARIABLES.
		; DEVUELVE EL CODIGO DE BARRAS EN LA VARIABLE barCodeStrCorregido
		;_________________________________________________________________________
		
		;______________________________________________________________
		;______________________________________________________________
		;____________________________NOTAS_____________________________
		;______________________________________________________________
		;______________________________________________________________
			
		;Los parametros se almacenan en la pila de la siguiente forma:
	
			;|||SEGMENT barCodeStrCorregido	|||
			;|||OFFSET barCodeStrCorregido 	|||	+16
			;|||codigoControlCorregido		|||	+14 
			;|||codigoProducto				|||	+12
			;|||codigoProducto	   			|||	+10 (2 huecos porque es un long)
			;|||codigoEmpresa				|||	+8
			;|||codigoPais					|||	+6
			;|||SEGMENT ret					|||	
			;|||OFFSET ret					|||	+2
			;|||bp							|||	0	Este le añadimos nosotros
			
			
		;______________________________________________________________
		;______________________________________________________________
		;__________________________FIN NOTAS___________________________
		;______________________________________________________________
		;______________________________________________________________
		
		
		_createBarCode PROC FAR
			push bp
			mov bp,sp
			push ax bx cx dx es si ;guardamos los registros que vayamos a modificar
			
			les si, ss:[bp+16] ;aqui almacenaremos los valores ascii en Little Endian
			mov al,0 ;Caracter de fin de cadena ASCIIZ
			mov es:[si+13],al ;ponemos el fin de cadena
			
			mov cx,10 ;vamos a dividir entre 10 para obtener los digitos de cada codigo, de menos a mas significativo
			
		;CODIGO DE PAIS
			mov ax, 0
			mov ax, ss:[bp+6] ;ax contiene el codigo de pais en decimal
			
			mov bx,2 ;repeticiones para los 3 digitos que forman el codigo. Es el indice donde guardar cada digito ASCII
			
		bucleCodigoPais:
			mov dx,0
			div cx ;operando de 16b. Resto en dx, con el cual trabajaremos, y cociente en ax
			add dl,48 ;valor ascii del digito. Usamos dl porque es un unico digito
			
			mov es:[si+bx], dl ;guardamos en el destino
			
			dec bx; Para obtener la posicion correcta en cada iteracion
			cmp bx, -1
			jne bucleCodigoPais
			
			
		;CODIGO DE EMPRESA
			mov ax, 0
			mov ax, ss:[bp+8] ;ax contiene el codigo de la empresa en decimal
			
			mov bx,3 ;repeticiones para los 4 digitos que hay que guardar. Es el indice donde guardar cada digito ASCII
			
		bucleCodigoEmpresa:
			mov dx,0
			div cx
			add dl,48
			mov es:[si+bx+3], dl ;guardamos en el destino. Se suma 3 por la posicion, pues ya hemos guardado los 3 primeros
			
			dec bx ;para almacenarlo en la posicion correcta en cada iteracion
			cmp bx,-1
			jne bucleCodigoEmpresa
			
		;CODIGO DE PRODUCTO
			mov ax, 0
			mov ax, ss:[bp+10] ;parte baja del long
			mov dx, ss:[bp+12] ;parte alta del long
			
			mov bx, 4 ;repeticiones para los 5 digitos que hay que guardar. Es el indice donde guardar cada digito ASCII
			
		bucleCodigoProducto:
			div cx
			add dl,48
			mov es:[si+bx+7], dl ;guardamos en el destino. Se suma 7 por la posicion, pues ya tenemos en total 7 digitos guardados
			mov dx,0
			
			dec bx
			cmp bx,-1
			jne bucleCodigoProducto 
			
		;DIGITO DE CONTROL
			mov al, [bp + 14]		
			add al, 48
			mov es:[si + 12], al
			
			pop si es dx cx bx ax ;restauramos antes de salir los registros modificados
			pop bp
			ret
			
		_createBarCode ENDP
	_MAIN ENDP 
_TEXT ENDS 
END