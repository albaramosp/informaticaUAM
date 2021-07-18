import pandas as pd
import numpy as np

class Datos:
  
  def __init__(self, nombreFichero, dtype = {}):
    self.datos = None # Numpy array containing the dataset in numerical format
    self.diccionario = {} # Mapping of nominal values to their corresponding numeric ones for each attribute
    self.nominalAtributos = [] # Indicates whether an atribute of the dataset is nominal (true) or float/int (false)

    df = pd.read_csv(nombreFichero, dtype=dtype)
    datos = np.array(df) # Create Numpy bidimensional array from dataframe

    columnNames = []
    for i in df.head():
      columnNames.append(i)

    # Create array nominalAtributos
    for i in df.dtypes:
      if i == np.object: # np.object indicates a nominal atribute
        self.nominalAtributos.append(True)
      elif i == np.float64 or i == np.int64:
        self.nominalAtributos.append(False)
      else:
        raise ValueError("Incorrect data type in the dataset")

    i = 0
    # Creates dictionary for each attribute
    for isNominalAttr in self.nominalAtributos:
      auxDict = {}

      # If the attribute is nominal, match values in lexicological order
      if isNominalAttr is True:
        auxSet = set() # Stores the attribute's nominal values
        l = 0 # Counter for lexicographical order

        for dato in datos:
          auxSet.add(dato[i]) # Stores each row's value for the current attribute

        for d in sorted(auxSet):
          auxDict[d] = l
          l += 1
      self.diccionario[columnNames[i]] = auxDict
      i+=1
      
      

    # Change the nominal data in the dataframe to its numerical value 
    self.datos = np.array(df.replace(self.diccionario))


  def extraeDatos(self, idx):
    return self.datos[idx]

