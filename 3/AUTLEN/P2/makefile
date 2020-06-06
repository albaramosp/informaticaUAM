# Makefile para automatizar la creacion y el borrado de los ficheros necesarios
# para ejecutar el programa de minimizacion de automatas.
# Alba Ramos Pedroviejo
# P24

CC = gcc
CFLAGS = -g -Wall -ansi -pedantic
EXE = pr_minimiza

all: afnd_ofus.o minimiza.o $(EXE)

pr_minimiza.o: pr_minimiza.c
		$(CC) $(CFLAGS) -c pr_minimiza.c -o pr_minimiza.o

minimiza.o: minimiza.c
		$(CC) $(CFLAGS) -c minimiza.c -o minimiza.o

afnd_ofus.o: afnd_ofus.c
	@echo ""
	@echo "Makefile para practica 2 AUTLEN"
	@echo "Alba Ramos Pedroviejo, P24"
	@echo ""
	@echo "Funcionalidad"
	@echo "\t-make genera el ejecutable para probar el programa."
	@echo "\t-make valgrind analiza el uso de memoria del programa"
	@echo "\t-make dot DOT="file.dot" PDF="file.pdf" genera un diagrama de transiciones en file.pdf a partir del file.dot especificado"
	@echo ""
	$(CC) -c afnd_ofus.c -o afnd_ofus.o

$(EXE): pr_minimiza.o
		$(CC) $(CFLAGS) -o $(EXE) pr_minimiza.o minimiza.o afnd_ofus.o
		@echo "Generado ejecutable $(EXE)"

dot:
	dot -Tpdf $(DOT) -o $(PDF)
	@echo "Generado diagrama de transiciones en $(PDF)"

valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
		rm -rf *.o
		rm -rf $(EXE)
		rm -rf *.dot
