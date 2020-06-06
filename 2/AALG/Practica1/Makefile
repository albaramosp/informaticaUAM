#-----------------------
# IMPORTANTE: Los valores de los parametros de los ejercicio?_test deben ajustarse.
# Se asume que los ficheros fuente se llaman ejercicio1.c, ejercicio2.c,...,ordenar.h
#-----------------------

CC = gcc -ansi -pedantic
CFLAGS = -Wall
EXE = ejercicio1 ejercicio2 ejercicio3 ejercicio4 ejercicio5

all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o ordenacion.o tiempos.o permutaciones.o
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ $@.o ordenacion.o tiempos.o permutaciones.o

permutaciones.o : permutaciones.c permutaciones.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

 ordenacion.o : ordenacion.c ordenacion.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

 tiempos.o : tiempos.c tiempos.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<
	
ejercicio1_test:
	@echo Ejecutando ejercicio1
	@valgrind --leak-check=yes --tool=memcheck ./ejercicio1 -limInf 6 -limSup 10 -numN 10

ejercicio2_test:
	@echo Ejecutando ejercicio2
	@valgrind --leak-check=yes --tool=memcheck ./ejercicio2 -tamanio 10 -numP 5

ejercicio3_test:
	@echo Ejecutando ejercicio3
	@valgrind --leak-check=yes --tool=memcheck ./ejercicio3 -tamanio 10 -numP 5

ejercicio4_test:
	@echo Ejecutando ejercicio4
	@valgrind --leak-check=yes --tool=memcheck ./ejercicio4 -tamanio 10

ejercicio5_test:
	@echo Ejecutando ejercicio5
	@valgrind --leak-check=yes --tool=memcheck ./ejercicio5 -num_min 9000 -num_max 10000 -incr 500 -numP 10 -fichSalida ejercicio5.log

