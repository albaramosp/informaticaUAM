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
		PUBLIC _computeControlDigit
		PUBLIC _decodeBarCode
		_resultadoimpares dw 0
		_resultadoCodigoPaisA dw 0
		_resultadoCodigoPaisB dw 0
		
		;AL LLAMAR A LA FUNCION, SE LE PASA COMO PARAMETRO EL CODIGO DE BARRAS EN ASCII, EL CUAL SE ALMACENA EN LA PILA
	_MAIN ENDP 

	
	;__________________________________________________________________________
	; SUBRUTINA PARA COMPROBAR EL DIGITO DE CONTROL DEL CODIGO DE BARRAS
	; RECIBE COMO PARAMETRO UNA CADENA ASCII QUE REPRESENTA EL CODIGO DE BARRAS
	; RETORNA EN AX EL DIGITO DE CONTROL CALCULADO
	;__________________________________________________________________________
	
	_computeControlDigit PROC FAR ;subrutina de puta madre
		;______________________________________________________________
		;______________________________________________________________
		;____________________________NOTAS_____________________________
		;______________________________________________________________
		;______________________________________________________________
			
		;Los parametros se almacenan en la pila de la siguiente forma:
			
			
			;|||SEGMENT barcode ASCII	|||	
			;|||OFFSET barcode ASCII	|||	+6
			;|||SEGMENT ret				|||	
			;|||OFFSET ret				|||	+2
			;|||bp						|||	0	Este le añadimos nosotros
			
			
		;______________________________________________________________
		;______________________________________________________________
		;__________________________FIN NOTAS___________________________
		;______________________________________________________________
		;______________________________________________________________
	
	
		push bp
		mov bp,sp
		push ax bx cx dx si di es ds
		
		les bx,[bp+6] ;Esta es la direccion donde esta el valor que queremos leer, es:[bx]
		mov cx,5 ;hay 6 impares, contando el que calculamos fuera del bucle. cx es el contador del mismo
		
		mov ax,0
		mov al, es:[bx]
		sub ax,48 ;transformacion de ASCII a decimal
		
		add bx, 12 ;asi vamos accediendo a las impares
		
	bucleImpares:
		mov dx, 0
		sub bx,cx
		sub bx,cx
		mov dl, es:[bx]
		add bx,cx
		add bx,cx
		sub dx,48
		add ax,dx
		loop bucleImpares
		
		sub bx,12 ;restauramos el valor inicial
		mov _resultadoimpares,ax ;guardamos el primer valor en la variable. Segun el ejemplo, deberia ser 33
		
		;Ahora los pares
		
		add bx, 1 ;bx apunta al primer elemento par que tenemos que coger para sumarlo :)
		mov cx,5 ;hay 6 pares
		mov ax,0
		mov al, es:[bx]
		sub ax,48
		mov si, 3
		mul si ;uso este registro porque no tengo mas disponibles. Ya lo siento
		mov dx,ax
		
	    
		add bx, 12 ;asi vamos accediendo a las pares
		
	buclePares:
		mov ax, 0
		sub bx,cx
		sub bx,cx
		mov al, es:[bx]
		add bx,cx
		add bx,cx
		sub ax,48
		mul si
		add dx,ax
		loop buclePares
		
		mov ax, _resultadoimpares ;vamos a sumar los resultados anteriores
		add ax,dx ;Segun nuestro ejemplo, ax = 105. Ahora calculamos la decena superior mas cercana
		mov cx,10
		mov dx,0
		
		div cx
		
		mov ax,10
		sub ax,dx ;dx es el valor que queremos... en principio
		
		cmp ax,10
		je caso10
		jmp fin
		
	caso10:
		mov ax,0 ;en caso de que la decena fuera 10, ponemos 0
		
	fin: ;Ahora ax contiene el digito de control bien bien
		mov _resultadoimpares,ax
		
		pop ds es di si dx cx bx ax
		pop bp ; Restaurar el valor de BP antes de salir
		
		mov ax,_resultadoimpares
		ret
	_computeControlDigit ENDP
	
	
	;_______________________________________________________________ 
	; SUBRUTINA PARA ALMACENAR CADA CAMPO DEL CODIGO DE BARRAS.
	; ALMACENAMOS CADA CAMPO DEL CODIGO EN LAS POSICIONES RECIBIDAS
	; POR ARGUMENTO.
	;_______________________________________________________________
	_decodeBarCode PROC FAR
		;______________________________________________________________
		;______________________________________________________________
		;____________________________NOTAS_____________________________
		;______________________________________________________________
		;______________________________________________________________
			
		;Los parametros se almacenan en la pila de la siguiente forma:
			
			
			;|||SEGMENT controlDigit	|||	
			;|||OFFSET controlDigit		|||	+22
			;|||SEGMENT productCode		|||	
			;|||OFFSET productCode		|||	+18
			;|||SEGMENT companyCode		|||	
			;|||OFFSET companyCode		|||	+14
			;|||SEGMENT countryCode		|||	
			;|||OFFSET countryCode		|||	+10
			;|||SEGMENT barcode ASCII	|||	
			;|||OFFSET barcode ASCII	|||	+6
			;|||SEGMENT ret				|||	
			;|||OFFSET ret				|||	+2
			;|||bp						|||	0	Este le añadimos nosotros
			
			
		;______________________________________________________________
		;______________________________________________________________
		;__________________________FIN NOTAS___________________________
		;______________________________________________________________
		;______________________________________________________________
		
		
		push bp
		mov bp,sp
		push ax bx cx dx si di es ds
		
		;CODIGO PAIS
		les bx,[bp+6] ;Esta es la direccion donde esta el valor que queremos leer, es:[bx]
		lds si, [bp + 10] ;aqui almacenaremos la direccion donde debemos escribir el valor al final, ds:[si]
		
		mov al, es:[bx]
		sub ax,48 ;ax tiene el primer valor del codigo en decimal.
		mov dl, 100 
		mul dl ;lo transformamos en centena
		
		mov cx,ax
		
		mov al,es:[bx+1]
		mov ah,0
		sub ax,48 
		mov dl,10
		mul dl ;lo transformamos en decena
		add ax, cx ;y lo sumamos a las centenas que teniamos
		
		mov cx, ax
		
		mov al,es:[bx+2]
		mov ah,0
		sub ax,48
		add ax,cx ;ya tenemos el resultado, 845 en el ejemplo
		
		mov ds:[si],ax ;a la posicion indicada en el parametro movemos el resultado, 845
		
		
		;CODIGO EMPRESA
		lds si, [bp + 14] ;aqui almacenaremos la direccion donde debemos escribir el valor al final
		
		mov al, es:[bx+3]
		mov ah,0
		sub ax,48
		mov cx, 1000
		mul cx
		mov dx, ax
		
		mov al, es:[bx+4]
		mov ah,0
		sub ax,48
		mov cl, 100
		mul cl
		add ax, dx
		mov dx,ax
		
		mov al, es:[bx+5]
		mov ah,0
		sub ax,48
		mov cl, 10
		mul cl
		add ax, dx
		mov dx,ax
		
		mov al, es:[bx+6]
		mov ah,0
		sub ax,48
		add ax, dx
		
		mov ds:[si],ax ;aqui mueve el resultado. En nuestro ejemplo, 6789
		
		
		;CODIGO PRODUCTO
		lds si, [bp + 18] ;aqui almacenaremos la direccion donde debemos escribir el valor al final
		
		;Como es un numero de 5 cifras necesitaremos 2 registros de 16b para calcular el resultado
		;el primer numero es el mas significativo
		mov al, es:[bx+7]
		mov ah,0
		sub ax,48 
		mov cx, 10000 ;operando. cabe bien en el registro
		;multiplicamos ax por cx
		mul cx ;parte alta en dx y parte baja en ax
		mov _resultadoCodigoPaisA, dx
		mov _resultadoCodigoPaisB, ax
		
		;los 4 ultimos numeros los puedo sumar normal
		mov al, es:[bx+8]
		mov ah,0
		sub ax,48
		mov cx, 1000
		mul cx
		mov dx, ax
		
		mov al, es:[bx+9]
		mov ah,0
		sub ax,48
		mov cl, 100
		mul cl
		add ax, dx
		mov dx,ax
		
		mov al, es:[bx+10]
		mov ah,0
		sub ax,48
		mov cl, 10
		mul cl
		add ax, dx
		mov dx,ax
		
		mov al, es:[bx+11]
		mov ah,0
		sub ax,48
		add ax, dx ;en ax estan los 4 digitos menos significativos ya bien, parte baja
		
		add ax, _resultadoCodigoPaisB ;sumamos las partes bajas, se guarda en ax
		mov dx, _resultadoCodigoPaisA
		adc dx, 0 ;sumamos el acarreo a la parte alta, se guarda en dx
		
		;movemos el resultado a su posicion de memoria en formato Little Endian
		mov ds:[si], ax
		mov ds:[si+2], dx
		
		;DIGITO DE CONTROL
		lds si, [bp + 22] ;aqui almacenaremos la direccion donde debemos escribir el valor al final
		
		mov al, es:[bx+12]
		mov ah,0
		sub ax,48
		
		mov ds:[si],ax ;aqui mueve 5 segun el ejemplo. Importante hacerlo como si fuera una palabra
		
		pop ds es di si dx cx bx ax
		pop bp	; Restaurar valores modificados en la funcion antes de salir
		
		ret	; Retorno a la funcion que nos ha llamado, devolviendo los resultados en cada posicion de memoria
	_decodeBarCode ENDP
_TEXT ENDS 
END