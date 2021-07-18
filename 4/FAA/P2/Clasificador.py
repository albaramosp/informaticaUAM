from abc import ABCMeta,abstractmethod
import numpy as np
import math
import statistics
import Datos

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
     
    
  # TODO: implementar
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
            #print(self.tablasAtributos[j])  
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

    # Version K-NN v.2.0: normaliza y guarda los valores de media y desviación de cada atributo
    if (self.normalizar):
      for i in range(datostrain.shape[1]):
        media = np.mean(datostrain[:, i])
        desviacion = np.std(datostrain[:, i])

        datostrain[:, i] -= media
        datostrain[:, i] /= desviacion

        self.medias.append(media)
        self.desviaciones.append(desviacion)
    
    
  def clasifica(self,datostest,atributosDiscretos,diccionario):
    predicciones = []

    if (self.normalizar):
      for i in range(datostest.shape[1]):
        datostest[:, i] -= self.medias[i]
        datostest[:, i] /= self.desviaciones[i]
    
    for fila in datostest:
      for dato in self.



    
    return predicciones
