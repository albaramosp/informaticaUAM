from abc import ABCMeta,abstractmethod
import numpy as np
import pandas as pd
import math
import statistics
import Datos
from scipy.spatial import distance
import random
from sklearn.preprocessing import OneHotEncoder
from collections import Counter
import copy

class Clasificador:
  
  # Clase abstracta
  __metaclass__ = ABCMeta
  
  # Metodos abstractos que se implementan en casa clasificador concreto
  @abstractmethod
  # TODO: esta funcion debe ser implementada en cada clasificador concreto
  # datosTrain: matriz numpy con los datos de entrenamiento
  # atributosDiscretos: array bool con la indicatriz de los atributos nominales
  # diccionario: array de diccionarios de la estructura Datos utilizados para la codificacion de variables discretas
  def entrenamiento(self,datosTrain,atributosDiscretos,diccionario):
    pass
  
  
  @abstractmethod
  # TODO: esta funcion debe ser implementada en cada clasificador concreto
  # devuelve un numpy array con las predicciones
  def clasifica(self,datosTest,atributosDiscretos,diccionario):
    pass
  
  
  # Obtiene el numero de aciertos y errores para calcular la tasa de fallo
  # Aqui se compara la prediccion (pred) con las clases reales y se calcula el error
  def error(self,datos,pred):
    error = 0
    FalsePositive = 0
    FalseNegative = 0
    TruePositive = 0
    TrueNegative = 0

    # Calculo del error y creacion de tabla de confusion con 2 clases
    for i in range(len(datos)):
      if datos[i][-1] != pred[i]:
        error += 1
        if self.classValues[0] == pred[i]:
          FalsePositive += 1
        else:
          FalseNegative += 1
      else:
        if self.classValues[0] == pred[i]:
          TruePositive += 1
        else:
          TrueNegative += 1
  
    self.FP.append(FalsePositive)
    self.FN.append(FalseNegative)
    self.TP.append(TruePositive)
    self.TN.append(TrueNegative)

    return error/len(datos)
  
  # Realiza una clasificacion utilizando una estrategia de particionado determinada
  def validacion(self,particionado,dataset,clasificador,seed=None):
    # Creamos las particiones siguiendo la estrategia llamando a particionado.creaParticiones
    # - Para validacion cruzada: en el bucle hasta nv entrenamos el clasificador con la particion de train i
    # y obtenemos el error en la particion de test i
    # - Para validacion simple (hold-out): entrenamos el clasificador con la particion de train
    # y obtenemos el error en la particion test. Otra opci�n es repetir la validaci�n simple un n�mero especificado de veces, obteniendo en cada una un error. Finalmente se calcular�a la media.
    particionado.creaParticiones(dataset, seed)
    errores = []
    self.FP = []
    self.FN = []
    self.TP = []
    self.TN = []

    for i in particionado.particiones:
      datosTrain = dataset.extraeDatos(i.indicesTrain)
      datosTest = dataset.extraeDatos(i.indicesTest)

      clasificador.entrenamiento(datosTrain, dataset.nominalAtributos, dataset.diccionario)
      predicciones = clasificador.clasifica(datosTest, dataset.nominalAtributos, dataset.diccionario)
      
      errores.append(self.error(datosTest, predicciones))
      
    matrizConfusion = np.array([[statistics.mean(self.TP), statistics.mean(self.FP)], [statistics.mean(self.FN), statistics.mean(self.TN)]])

    return errores, matrizConfusion

##############################################################################

class ClasificadorNaiveBayes(Clasificador):
  def __init__(self, laplace = False):
    self.laplace = laplace

  def entrenamiento(self,datostrain,atributosDiscretos,diccionario):
    self.tablasAtributos = []
    self.tablaAPriori = {}

    self.classValues, counts = np.unique(datostrain[:, -1], return_counts=True)
    numClases = len(self.classValues)

    nFilas = len(datostrain)

    # Probabilidades a priori de la hipotesis
    for i in range(numClases):
      self.tablaAPriori[self.classValues[i]] = counts[i]/nFilas
      
    # Tablas de cada atributo
    count = 0
    aux = 0
    for key, i in diccionario.items():
      if(count == len(diccionario) -1):
        break
        
      if atributosDiscretos[count]:
        # Atributo discreto
        tabla = np.zeros((len(i), numClases))

        for fila in datostrain:
          tabla[int(fila[count]), np.where(self.classValues == fila[-1])] += 1
                  
        if self.laplace and np.any(tabla == 0):
          tabla += 1
                  
      else:
        # Atributo continuo
        tabla = np.zeros((2, numClases)) # Filas: media y desviacion estandar
        countClases = 0

        for j in range(numClases):
          media = np.mean(datostrain[(datostrain[:, -1] == self.classValues[countClases]), count])
          varianza = np.var(datostrain[(datostrain[:, -1] == self.classValues[countClases]), count])
          tabla[0][j] = media
          tabla[1][j] = varianza

          countClases += 1

      count += 1
      self.tablasAtributos.append(tabla)
     
    
  def clasifica(self,datostest,atributosDiscretos,diccionario):
    predicciones = []

    for fila in datostest:
      # Calculamos PRODj [(P(Xj|Hi)*P(Hi))] para cada clase
      prodHi = {}

      contClases = 0
      for i in self.tablaAPriori: 
        pHi = self.tablaAPriori[i] # P(Hi)

        j = 0
        prod = pHi
        while j < len(fila) - 1:
          if atributosDiscretos[j]:
            ejsClase = sum(self.tablasAtributos[j][:, contClases])
            prod *= self.tablasAtributos[j][int(fila[j])][contClases] / ejsClase # P(X1|Hi) * P(X2|Hi) * ...

          else:
            op1 = (1/(math.sqrt(2*math.pi*self.tablasAtributos[j][1][contClases])))
            op2 =math.exp((-(fila[j]-self.tablasAtributos[j][0][contClases]))/(2*self.tablasAtributos[j][1][contClases]))
            prod *= op1*op2

          j += 1
        
        prodHi[i] = prod
        contClases += 1
       
      predicciones.append(max(prodHi, key=prodHi.get)) # Decision = argmax_Hi PRODj [(P(Xj|Hi)*P(Hi))]
      
    return predicciones


#########################################################################################


# La variable distancia puede tomar los valores "euclidea", "manhattan" o "mahalanobis"
class ClasificadorVecinosProximos(Clasificador):
  def __init__(self, distancia = "euclidea", k = 5, normalizar = True):
    self.distancia = distancia
    self.k = k
    self.normalizar = normalizar

  def entrenamiento(self,datostrain,atributosDiscretos,diccionario):
    self.medias = []
    self.desviaciones = []

    # Version K-NN v.1.0: si no hay que normalizar entonces no hace nada salvo guardar datostrain
    self.datostrain = datostrain
    self.classValues, counts = np.unique(datostrain[:, -1], return_counts=True)

    # Version K-NN v.2.0: normaliza y guarda los valores de media y desviación de cada atributo
    if (self.normalizar):
      for i in range(datostrain.shape[1] - 1):
        media = np.mean(datostrain[:, i])
        desviacion = np.std(datostrain[:, i])

        datostrain[:, i] -= media
        datostrain[:, i] /= desviacion

        self.medias.append(media)
        self.desviaciones.append(desviacion)
    
    
  def clasifica(self,datostest,atributosDiscretos,diccionario):
    predicciones = []

    if (self.normalizar):
      for i in range(datostest.shape[1] - 1):
        datostest[:, i] -= self.medias[i]
        datostest[:, i] /= self.desviaciones[i]
    
    # Se calcula la matriz de covarianza solo cuando usamos la distancia Mahalanobis
    if self.distancia == "mahalanobis":
      cov = np.cov(np.array(datostest[:,:-1].T))
      vi = np.linalg.inv(cov)
    
    for fila in datostest:
      distancias = []
      
      for dato in self.datostrain:
        if self.distancia == "euclidea":
          distancias.append([distance.euclidean(fila[:-1], dato[:-1]), dato[-1]])
        elif self.distancia == "manhattan":
          distancias.append([distance.cityblock(fila[:-1], dato[:-1]), dato[-1]])
        elif self.distancia == "mahalanobis":
          distancias.append([distance.mahalanobis(fila[:-1], dato[:-1], vi), dato[-1]])
      
      distancias.sort()
      calcModa = []
      for i in distancias[:self.k]:
        calcModa.append(i[1])
      
      c = Counter(calcModa)
      predicciones.append((c.most_common(1)[0])[0])
    return predicciones
       
##############################################################################################

class ClasificadorRegresionLogistica(Clasificador):
  def __init__(self, cteAprendizaje = 1, nEpocas = 1, normalizar = True):
    self.nEpocas = nEpocas
    self.cteAprendizaje = cteAprendizaje
    self.normalizar = normalizar

  def entrenamiento(self,datostrain,atributosDiscretos,diccionario):
    self.w = [] # w0, w1, ... , w_d
    self.classValues, counts = np.unique(datostrain[:, -1], return_counts=True)
    self.medias = []
    self.desviaciones = []

    # Inicializar w_i aleatoriamente entre [-0.5, 0.5]
    for i in range(len(atributosDiscretos)):
      self.w.append(random.uniform(-0.5, 0.5))

    if (self.normalizar):
      for i in range(datostrain.shape[1] - 1):
        media = np.mean(datostrain[:, i])
        desviacion = np.std(datostrain[:, i])

        datostrain[:, i] -= media
        datostrain[:, i] /= desviacion

        self.medias.append(media)
        self.desviaciones.append(desviacion)
    
    for i in range(self.nEpocas):
      for dato in datostrain:
        x = [1]
        x.extend(dato[:-1]) # Añadimos x0=1
        xw = np.dot(x, self.w) # Producto escalar
        sigma = self.sigmoidal(xw) # sigma(x*w)
        producto = self.cteAprendizaje * (sigma - dato[-1])
        self.w = [self.w[j] - [i * producto for i in x][j] for j in range(len(self.w))]


  def clasifica(self,datostest,atributosDiscretos,diccionario):
    predicciones = []

    if (self.normalizar):
      for i in range(datostest.shape[1] - 1):
        datostest[:, i] -= self.medias[i]
        datostest[:, i] /= self.desviaciones[i]

    for dato in datostest:
      x = [1]
      x.extend(dato[:-1]) # Añadimos x0=1
      xw = np.dot(x, self.w) # Producto escalar

      if xw < 0:
        predicciones.append(0)
      else:
        predicciones.append(1)
    
    return predicciones

  
  def sigmoidal(self, x):
    try:
      aux = 1/(1+np.exp(-x))
      return aux
    except Exception:
      return 0
  
    
class ClasificadorGenetico(Clasificador):
  def __init__(self, nReglas, tamPoblacion, nEpocas, datos, elitismo = 0.05, pCruce=1, pMutacion=0.1, randomPred=False):
    self.nReglas = nReglas # Reglas de cada individuo
    self.tamPoblacion = tamPoblacion # Probar entre 50 y 150 individuos
    self.nEpocas = nEpocas # Probar entre 100 y 200
    self.elitismo = elitismo # Porcentaje de la poblacion pasa directamente a siguiente generacion
    self.pCruce = pCruce
    self.pMutacion = pMutacion
    self.poblacion = [] # Conjunto de individuos, y cada individuo es un conjunto de reglas
    self.mejorIndividuo = [] # Durante el entrenamiento nos quedaremos al mejor de la poblacion
    self.tamCromosoma = 0
    self.randomPred = randomPred

    df = pd.DataFrame(datos.datos, columns=datos.diccionario.keys())
    nombres_atrs = list(df.columns)
    nombres_atrs.remove("Class")
    X = df[nombres_atrs].values
    self.enc = OneHotEncoder(sparse=False, dtype=np.int)
    self.enc.fit(X)

  def creaPoblacion(self, diccionario):
    self.tamCromosoma = 0

    # Calcular el tamaño de un cromosoma: sum(posibilidades de cada atr)
    for key, value in diccionario.items(): 
      if(key == "Class"):
          self.tamCromosoma += 1
      else:
          self.tamCromosoma += len(value)

    # Generar poblacion
    for i in range(self.tamPoblacion):
      individuo=[]
      for j in range(random.randrange(1, self.nReglas+1)): # Entero en = [1, nReglas+1)
        regla=[]
        for k in range(self.tamCromosoma):
          regla.append(random.choice([0, 1]))
        individuo.append(regla)
      self.poblacion.append(individuo)

      # Generar vector aleatorio para cruce uniforme
      self.uniforme = []
      for i in range(self.tamCromosoma):
        self.uniforme.append(random.choice([0, 1]))
        
  def cruzarIndividuos(self, individuo1, individuo2):
    if(len(individuo1) > len(individuo2)):
      reglas = len(individuo2)
    else:
      reglas = len(individuo1)

    for regla in range(reglas):
      bit = 0
      # print(individuo1[regla])
      # print(individuo2[regla])
      for i in self.uniforme:
        if(i == 1):
          aux = individuo1[regla][bit]
          individuo1[regla][bit] = individuo2[regla][bit]
          individuo2[regla][bit] = aux
        bit+=1

  def cruzarPoblacion(self):
    individuo1 = []
    individuo2 = []
    i = 0
    for individuo in self.poblacion:
      # Proporcional a self.pCruce, cruzar individuos según la probabilidad
      if (self.pCruce > np.random.random()): # Cambiado el orden de >
        if individuo1 == []:
          individuo1 = individuo
        elif individuo2 == []:
          individuo2 = individuo
        if(individuo1 != [] and individuo2 != []):
          self.cruzarIndividuos(individuo1, individuo2)
          individuo1 = []
          individuo2 = []

  def mutarIndividuo(self, individuo):
    # Se realiza un bit flip en cada cromosoma
    for regla in range(len(individuo)):
      for bit in range(self.tamCromosoma):
        if (self.pMutacion > np.random.random()):
          if individuo[regla][bit] == 1:
            individuo[regla][bit] = 0
          else:
            individuo[regla][bit] = 1

  
  def mutarPoblacion(self):
    for individuo in self.poblacion:
        self.mutarIndividuo(individuo)

  def fitness(self, datos, diccionario, individuo, predicciones=False):
    # Codificamos como OneHot cada fila de datos para tenerla en el formato de las reglas
    df = pd.DataFrame(datos, columns=diccionario.keys())
    nombres_atrs = list(df.columns)
    nombres_atrs.remove("Class")
    X = df[nombres_atrs].values
    # self.enc = OneHotEncoder(sparse=False, dtype=np.int)
    # self.enc.fit(X)
    X2 = self.enc.transform(X)

    resultados = []
    aciertos = 0
    numFila = 0
    for fila in X2:
      predicciones_validas = []
      for regla in individuo:
        # Comprobar cada una de las reglas del individuo si se activa o no con las filas de datos
        limInfRango = 0
        for key, value in diccionario.items():
          if key == "Class":
            # Guardamos la predicción de la regla si es válida
            predicciones_validas.append(regla[-1])
            continue

          longitudAtr = len(value)
          comparacion = fila[limInfRango:longitudAtr+limInfRango] & regla[limInfRango:longitudAtr+limInfRango]
          if(1 not in comparacion):
            break

          limInfRango += longitudAtr

      # Si se activa, mirar si predice correctamente o no
      # Si hay varias que se activan, elegir la clase mayoritaria
      # En caso de empate, error
      # Cogemos la moda de las predicciones y comprobamos si predice bien, si hay empate (salta excepción) fallo
      try:
        prediccion = statistics.mode(predicciones_validas)
        if predicciones:
          resultados.append(prediccion)
        else:
          if prediccion == datos[numFila,-1]:
            aciertos+=1
      except:
        if self.randomPred:
          resultados.append(random.choice([0, 1]))
        else:
          resultados.append(-1)

      numFila += 1

    if predicciones: 
      return resultados
    
    else:
      return aciertos/numFila
  
  def entrenamiento(self,datostrain,atributosDiscretos,diccionario):
    self.scoresIndividuos = []
    self.scoresMedios = []
    
    self.creaPoblacion(diccionario)
    mejorIndividuo = None
    self.classValues, counts = np.unique(datostrain[:, -1], return_counts=True)

    for i in range(self.nEpocas):
      # Seleccion
      nueva_poblacion = []
      fitnessIndividuos = []
      sumaFitness = 0
      for individuo in self.poblacion:
        fitnessIndividuo = self.fitness(datostrain, diccionario, individuo)
        sumaFitness += fitnessIndividuo
        # Se guarda tupla fitness - individuo
        fitnessIndividuos.append([fitnessIndividuo, individuo])
      # Primero los mejoress
      # print(sumaFitness)
      fitnessIndividuos.sort(reverse=True)
      mejorIndividuo = fitnessIndividuos[0]
      self.scoresIndividuos.append(mejorIndividuo[0])
      self.scoresMedios.append(sumaFitness/self.tamPoblacion)
      # print("Score del mejor individuo de la epoca: " + str(mejorIndividuo[0]))
      # print("Score medio de la epoca: " + str(sumaFitness/self.tamPoblacion))
      tamElitismo = int(self.tamPoblacion * self.elitismo)
      tamNuevaPoblacion = tamElitismo

      # Seleccion proporcional a fitness
      while tamNuevaPoblacion < self.tamPoblacion:
        for individuo in fitnessIndividuos:
          if individuo[0]/sumaFitness > np.random.random():
            nueva_poblacion.append(individuo[1])
            tamNuevaPoblacion += 1
            if tamNuevaPoblacion == self.tamPoblacion:
              break


      # Actualizamos la población
      self.poblacion = copy.deepcopy(nueva_poblacion)
      
      # Cruzar
      self.cruzarPoblacion()
      # Mutar
      self.mutarPoblacion()

      # Agregamos los individuos elite a la población
      
      for individuo in fitnessIndividuos[:tamElitismo]:
        self.poblacion.append(individuo[1])

    # Elegir mejor individuo
    print("Acaba el entrenamiento, score del mejor individuo: " + str(mejorIndividuo[0]))
    self.mejorIndividuo = mejorIndividuo[1]
    
    
  def clasifica(self,datostest,atributosDiscretos,diccionario):
    return self.fitness(datostest, diccionario, self.mejorIndividuo, predicciones=True)
