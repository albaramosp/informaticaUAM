#   Sistemas Operativos
#   Práctica 1
# 	Fecha: Febrero de 2019
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
EJECUTABLES = p1_exercise_3 p1_exercise_5 ejercicio4 p1_exercise_7 p1_exercise_9 ejercicio12

#=============================================================
# Generación de ejecutables
#=============================================================
all: $(EJECUTABLES)

p1_exercise_3: p1_exercise_3.o
	$(CC) $(CFLAGS) -o p1_exercise_3 p1_exercise_3.o

ejercicio4: ejercicio4.o
	$(CC) $(CFLAGS) -o ejercicio4 ejercicio4.o

p1_exercise_5: p1_exercise_5.o
	$(CC) $(CFLAGS) -o p1_exercise_5 p1_exercise_5.o

p1_exercise_7: p1_exercise_7.o
	$(CC) $(CFLAGS) -o p1_exercise_7 p1_exercise_7.o

p1_exercise_9: p1_exercise_9.o
	$(CC) $(CFLAGS) -o p1_exercise_9 p1_exercise_9.o

ejercicio12: ejercicio12.o
	$(CC) $(CFLAGS) -o ejercicio12 ejercicio12.o -lm

#=============================================================
# Generación de archivos .o
#=============================================================
# Por cada archivo .c se genera un .o
# Cada .o depende del archivo .c indicado y de todos los .h que se incluyan

p1_exercise_3.o: p1_exercise_3.c
	$(CC) $(CFLAGS) -c p1_exercise_3.c -o p1_exercise_3.o

ejercicio4.o: ejercicio4.c
	$(CC) $(CFLAGS) -c ejercicio4.c -o ejercicio4.o

p1_exercise_5.o:p1_exercise_5.c
	$(CC) $(CFLAGS) -c p1_exercise_5.c -o p1_exercise_5.o

p1_exercise_7.o: p1_exercise_7.c
	$(CC) $(CFLAGS) -c p1_exercise_7.c -o p1_exercise_7.o

p1_exercise_9.o: p1_exercise_9.c
	$(CC) $(CFLAGS) -c p1_exercise_9.c -o p1_exercise_9.o

ejercicio12.o: ejercicio12.c
	$(CC) $(CFLAGS) -c ejercicio12.c -o ejercicio12.o -lm


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

#=============================================================
# Comprobación de fugas de memoria con Valgrind
#=============================================================
valgrind_p1_exercise_5:
	valgrind -v --leak-check=full ./p1_exercise_5
