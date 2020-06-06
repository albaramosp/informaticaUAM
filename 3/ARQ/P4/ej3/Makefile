LIBS = 
LIB_DIR =  
FLAGS = -g -Wall -D_GNU_SOURCE -lgomp -fopenmp

.PHONY: clean all

all: calcularProducto calcularProducto1 calcularProducto2 calcularProducto3
	
calcularProducto: calcularProducto.c arqo3.c
	gcc $(FLAGS) $(LIB_DIR) -o $@ $^ $(LIBS)

calcularProducto1: calcularProducto1.c arqo3.c
	gcc $(FLAGS) $(LIB_DIR) -o $@ $^ $(LIBS)

calcularProducto2: calcularProducto2.c arqo3.c
	gcc $(FLAGS) $(LIB_DIR) -o $@ $^ $(LIBS)

calcularProducto3: calcularProducto3.c arqo3.c
	gcc $(FLAGS) $(LIB_DIR) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~ calcularProducto calcularProducto1 calcularProducto2 calcularProducto3
