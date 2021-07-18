from conexion import Conexion
from enum import IntEnum

import math

class TiposNeurona(IntEnum):
    MCCULLOCH = 1
    PERCEPTRON = 2
    DIRECTA = 3
    SESGO = 4
    ADALINE_TRAIN = 5
    ADALINE_TEST = 6
    SIGMOIDE_BINARIA = 7
    SIGMOIDE_BIPOLAR = 8

    def funcion_sigmoide_binaria(x):
        return 1/(1+math.exp(-x))

    def derivada_sigmoide_binaria(x):
        return TiposNeurona.funcion_sigmoide_binaria(x) * (1-TiposNeurona.funcion_sigmoide_binaria(x))

    def funcion_sigmoide_bipolar(x):
        try:
            return 2/(1+math.exp(-x)) - 1
        except:
            return 0.0

    def derivada_sigmoide_bipolar(x):
        return 1/2 * (1+TiposNeurona.funcion_sigmoide_bipolar(x)) * (1-TiposNeurona.funcion_sigmoide_bipolar(x))
    

class Neurona:

    def __init__(self, umbral, tipo, valor=0.0, salida_activa=1.0, salida_inactiva=0.0, f_x=0.0):
        self.umbral=umbral
        self.tipo=tipo
        self.valor=valor
        self.salida_activa=salida_activa
        self.salida_inactiva=salida_inactiva
        self.f_x=f_x
        self.conexiones=[]
        self.capa=None


    def liberar(self):
        pass


    def inicializar(self, valor):
        self.valor = valor


    def conectar(self, neurona, peso):
        self.conexiones.append(Conexion(peso, neurona))


    def disparar(self):
        if self.tipo == TiposNeurona.MCCULLOCH or self.tipo == TiposNeurona.ADALINE_TEST:
            if self.valor >= self.umbral:
                self.f_x = self.salida_activa
            else:
                self.f_x = self.salida_inactiva
        
        elif self.tipo == TiposNeurona.PERCEPTRON:
            if self.valor > self.umbral:
                self.f_x = self.salida_activa
            elif self.valor < -self.umbral:
                self.f_x = self.salida_inactiva
            else:
                self.f_x = 0.0 # Incertidumbre
        
        elif self.tipo == TiposNeurona.DIRECTA or self.tipo == TiposNeurona.ADALINE_TRAIN:
            self.f_x = self.valor

        elif self.tipo == TiposNeurona.SESGO:
            self.f_x = 1.0
        
        elif self.tipo == TiposNeurona.SIGMOIDE_BINARIA:
            self.f_x = TiposNeurona.funcion_sigmoide_binaria(self.valor)

        elif self.tipo == TiposNeurona.SIGMOIDE_BIPOLAR:
            self.f_x = TiposNeurona.funcion_sigmoide_bipolar(self.valor)
        
        else:
            pass
        
        for c in self.conexiones:
            c.propagar(self.f_x)
            #c.valor_recibido = self.f_x * self.valor


    def propagar(self):
        for c in self.conexiones:
            # print("Propagamos: ", c.neurona.valor, " + ", c.valor_recibido, " * ", c.peso)
            c.neurona.valor += c.valor_recibido*c.peso
            #c.propagra()