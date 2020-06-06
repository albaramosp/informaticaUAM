#   Sistemas Operativos
#   Práctica 4
# 	Fecha: Abril de 2019
#	  Autoras: Alba Ramos, Andrea Salcedo
# 	Escuela Politecnica Superior - UAM

#=============================================================
# Entorno de desarrollo y opciones
#=============================================================
# Compilador

CC = gcc

# Opciones de compilacion
CFLAGS= -Wall -pedantic -g -pthread

# Opciones de enlazado
# Indican el directorio donde localizar cada fichero
DOCPATH = ./doc/


#=============================================================
# Archivos que componen el proyecto
#=============================================================
# Ejecutables
EJECUTABLES = ejercicio2 ejercicio4A ejercicio4B ejercicio4C ejercicio4 ejercicio3_productor ejercicio3_consumidor ejercicio3_productor_b ejercicio3_consumidor_b

#=============================================================
# Generación de ejecutables
#=============================================================
all: $(EJECUTABLES)

ejercicio2: ejercicio2.o
	$(CC) $(CFLAGS) -o ejercicio2 ejercicio2.o -lrt

ejercicio4A: ejercicio4A.o
	$(CC) $(CFLAGS) -o ejercicio4A ejercicio4A.o -lrt

ejercicio4B: ejercicio4B.o
	$(CC) $(CFLAGS) -o ejercicio4B ejercicio4B.o -lrt

ejercicio4C: ejercicio4C.o
	$(CC) $(CFLAGS) -o ejercicio4C ejercicio4C.o -lrt

ejercicio4: ejercicio4.o
	$(CC) $(CFLAGS) -o ejercicio4 ejercicio4.o -lrt

ejercicio3_productor: ejercicio3_productor.o queue.o
	$(CC) $(CFLAGS) -o ejercicio3_productor ejercicio3_productor.o queue.o -lrt

ejercicio3_consumidor: ejercicio3_consumidor.o queue.o
	$(CC) $(CFLAGS) -o ejercicio3_consumidor ejercicio3_consumidor.o queue.o -lrt

ejercicio3_productor_b: ejercicio3_productor_b.o queue.o
	$(CC) $(CFLAGS) -o ejercicio3_productor_b ejercicio3_productor_b.o queue.o -lrt

ejercicio3_consumidor_b: ejercicio3_consumidor_b.o queue.o
	$(CC) $(CFLAGS) -o ejercicio3_consumidor_b ejercicio3_consumidor_b.o queue.o -lrt

#=============================================================
# Generación de archivos .o
#=============================================================
# Por cada archivo .c se genera un .o
# Cada .o depende del archivo .c indicado y de todos los .h que se incluyan

ejercicio2.o: ejercicio2.c
	$(CC) $(CFLAGS) -c ejercicio2.c -o ejercicio2.o

ejercicio4A.o: ejercicio4A.c
	$(CC) $(CFLAGS) -c ejercicio4A.c -o ejercicio4A.o

ejercicio4B.o: ejercicio4B.c
	$(CC) $(CFLAGS) -c ejercicio4B.c -o ejercicio4B.o

ejercicio4C.o: ejercicio4C.c
	$(CC) $(CFLAGS) -c ejercicio4C.c -o ejercicio4C.o

ejercicio4.o: ejercicio4.c
	$(CC) $(CFLAGS) -c ejercicio4.c -o ejercicio4.o

queue.o: queue.c queue.h types.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o

ejercicio3_consumidor.o: ejercicio3_consumidor.c queue.h
	$(CC) $(CFLAGS) -c ejercicio3_consumidor.c -o ejercicio3_consumidor.o

ejercicio3_productor.o: ejercicio3_productor.c queue.h
	$(CC) $(CFLAGS) -c ejercicio3_productor.c -o ejercicio3_productor.o

ejercicio3_consumidor_b.o: ejercicio3_consumidor_b.c queue.h
	$(CC) $(CFLAGS) -c ejercicio3_consumidor_b.c -o ejercicio3_consumidor_b.o

ejercicio3_productor_b.o: ejercicio3_productor_b.c queue.h
	$(CC) $(CFLAGS) -c ejercicio3_productor_b.c -o ejercicio3_productor_b.o
#=================================================================
# El objetivo clean limpia los archivos .o, la carpeta comprimida,
# los módulos de prueba y ejecutables del juego
#=================================================================
clean:
	rm -rf *.o  $(EJECUTABLES)

#=================================================================
# El objetivo doc limpia los archivos .o, la carpeta comprimida,
# los módulos de prueba y ejecutables del juego, y crea la
# documentación del proyecto en formatos html y latex
#=================================================================
docc:
	doxygen
