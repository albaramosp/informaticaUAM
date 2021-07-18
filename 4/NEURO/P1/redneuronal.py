from capa import *
from neurona import *
from random import uniform
from conexion import *
import numpy as np

class RedNeuronal:

    def __init__ (self):
        self.capas = []


    def inicializar(self):
        for c in self.capas:
            c.inicializar()


    def añadir(self, capa):
        self.capas.append(capa)

    
    def disparar(self):
        for capa in self.capas:
            capa.disparar()


    def propagar(self):
        for capa in self.capas:
            capa.propagar
        


def adaline(xTrain, yTrain, paciencia=500, umbral=0.0, alpha=0.1, tolerancia=0.1):
    # Sacamos nAtr y nClases
    nAtrs = len(xTrain[0])
    nClases = len(yTrain[0])

    adaline = RedNeuronal()

    # Capa de entrada con bias
    capaEntrada = Capa()

    entrada = []
    for i in range(0, nAtrs):
        entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA))
    entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias

    # Capa de salida
    capaSalida = Capa()
    salida = [] # Cada neurona predice 1 clase

    for i in range(nClases):
        salida.append(Neurona(umbral = 0.0, tipo=TiposNeurona.ADALINE_TRAIN, salida_activa=1.0, salida_inactiva=-1.0)) 

    # Creadas las neuronas

    # Conectamos las neuronas entre ellas con los pesos inicialmente a un valor random pequeño
    for i in range(0, len(entrada)):
        for j in range(0, len(salida)):
            entrada[i].conectar(salida[j], uniform(0.1, 0.5))
        capaEntrada.añadir(entrada[i])
    
    for j in range(0, len(salida)):
        capaSalida.añadir(salida[j])

    # Conectadas y añadidas a las capas

    adaline.añadir(capaEntrada)
    adaline.añadir(capaSalida)

    # Red construida
    # Comienza el algoritmo iterativo

    # 1. Presentar patrones hasta que cumpla la condicion de parada
    for contEpocas in range(paciencia+1): # Epocas
        print("_______ Epoca {} _______".format(contEpocas+1))
        maxCambio = 0.0
        ecm = 0

        # 2. Presentar cada x_i
        for i in range(len(xTrain)):
            # Inicializar la salida de la red a 0 para que no acumule entre epocas
            for j in range(len(salida)):
                salida[j].inicializar(0.0)

            # 3. Activar las neuronas de entrada
            for j in range(len(entrada)-1):
                entrada[j].inicializar(float(xTrain[i][j]))
                entrada[j].disparar()
                entrada[j].propagar()
            entrada[-1].inicializar(1.0) # Bias
            entrada[-1].disparar()
            entrada[-1].propagar()
            
            # 4. Calcular la respuesta de la neurona de salida
            for j in range(len(salida)):
                salida[j].disparar()
                print("Dispara ", salida[j].f_x)
                
            # 5. Ajustar pesos
            for j in range(len(salida)):
                error = float(yTrain[i][j]) - salida[j].f_x
                ecm += error**2

                for k in range(len(entrada)):
                    for c in entrada[k].conexiones:
                        if c.neurona == salida[j]:
                            # nuevo = antiguo + alpha * (target - y) * x_i
                            x_i = 1.0 if k == len(entrada)-1 else float(xTrain[i][k])
                            c.peso_anterior = c.peso
                            cambio = c.peso + x_i * alpha * error

                            if abs((cambio-c.peso)) > maxCambio:
                                maxCambio = abs(cambio-c.peso)
                            
                            c.peso = cambio
        # 6. Si el cambio de peso más grande es menor que una tolerancia, salir
        print("ECM :{:.4f}".format(ecm/(len(salida) * len(xTrain))))
        if maxCambio < tolerancia: 
            break
        
    # Lo ponemos para clasificacion
    for y in salida:
        y.tipo = TiposNeurona.ADALINE_TEST
        y.umbral = umbral

    print("Adaline construido: ")
    for x in range(len(entrada)-1):
        cont = 0
        for c in entrada[x].conexiones:
            print("w_"+str(cont) + str(x) +" = {:.2f}".format(c.peso))
            cont += 1
    
    cont = 0
    for c in entrada[-1].conexiones:
        print("Bias_" + str(cont) +" = {:.2f}".format(c.peso))
        cont += 1

    return adaline


def perceptron(xTrain, yTrain, paciencia=100, umbral=0.0, alpha=1.0):
    # Sacamos nAtr y nClases
    nAtrs = len(xTrain[0])
    nClases = len(yTrain[0])

    perceptron = RedNeuronal()

    # En perceptron, al haber bias, el umbral es 0

    # Capa de entrada con bias
    capaEntrada = Capa()

    entrada = []
    for i in range(0, nAtrs):
        entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA))
    entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias

    # Capa de salida
    capaSalida = Capa()
    salida = [] # Cada neurona predice 1 clase

    for i in range(nClases):
        salida.append(Neurona(umbral = 0.0, tipo=TiposNeurona.PERCEPTRON, salida_activa=1.0, salida_inactiva=-1.0)) 

    # Creadas las neuronas

    # Conectamos las neuronas entre ellas con los pesos inicialmente a 0
    for i in range(0, len(entrada)):
        for j in range(0, len(salida)):
            entrada[i].conectar(salida[j], 0.0)
        capaEntrada.añadir(entrada[i])
    
    for j in range(0, len(salida)):
        capaSalida.añadir(salida[j])

    # Conectadas y añadidas a las capas

    perceptron.añadir(capaEntrada)
    perceptron.añadir(capaSalida)

    # return

    # Red construida
    # Comienza el algoritmo iterativo

    # 1. Presentar patrones hasta que cumpla la condicion de parada
    for contEpocas in range(0, paciencia+1): # Epocas
        print("_______ Epoca {} _______".format(contEpocas+1))
        contAciertos = 0
        ecm = 0

        # 2. Presentar cada x_i
        for i in range(len(xTrain)):
            # Inicializar la salida de la red a 0 para que no acumule entre epocas
            for j in range(0, len(salida)):
                salida[j].inicializar(0.0)

            # 3. Activar las neuronas de entrada 
            for j in range(0, len(entrada)-1):
                entrada[j].inicializar(float(xTrain[i][j]))
                entrada[j].disparar()
                entrada[j].propagar()
            entrada[-1].inicializar(1.0) # Bias
            entrada[-1].disparar()
            entrada[-1].propagar()
            
            # 4. Calcular la respuesta de la neurona de salida
            error = False
            for j in range(0, len(salida)):
                salida[j].disparar()

                if salida[j].f_x != float(yTrain[i][j]):
                    error = True
                    break
                else:
                    contAciertos += 1
            # 5. Ajustar pesos si ha habido error
            ecm += (float(yTrain[i][j]) - salida[j].f_x)**2

            for k in range(0, len(entrada)):
                for c in entrada[k].conexiones:
                    for j in range(0, len(salida)):
                        if error:
                            if c.neurona == salida[j]:
                                c.peso_anterior = c.peso
                                c.peso += entrada[k].f_x * float(yTrain[i][j]) * alpha

        # 6. Si no han cambiado los pesos, salir del bucle while
        print("ECM :{:.4f}".format(ecm/(len(salida) * len(xTrain))))
        if (contAciertos == len(xTrain)*len(salida)): 
            print("Converge")
            break
    print("Score del perceptron train = {:.2f}%".format(contAciertos/(len(xTrain)*len(salida))*100))
    print("Perceptron construido: ")
    for x in range(len(entrada)-1):
        cont = 0
        for c in entrada[x].conexiones:
            print("w_"+str(cont) + str(x) +" = {:.2f}".format(c.peso))
            cont += 1
    
    cont = 0
    for c in entrada[-1].conexiones:
        print("Bias_" + str(cont) +" = {:.2f}".format(c.peso))
        cont += 1
        
        
    return perceptron

def score(red, xTest, yTest):
    score = 0
    entrada = red.capas[0].neuronas
    salida = red.capas[-1].neuronas

    # Para cada patron a clasificar...
    for i in range(len(xTest)):
        # Reinicializar salidas para que no acumulen entre patrones
        for j in range(len(salida)):
            salida[j].inicializar(0.0)

        # Activar neuronas xi
        for j in range(0, len(entrada)-1):
            entrada[j].inicializar(float(xTest[i][j]))
            entrada[j].disparar()
            entrada[j].propagar()
        entrada[-1].disparar()
        entrada[-1].propagar()
    
        # Calcular la respuesta de las neuronas de salida
        for j in range(len(salida)):
            salida[j].disparar()

            if salida[j].f_x == float(yTest[i][j]):
                score += 1
    
    score = score/(len(xTest)*len(salida))*100
    print("Score en test = {:.2f}%".format(score))
    
    return score

def predToFile(red, xTest, yTest, file):
    with open(file, 'w') as f:
        entrada = red.capas[0].neuronas
        salida = red.capas[-1].neuronas

        # Para cada patron a clasificar...
        for i in range(len(xTest)):
            # Reinicializar salidas para que no acumulen entre patrones
            for j in range(len(salida)):
                salida[j].inicializar(0.0)

            # Activar neuronas xi
            for j in range(0, len(entrada)-1):
                f.write(xTest[i][j] + " ")
                entrada[j].inicializar(float(xTest[i][j]))
                entrada[j].disparar()
                entrada[j].propagar()
            entrada[-1].disparar()
            entrada[-1].propagar()
        
            # Calcular la respuesta de las neuronas de salida
            for j in range(len(salida)):
                salida[j].disparar()
                f.write(str(int(salida[j].f_x)) + " ")
            f.write("\n")
                
    