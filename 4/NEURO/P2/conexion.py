class Conexion:

    def __init__(self, peso, neurona):
        self.peso = peso
        self.neurona = neurona
        self.peso_anterior = 0.0
        self.valor_recibido = 0.0
        

    def liberar(self):
        pass
    #     self.neurona = None
    #     self.peso = 0.0
    #     self.peso_anterior = 0.0
    #     self.valor_recibido = 0.0


    def propagar(self, valor): # no pasarle valor
        #self.neurona.valor += self.valor_recibido*self.peso
        self.valor_recibido = valor
    