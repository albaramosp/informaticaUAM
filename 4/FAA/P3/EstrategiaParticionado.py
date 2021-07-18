from abc import ABCMeta,abstractmethod
import random
from Datos import Datos


class Particion():

  # Esta clase mantiene la lista de indices de Train y Test para cada particion del conjunto de particiones  
  def __init__(self):
    self.indicesTrain=[]
    self.indicesTest=[]
  
  def setIndices(self, idxTrain, idxTest):
    self.indicesTrain = idxTrain
    self.indicesTest = idxTest

#####################################################################################################

class EstrategiaParticionado:
  
  # Clase abstracta
  __metaclass__ = ABCMeta

  # Atributos: deben rellenarse adecuadamente para cada estrategia concreta. Se pasan en el constructor 
  
  @abstractmethod
  # TODO: esta funcion deben ser implementadas en cada estrategia concreta  
  def creaParticiones(self,datos,seed=None):
    pass
  

#####################################################################################################

class ValidacionSimple(EstrategiaParticionado):
  def __init__(self, proporcionTest = 30, numeroEjecuciones = 1):
    self.proporcionTest = proporcionTest
    self.numeroEjecuciones = numeroEjecuciones
    self.particiones=[]
  
  # Crea particiones segun el metodo tradicional de division de los datos segun el porcentaje deseado y 
  # el numero de ejecuciones deseado.
  # Devuelve una lista de particiones (clase Particion)
  # TODO: implementar
  def creaParticiones(self, datos, seed=None):
    self.particiones=[]
    random.seed(seed)
    filas = datos.datos.shape[0]
    indices = list(range(filas))

    proporcion = (self.proporcionTest * filas) // 100

    for i in range(self.numeroEjecuciones):
      # Permutación los indices de los datos para evitar sesgos si estos están ordenados
      random.shuffle(indices)

      particion = Particion()
      particion.setIndices(indices[proporcion:], indices[:proporcion])
      
      self.particiones.append(particion)

      
#####################################################################################################      
class ValidacionCruzada(EstrategiaParticionado):
  def __init__(self, k = 10):
    self.numeroParticiones = k
    self.particiones=[]
  
  # Crea particiones segun el metodo de validacion cruzada.
  # El conjunto de entrenamiento se crea con las nfolds-1 particiones y el de test con la particion restante
  # Esta funcion devuelve una lista de particiones (clase Particion)
  # TODO: implementar
  def creaParticiones(self, datos, seed=None):
    self.particiones=[]   
    random.seed(seed)
    filas = datos.datos.shape[0]
    indices = list(range(filas))

    proporcion = filas // self.numeroParticiones

    # Permutación los indices de los datos para evitar sesgos si estos están ordenados
    random.shuffle(indices)

    for i in range(self.numeroParticiones):
      # El conjunto de Train = conjuto Original - conjunto de Test
      idxTrain = indices.copy()
      particion = Particion()
      
      if i == self.numeroParticiones-1:
        idxTest = indices[i*proporcion:]
        del idxTrain[i*proporcion:]
      else: 
        idxTest = indices[i*proporcion:(i+1)*proporcion]
        del idxTrain[i*proporcion:(i+1)*proporcion]
      
      particion.setIndices(idxTrain, idxTest)
      self.particiones.append(particion)
