;**************************************************************************
; 					SBM 
; Pareja numero: 5
; Alba Ramos Pedroviejo
; Arturo Morcillo Penares
;**************************************************************************

1. Instrucciones
	El programa permite calcular determinantes de matrices 3x3. Al arrancarlo, aparece un menú que permite seleccionar la opción de calcular el determinante por 
	defecto de nuestro programa o introducir tus propios valores en la matriz. Si se escoge la última opción, se mostrarán unas instrucciones por pantalla y, a continuación,
	se solicitarán los números por filas. Los números permitidos van desde -16 hasta 15, ambos incluidos. 
	Además, el menú principal detecta cuando has tecleado una opción distinta a las que se esperan, y termina la ejecución del programa. Sin embargo, no está controlado
	cuando se introducen números fuera del rango especificado, ya que se nos especifica que las entradas serán de 8 bits. 

2. Decisiones de diseño
	El código del determinante por defecto no presenta dificultades en su comprensión, simplemente son llamadas a funciones.

	El código del determinante personalizado es más complejo. Este código está estructurado de la siguiente forma:
	- Mostrar un mensaje de la fila y columna solicitada
	- Comprobar si se han introducido 1, 2 o 3 caracteres, para guardar su ASCII en memoria para mostrarlo en la matriz después
	- Comprobar si el número es negativo (de 1 o 2 cifras) o no, y obtener su valor decimal a partir del ASCII. Esto se guarda en BX
	- Almacenar en la matriz de la memoria este resultado, para calcular el determinante luego.

	Esto se repite para cada una de las 9 posiciones de la matriz. Queríamos hacerlo con un bucle, pero no sabíamos como estructurarlo para almacenar cada fila.

	- Tras tener la matriz de decimales, se calcula el determinante
	- Comprobamos si es positivo o negativo, para mostrar su ASCII en el resultado
	- Mostrar la matriz y el resultado del determinante

3. Funciones

	Se explican algunas funciones utilizadas: 
	- GUARDAR_RESULTADO: guarda en BX el resultado del determinante. Este está en decimal, y queremos guardarlo en ASCII para mostrarlo por pantalla. Como puede haber 
		hasta 4 cifras (además del signo '-' en su caso), tenemos que almacenar unidades de millar (1000), centenas (100), decenas (10) y unidades. Para ello, dividimos 
		entre DX (que contiene cada valor mencionado previamente entre paréntesis). El cociente de la división está en AX (le sumaremos 48, que es el ASCII del 0) y el resto 
		en DX (para la siguiente cifra usaremos este resto y repetiremos la operación).
	- GUARDAR_RESULTADO_NEGATIVO: realiza la misma función que la anterior, pero poniendo un '-' (cuyo ASCII es 45) delante del número, el cual hay que
		pasar a complemento a 2 para operar.
	- TECLADO1 y TECLADO2: almacenan el valor ASCII tecleado por el usuario en la matriz ASCII de memoria, la que se mostrará al final
	- CONVERTIRNUMERO: convierte un número ASCII positivo (los negativos los tratan las dos últimas funciones, similares a esta) a decimal haciendo 
		el procedimiento inverso de GUARDAR_RESULTADO: en vez de dividir entre 100, 10, etc, multiplica, y en vez de sumar 48, lo resta. 
