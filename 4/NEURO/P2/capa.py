class Capa:

    def __init__(self):
        self.neuronas=[]
        self.redNeuronal = None


    def inicializar(self):
        for n in self.neuronas:
            n.inicializar(0.0)
    
    
    def liberar(self):
        for n in self.neuronas:
            n.liberar()

        
    def añadir(self, neurona):
        self.neuronas.append(neurona)


    # Para perceptron
    def conectar(self, neurona, modo_peso):
        for n in self.neuronas:
            n.conectar(neurona, )


    # idem
    def conectar(self, capa, modo_peso):
        for n in self.neuronas:
            for n2 in capa.neuronas:
                n.conectar(n2, )


    def disparar(self):
        for n in self.neuronas:
            n.disparar()
        

    def propagar(self):
        for n in self.neuronas:
            n.propagar()
    