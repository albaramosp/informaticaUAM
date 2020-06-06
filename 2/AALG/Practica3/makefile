#-----------------------
# IMPORTANTE: Los valores de los parametros de los ejercicio?_test deben ajustarse.
# Se asume que los ficheros fuente se llaman ejercicio1.c, ejercicio2.c,...,ordenar.h
#-----------------------

CC = gcc -ansi -pedantic
CFLAGS = -Wall
EXE = ejercicio1 ejercicio2

all : $(EXE)

.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o busqueda.o tiempos.o permutaciones.o
	@echo "#---------------------------"
	@echo "# Generando $@ "
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ $@.o busqueda.o tiempos.o permutaciones.o

permutaciones.o : permutaciones.c permutaciones.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<

 busqueda.o : busqueda.c busqueda.h
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
	@./ejercicio1 -tamanio 10 -clave 8

ejercicio2_test:
	@echo Ejecutando ejercicio2
	@./ejercicio2 -num_min 1000 -num_max 10000 -incr 50 -n_veces 1 -fichSalida busqueda_bbin.dat

ejercicio2_valtest:
	@echo Ejecutando ejercicio2
	@./ejercicio2 -num_min 1000 -num_max 10000 -incr 50 -n_veces 1 -fichSalida busqueda_blin_no_ord.dat
