#   Sistemas Operativos
#   Práctica 2
# 	Fecha: Marzo de 2019
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
EJECUTABLES = ejercicio2 ejercicio3d ejercicio4 ejercicio6a ejercicio6b ejercicio8 ejercicio9

#=============================================================
# Generación de ejecutables
#=============================================================
all: $(EJECUTABLES)

ejercicio2: ejercicio2.o
	$(CC) $(CFLAGS) -o ejercicio2 ejercicio2.o

ejercicio3d: ejercicio3d.o
	$(CC) $(CFLAGS) -o ejercicio3d ejercicio3d.o

ejercicio4: ejercicio4.o
	$(CC) $(CFLAGS) -o ejercicio4 ejercicio4.o

ejercicio6a: ejercicio6a.o
	$(CC) $(CFLAGS) -o ejercicio6a ejercicio6a.o

ejercicio6b: ejercicio6b.o
	$(CC) $(CFLAGS) -o ejercicio6b ejercicio6b.o

ejercicio8: ejercicio8.o
	$(CC) $(CFLAGS) -o ejercicio8 ejercicio8.o

ejercicio9: ejercicio9.o
	$(CC) $(CFLAGS) -o ejercicio9 ejercicio9.o

#=============================================================
# Generación de archivos .o
#=============================================================
# Por cada archivo .c se genera un .o
# Cada .o depende del archivo .c indicado y de todos los .h que se incluyan

ejercicio2.o: ejercicio2.c
	$(CC) $(CFLAGS) -c ejercicio2.c -o ejercicio2.o

ejercicio3d.o: ejercicio3d.c
	$(CC) $(CFLAGS) -c ejercicio3d.c -o ejercicio3d.o

ejercicio4.o: ejercicio4.c
	$(CC) $(CFLAGS) -c ejercicio4.c -o ejercicio4.o

ejercicio6a.o: ejercicio6a.c
	$(CC) $(CFLAGS) -c ejercicio6a.c -o ejercicio6a.o

ejercicio6b.o: ejercicio6b.c
	$(CC) $(CFLAGS) -c ejercicio6b.c -o ejercicio6b.o

ejercicio8.o: ejercicio8.c
	$(CC) $(CFLAGS) -c ejercicio8.c -o ejercicio8.o

ejercicio9.o: ejercicio9.c
	$(CC) $(CFLAGS) -c ejercicio9.c -o ejercicio9.o

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
