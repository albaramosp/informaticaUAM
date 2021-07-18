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

# TODO: pasarle los yTrain ya en codificacion bipolar
def perceptronMulticapa(xTrain, yTrain, neuronasOcultas=2, paciencia=100, alpha=1.0, tolerancia = None, validateData = False, xVal = None, yVal = None):
    ecmFile = open("ECM.dat", "a")
    evalFile = open("EVAL.dat", "a")

    # Sacamos nAtr y nClases
    nAtrs = len(xTrain[0])
    nClases = len(yTrain[0])

    perceptron = RedNeuronal()

    # Capa de entrada con bias
    capaEntrada = Capa()

    entrada = []
    for i in range(0, nAtrs):
        entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA))
    entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias capa de entrada

    # Capa oculta
    capaOculta = Capa()
    oculta = []

    for i in range(neuronasOcultas):
        oculta.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SIGMOIDE_BIPOLAR)) 
    oculta.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias capa oculta

    # Capa de salida
    capaSalida = Capa()
    salida = [] # Cada neurona predice 1 clase

    for i in range(nClases):
        salida.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SIGMOIDE_BIPOLAR, salida_activa=1.0, salida_inactiva=-1.0)) 

    # Creadas las neuronas
    # Conectamos las neuronas de entrada con las de la capa oculta
    
    # Paso 0: Inicializar todos los pesos y sesgos (valores aleatorios pequeños).
    for i in range(0, len(entrada)):
        for j in range(0, len(oculta) - 1):
            peso = uniform(-0.5, 0.5)
            entrada[i].conectar(oculta[j], peso)
        capaEntrada.añadir(entrada[i])

    for j in range(0, len(oculta)):
        for k in range(0, len(salida)):
            peso = uniform(-0.5, 0.5)
            oculta[j].conectar(salida[k], peso)
        capaOculta.añadir(oculta[j])
    
    for k in range(0, len(salida)):
        capaSalida.añadir(salida[k])

    # Conectadas y añadidas a las capas

    perceptron.añadir(capaEntrada)
    perceptron.añadir(capaOculta)
    perceptron.añadir(capaSalida)

    # Red construida
    # Comienza el algoritmo iterativo
    # Paso 1: Mientras que la condición de parada sea falsa, ejecutar pasos 2-9:
    for contEpocas in range(0, paciencia): # Epocas
        # print("_______ Epoca {} _______".format(contEpocas+1)) 
        # Paso 2: Para cada par de entrenamiento, ejecutar los pasos 3-8:
        for fila in range(len(xTrain)):
            # Inicializar la salida y capa oculta a 0 para que no acumulen entre patrones ni epocas
            for k in range(0, len(salida)):
                salida[k].inicializar(0.0)

            for j in range(0, len(oculta)):
                oculta[j].inicializar(0.0)

            ###############
            # Feedforward #
            ###############
            
            # Paso 3: Establecer las activaciones a las neuronas de entrada
            for i in range(0, len(entrada)-1):
                entrada[i].inicializar(float(xTrain[fila][i]))
                entrada[i].disparar()
                #print("La neurona de entrada ",i," dispara ", entrada[i].f_x)
                entrada[i].propagar()
            entrada[-1].inicializar(1.0) # Bias
            entrada[-1].disparar()
            entrada[-1].propagar()
    
            # Paso 4: Calcular la respuesta de las neuronas de la capa oculta:
            for j in range(0, len(oculta) - 1):
                oculta[j].disparar()
                #print("La neurona oculta ",j," dispara ", oculta[j].f_x)
                oculta[j].propagar()
            oculta[-1].disparar() # Bias
            oculta[-1].propagar()

            # Paso 5: Calcular la respuesta de las neuronas de salida:
            for k in range(0, len(salida)):
                salida[k].disparar()
                #print("La salida ",k," dispara ", salida[k].f_x)
            
            ###################
            # BackPropagation #
            ###################
            
            # Retropropagación del error
            # Paso 6: Cada neurona de salida (Y k , k=1...m) recibe un patrón objetivo
            # que corresponde al patrón de entrada de entrenamiento, calcula
            # el error: δ k = (t k – y k )f’(y_in k ) ,
            # calcula su corrección de peso (utilizada para actualizar w jk más
            # tarde): ∆w jk = αδ k z j ,
            # calcula su corrección de sesgo (utilizada para actualizar w 0k más
            # tarde): ∆w 0k = αδ k
            # y envía δ k a las neuronas de la capa anterior.
            deltas_oculta = [0] * (len(oculta)-1) # Esto me lo voy a ir guardando desde ya en este mismo paso

            for k in range(0, len(salida)):
                # δ_k = (t_k – y_k )f’(y_in_k )
                delta = (float(yTrain[fila][k]) - salida[k].f_x) * TiposNeurona.derivada_sigmoide_bipolar(salida[k].valor)
                # print("Entrada: ", xTrain[fila], yTrain[fila])
                # print("Tiene que predecir: ", yTrain[fila][k], " predice: ", salida[k].f_x)
                # Correcciones de peso y deltas de capa oculta
                for j in range(0, len(oculta) - 1):
                    for c in oculta[j].conexiones:
                        if c.neurona == salida[k]:
                            # Paso 7: Cada neurona de la capa oculta (Z j , j=1...p) suma sus entradas
                            # δ_ in_j = ∑ δ_k w_jk
                            deltas_oculta[j] += delta * c.peso

                            # Ya que estoy te actualizo los pesos
                            # ∆w_jk = α * δ_k * z_j
                            c.peso_anterior = c.peso
                            c.peso += alpha * delta * (oculta[j].f_x)
                
                # Corrección de sesgo
                for c in oculta[-1].conexiones:
                    if c.neurona == salida[k]:
                        c.peso_anterior = c.peso
                        # ∆w_0k = α * δ_k
                        c.peso += alpha * delta
                    
            # Paso 7: Cada neurona de la capa oculta (Z j , j=1...p) suma sus entradas
            # delta (de las neuronas de la capa posterior)
            # m
            # δ_ in_j = ∑ δ k w jk
            # k = 1
            # lo multiplica por la derivada de su función de activación para
            # calcular el error: δ j = δ_in j f’(z_in j ) ,
            # calcula su corrección de peso (utilizada para actualizar v ij más
            # tarde): ∆v ij = αδ j x i ,
            # y calcula su corrección de sesgo para actualizar v 0j más tarde):
            for j in range(0, len(oculta)-1):
                # δ_j = δ_in_j * f’(z_in_j)
                delta = deltas_oculta[j] * TiposNeurona.derivada_sigmoide_bipolar(oculta[j].valor)

                for i in range(0, len(entrada) - 1):
                    for c in entrada[i].conexiones:
                        if c.neurona == oculta[j]:
                            # Actualizo los pesos
                            # ∆v_ij = α * δ_j * x_i 
                            c.peso_anterior = c.peso
                            c.peso += alpha * delta * (entrada[i].f_x)
                
                # Corrección de sesgo
                for c in entrada[-1].conexiones:
                    if c.neurona == oculta[j]:
                        c.peso_anterior = c.peso
                        c.peso += alpha * delta

        # Paso 9: Comprobar la condición de parada
        # Tenemos diferentes condiciones de parada:
        #   1.  Si declaramos un float para la variable tolerancia,
        #       se parará cuando el ECM sea menor o igual que el marcado
        #   2.  Si declaramos val como True, tenemos que pasar xVal e yVal
        #       se parara si el ECM de validacion empieza a aumentar
        #   3.  Se agote la paciencia

        ECM = 0
        for fila in range(len(xTrain)):
            # Inicializar la salida de la red a 0 para que no acumule entre patrones ni epocas
            for k in range(0, len(salida)):
                salida[k].inicializar(0.0)

            for j in range(0, len(oculta)):
                oculta[j].inicializar(0.0)

            
            for i in range(0, len(entrada)-1):
                entrada[i].inicializar(float(xTrain[fila][i]))
                entrada[i].disparar()
                entrada[i].propagar()
            entrada[-1].inicializar(1.0) # Bias
            entrada[-1].disparar()
            entrada[-1].propagar()

            for j in range(0, len(oculta) - 1):
                oculta[j].disparar()
                oculta[j].propagar()
            oculta[-1].disparar() # Bias
            oculta[-1].propagar()

            sumatorio = 0
            for k in range(0, len(salida)):
                salida[k].disparar()
                sumatorio += pow((float(yTrain[fila][k]) - salida[k].f_x), 2)

            ECM += (1/2) * sumatorio

        ECM = ECM / len(xTrain)
        #print("ECM ", ECM)
        ecmFile.write(str(ECM)+"\n")

        if tolerancia != None:
            if ECM < tolerancia:
                break

        
        if validateData:
            ECMVal = 0

            for fila in range(len(xVal)):
                # Inicializar la salida de la red a 0 para que no acumule entre patrones ni epocas
                for k in range(0, len(salida)):
                    salida[k].inicializar(0.0)

                for j in range(0, len(oculta)):
                    oculta[j].inicializar(0.0)

                
                for i in range(0, len(entrada)-1):
                    entrada[i].inicializar(float(xVal[fila][i]))
                    entrada[i].disparar()
                    entrada[i].propagar()
                entrada[-1].inicializar(1.0) # Bias
                entrada[-1].disparar()
                entrada[-1].propagar()

                for j in range(0, len(oculta) - 1):
                    oculta[j].disparar()
                    oculta[j].propagar()
                oculta[-1].disparar() # Bias
                oculta[-1].propagar()

                sumatorio = 0
                for k in range(0, len(salida)):
                    salida[k].disparar()
                    sumatorio += pow((float(yVal[fila][k]) - salida[k].f_x), 2)

                ECMVal += (1/2) * sumatorio

            ECMVal = ECMVal / len(xVal)
            #print("ECMVal ", ECMVal)
            evalFile.write(str(ECMVal)+"\n")

            if contEpocas == 0:
                lastECMVal = ECMVal
            else:
                if lastECMVal >= ECMVal:
                    lastECMVal = ECMVal
                else:
                    break
    
    cont = 1
    for c in entrada[-1].conexiones:
        #print("v_0" + str(cont) +" = {:.4f}".format(c.peso))
        cont += 1
    
    for x in range(len(entrada)-1):
        cont = 1
        for c in entrada[x].conexiones:
            #print("v_"+ str(x+1) + str(cont) + " = {:.4f}".format(c.peso))
            cont += 1

    cont = 1
    for c in oculta[-1].conexiones:
        #print("w_0" + str(cont) +" = {:.4f}".format(c.peso))
        cont += 1
    
    for x in range(len(oculta)-1):
        cont = 1
        for c in oculta[x].conexiones:
            #print("w_" + str(x+1) + str(cont) +" = {:.4f}".format(c.peso))
            cont += 1
    return perceptron, contEpocas

def ejemploClasePerceptronMulticapa(xTrain, yTrain, neuronasOcultas=2, paciencia=0, alpha=0.25):
    # Sacamos nAtr y nClases
    nAtrs = len(xTrain[0])
    nClases = len(yTrain[0])

    perceptron = RedNeuronal()

    # Capa de entrada con bias
    capaEntrada = Capa()

    entrada = []
    for i in range(0, nAtrs):
        entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA))
    entrada.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias capa de entrada

    # Capa oculta
    capaOculta = Capa()
    oculta = []

    for i in range(neuronasOcultas):
        oculta.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SIGMOIDE_BIPOLAR)) 
    oculta.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SESGO)) # bias capa oculta

    # Capa de salida
    capaSalida = Capa()
    salida = [] # Cada neurona predice 1 clase

    for i in range(nClases):
        salida.append(Neurona(umbral = 0.0, tipo=TiposNeurona.SIGMOIDE_BIPOLAR, salida_activa=1.0, salida_inactiva=-1.0)) 

    # Creadas las neuronas
    # Conectamos las neuronas de entrada con las de la capa oculta
    
    # Paso 0: Inicializar todos los pesos y sesgos (valores aleatorios pequeños).
    entrada[0].conectar(oculta[0], 0.7)
    entrada[0].conectar(oculta[1], -0.4)
    entrada[1].conectar(oculta[0], -0.2)
    entrada[1].conectar(oculta[1], 0.3)
    entrada[-1].conectar(oculta[0], 0.4)
    entrada[-1].conectar(oculta[1], 0.6)
    capaEntrada.añadir(entrada[0])
    capaEntrada.añadir(entrada[1])
    capaEntrada.añadir(entrada[-1])

    oculta[0].conectar(salida[0], 0.5)
    oculta[1].conectar(salida[0], 0.1)
    oculta[-1].conectar(salida[0], -0.3)

    capaOculta.añadir(oculta[0])
    capaOculta.añadir(oculta[1])
    capaOculta.añadir(oculta[-1])
    
    for k in range(0, len(salida)):
        capaSalida.añadir(salida[k])

    # Conectadas y añadidas a las capas

    perceptron.añadir(capaEntrada)
    perceptron.añadir(capaOculta)
    perceptron.añadir(capaSalida)


    # Red construida
    # Comienza el algoritmo iterativo
    # Paso 1: Mientras que la condición de parada sea falsa, ejecutar pasos 2-9:
    for contEpocas in range(0, paciencia+1): # Epocas
        print("_______ Epoca {} _______".format(contEpocas+1)) 
        # Paso 2: Para cada par de entrenamiento, ejecutar los pasos 3-8:
        for fila in range(len(xTrain)):
            # TODO: inicializar capa oculta para que no acumule??
            # Inicializar la salida de la red a 0 para que no acumule entre epocas
            for k in range(0, len(salida)):
                salida[k].inicializar(0.0)

            for j in range(0, len(oculta)):
                oculta[j].inicializar(0.0)

            ###############
            # Feedforward #
            ###############
            
            # Paso 3: Establecer las activaciones a las neuronas de entrada
            for i in range(0, len(entrada)-1):
                entrada[i].inicializar(float(xTrain[fila][i]))
                entrada[i].disparar()
                print("La neurona de entrada ",i," dispara ", entrada[i].f_x)
                entrada[i].propagar()
            entrada[-1].inicializar(1.0) # Bias
            entrada[-1].disparar()
            print("La sesgo de entrada dispara ", entrada[-1].f_x)
            entrada[-1].propagar()
    
            # TODO: aqui posiblemente la estes cagando
            # Paso 4: Calcular la respuesta de las neuronas de la capa oculta:
            for j in range(0, len(oculta) - 1):
                oculta[j].disparar()
                print("La neurona oculta ",j, " in: ", oculta[j].valor, " dispara ", oculta[j].f_x)
                oculta[j].propagar()
            oculta[-1].disparar() # Bias
            oculta[-1].propagar()

            # Paso 5: Calcular la respuesta de las neuronas de salida:
            for k in range(0, len(salida)):
                salida[k].disparar()
                print("La salida ",k," dispara ", salida[k].f_x)
            
            ###################
            # BackPropagation #
            ###################
            
            # Retropropagación del error
            # Paso 6: Cada neurona de salida (Y k , k=1...m) recibe un patrón objetivo
            # que corresponde al patrón de entrada de entrenamiento, calcula
            # el error: δ k = (t k – y k )f’(y_in k ) ,
            # calcula su corrección de peso (utilizada para actualizar w jk más
            # tarde): ∆w jk = αδ k z j ,
            # calcula su corrección de sesgo (utilizada para actualizar w 0k más
            # tarde): ∆w 0k = αδ k
            # y envía δ k a las neuronas de la capa anterior.
            deltas_oculta = [0] * (len(oculta)-1) # Esto me lo voy a ir guardando desde ya en este mismo paso

            for k in range(0, len(salida)):
                # δ_k = (t_k – y_k )f’(y_in_k )
                delta = (float(yTrain[fila][k]) - salida[k].f_x) * TiposNeurona.derivada_sigmoide_bipolar(salida[k].valor)
                print("Entrada: ", xTrain[fila], yTrain[fila])
                print("Tiene que predecir: ", yTrain[fila][k], " predice: ", salida[k].f_x)
                print("delta k", k, " = ", delta)
                # Correcciones de peso y deltas de capa oculta
                for j in range(0, len(oculta) - 1):
                    for c in oculta[j].conexiones:
                        if c.neurona == salida[k]:
                            # Paso 7: Cada neurona de la capa oculta (Z j , j=1...p) suma sus entradas
                            # δ_ in_j = ∑ δ_k w_jk
                            deltas_oculta[j] += delta * c.peso

                            # Ya que estoy te actualizo los pesos
                            # ∆w_jk = α * δ_k * z_j
                            c.peso_anterior = c.peso
                            c.peso += alpha * delta * (oculta[j].f_x)
                
                # Corrección de sesgo
                for c in oculta[-1].conexiones:
                    if c.neurona == salida[k]:
                        c.peso_anterior = c.peso
                        # ∆w_0k = α * δ_k
                        c.peso += alpha * delta
                    
            # Paso 7: Cada neurona de la capa oculta (Z j , j=1...p) suma sus entradas
            # delta (de las neuronas de la capa posterior)
            # m
            # δ_ in_j = ∑ δ k w jk
            # k = 1
            # lo multiplica por la derivada de su función de activación para
            # calcular el error: δ j = δ_in j f’(z_in j ) ,
            # calcula su corrección de peso (utilizada para actualizar v ij más
            # tarde): ∆v ij = αδ j x i ,
            # y calcula su corrección de sesgo para actualizar v 0j más tarde):
            for j in range(0, len(oculta)-1):
                # δ_j = δ_in_j * f’(z_in_j)
                delta = deltas_oculta[j] * TiposNeurona.derivada_sigmoide_bipolar(oculta[j].valor)
                print("delta j_in", j, " = ", deltas_oculta[j])
                print("delta j", j, " = ", delta)

                for i in range(0, len(entrada) - 1):
                    for c in entrada[i].conexiones:
                        if c.neurona == oculta[j]:
                            # Actualizo los pesos
                            # ∆v_ij = α * δ_j * x_i 
                            c.peso_anterior = c.peso
                            c.peso += alpha * delta * (entrada[i].f_x)
                
                # Corrección de sesgo
                for c in entrada[-1].conexiones:
                    if c.neurona == oculta[j]:
                        c.peso_anterior = c.peso
                        c.peso += alpha * delta

        # Paso 9: Comprobar la condición de parada
        # Si el error cuadratico decreció y vuelve a comenzar a crecer, parar
        # Si no ha habido un decrecimiento del ECM por x épocas, parar.
        # for fila in range(len(xTrain)):

    cont = 1
    for c in entrada[-1].conexiones:
        print("v_0" + str(cont) +" = {:.4f}".format(c.peso))
        cont += 1
    
    for x in range(len(entrada)-1):
        cont = 1
        for c in entrada[x].conexiones:
            print("v_"+ str(x+1) + str(cont) + " = {:.4f}".format(c.peso))
            cont += 1

    cont = 1
    for c in oculta[-1].conexiones:
        print("w_0" + str(cont) +" = {:.4f}".format(c.peso))
        cont += 1
    
    for x in range(len(oculta)-1):
        cont = 1
        for c in oculta[x].conexiones:
            print("w_" + str(x+1) + str(cont) +" = {:.4f}".format(c.peso))
            cont += 1
    
    
    
    return perceptron


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


def confusionMatrix(red, xTest, yTest):
    score = 0
    entrada = red.capas[0].neuronas
    oculta = red.capas[1].neuronas
    salida = red.capas[-1].neuronas

    FalsePositive = 0
    FalseNegative = 0
    TruePositive = 0
    TrueNegative = 0

    # Para cada patron a clasificar...
    for i in range(len(xTest)):
        # Reinicializar salidas para que no acumulen entre patrones
        for k in range(len(salida)):
            salida[k].inicializar(0.0)

        # Reinicializar capas ocultas para que no acumulen entre patrones
        for j in range(0, len(oculta)):
            oculta[j].inicializar(0.0)

        # Activar neuronas xi
        for j in range(0, len(entrada)-1):
            entrada[j].inicializar(float(xTest[i][j]))
            entrada[j].disparar()
            entrada[j].propagar()
        entrada[-1].disparar()
        entrada[-1].propagar()

        for j in range(0, len(oculta) - 1):
            oculta[j].disparar()
            oculta[j].propagar()
        oculta[-1].disparar() # Bias
        oculta[-1].propagar()

        # Paso 5: Calcular la respuesta de las neuronas de salida:
        #print("Entrada: ", xTest[i], yTest[i])
        for k in range(0, len(salida)):
            salida[k].disparar()
            #print("Tiene que predecir: ", yTest[i][k], " predice: ", salida[k].f_x)
            if salida[k].f_x > 0 and float(yTest[i][k]) > 0:
                TruePositive += 1
            elif salida[k].f_x >= 0 and float(yTest[i][k]) < 0:
                FalsePositive += 1
            elif salida[k].f_x <= 0 and float(yTest[i][k]) > 0:
                FalseNegative += 1
            elif salida[k].f_x < 0 and float(yTest[i][k]) < 0:
                TrueNegative += 1
    
    matrizConfusion = np.array([[TruePositive, FalsePositive], [FalseNegative, TrueNegative]])
    score = (TruePositive+TrueNegative)/(TrueNegative+TruePositive+FalsePositive+FalseNegative)*100
    return score, matrizConfusion


def predToFile(red, xTest, yTest, file):
    with open(file, 'w') as f:
        entrada = red.capas[0].neuronas
        oculta = red.capas[1].neuronas
        salida = red.capas[-1].neuronas

        # Para cada patron a clasificar...
        for i in range(len(xTest)):
            # Reinicializar salidas para que no acumulen entre patrones
            for k in range(len(salida)):
                salida[k].inicializar(0.0)

            # Reinicializar capas ocultas para que no acumulen entre patrones
            for j in range(0, len(oculta)):
                oculta[j].inicializar(0.0)

            # Activar neuronas xi
            for j in range(0, len(entrada)-1):
                f.write(xTest[i][j] + " ")
                entrada[j].inicializar(float(xTest[i][j]))
                entrada[j].disparar()
                print("La neurona de entrada ",j," dispara ", entrada[j].f_x)
                entrada[j].propagar()
            entrada[-1].disparar()
            entrada[-1].propagar()

            for j in range(0, len(oculta) - 1):
                oculta[j].disparar()
                print("La neurona oculta ",j," dispara ", oculta[j].f_x)
                oculta[j].propagar()
            oculta[-1].disparar() # Bias
            oculta[-1].propagar()

            # Paso 5: Calcular la respuesta de las neuronas de salida:
            for k in range(0, len(salida)):
                salida[k].disparar()
                print("La salida ",k," dispara ", salida[k].f_x)
                f.write(str((salida[k].f_x)) + " ")
            f.write("\n")

def predToFileMulticapa(red, xTest, yTest, file):
    with open(file, 'w') as f:
        entrada = red.capas[0].neuronas
        oculta = red.capas[1].neuronas
        salida = red.capas[-1].neuronas

        # Para cada patron a clasificar...
        for i in range(len(xTest)):
            # Reinicializar salidas para que no acumulen entre patrones
            for k in range(len(salida)):
                salida[k].inicializar(0.0)

            # Reinicializar capas ocultas para que no acumulen entre patrones
            for j in range(0, len(oculta)):
                oculta[j].inicializar(0.0)

            # Activar neuronas xi
            for j in range(0, len(entrada)-1):
                f.write(xTest[i][j] + " ")
                entrada[j].inicializar(float(xTest[i][j]))
                entrada[j].disparar()
                entrada[j].propagar()
            entrada[-1].disparar()
            entrada[-1].propagar()

            for j in range(0, len(oculta) - 1):
                oculta[j].disparar()
                oculta[j].propagar()
            oculta[-1].disparar() # Bias
            oculta[-1].propagar()

            # Paso 5: Calcular la respuesta de las neuronas de salida:
            for k in range(0, len(salida)):
                salida[k].disparar()
                if salida[k].f_x > 0:
                    f.write("1 ")
                elif salida[k].f_x < 0:
                    f.write("-1 ")
                else:
                   f.write("0 ") 
            f.write("\n")
                
    