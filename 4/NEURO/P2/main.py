from fileReader import *
from redneuronal import *
from redneuronal import perceptron
from capa import Capa
from neurona import Neurona, TiposNeurona
import argparse
import sys
import getopt, sys
from random import uniform
import numpy
from statistics import mean

if __name__ == "__main__":
  # Get full command-line arguments
  fullCmdArguments = sys.argv

  # Keep all but the first
  argumentList = fullCmdArguments[1:]

  if not argumentList:
    print("Argumentos:")
    print("\t-f: Fichero con los datos de la red")
    print("\t-n: Normalizar los datos leidos del fichero de datos")
    print("\t-compilar: Compila lo necesario para ejecutar el programa (No hace nada)")
    print("\t-ayuda_retro: Explicación de los argumentos necesarios para ejecutar el programa")
    print("\t-ejecuta_problema_real2: Ejecuta el problema real 2 y devuelve las predicciones en prediccion_problema_real2.txt")
    print("\t-ejecuta_problema_real6: Ejecuta el problema real 6")
    sys.exit(0)

  for argv in argumentList:
    if(argv == "-compilar"):
      print("Soy un programa en python, no necesito compilar")
      sys.exit(0)

    elif(argv == "-ayuda_retro"):
      print("python3 main.py -f NombreDelFichero -n")
      sys.exit(0)

    elif(argv == "-ejecuta_problema_real2"):
      xTrain, yTrain, xTest, yTest = leer3("problema_real2.txt", "problema_real2_no_etiquetados.txt")
      p, epoca = perceptronMulticapa(xTrain, yTrain, alpha=0.1, neuronasOcultas=7, paciencia=50, tolerancia = 0.01)
      predToFileMulticapa(p, xTest, yTest, "prediccion_problema_real2.txt")
      sys.exit(0)

    elif(argv == "-ejecuta_problema_real6"):
      xTrain, yTrain, xTest, yTest = leer1("problema_real6.txt", 70, normalize=True)
      print()
      p, epoca = perceptronMulticapa(xTrain, yTrain, alpha=0.1, neuronasOcultas=20, paciencia=5000)
      score, matrizConfusion = confusionMatrix(p, xTest, yTest)
      print("Matriz con 20 neuronas: y alpha 0.1 = : ")
      print(matrizConfusion)
      print("Score medio en test = {:.2f}%".format(score))

      sys.exit(0)     

    elif(argv == "-f"):
        fileIndex = argumentList.index("-f") + 1
        normalizeIndex = argumentList.index("-f") + 2
        if len(argumentList) > fileIndex:
            
            # Validacion aleatoria k=10
            xTrain = [0 for i in range(10)] 
            yTrain = [0 for i in range(10)]
            xTest = [0 for i in range(10)]
            yTest = [0 for i in range(10)]

            
            try:
              if argumentList[normalizeIndex] == "-n":
                for i in range(10):
                    xTrain[i], yTrain[i], xTest[i], yTest[i] = leer1(argumentList[fileIndex], 70, normalize=True)
              else:
                for i in range(10):
                    xTrain[i], yTrain[i], xTest[i], yTest[i] = leer1(argumentList[fileIndex], 70)

            except:
              for i in range(10):
                    xTrain[i], yTrain[i], xTest[i], yTest[i] = leer1(argumentList[fileIndex], 70)

            # Variando las neuronas ocultas
            for n in range(1, 10, 2):
                # Variando alpha
                for a in [0.01, 0.1, 0.5, 1.0]:
                  listScores = []
                  listEpocas = []
                  for i in range(10):
                    p, epoca = perceptronMulticapa(xTrain[i], yTrain[i], alpha=a, neuronasOcultas=n, paciencia=20000, tolerancia = 0.01, validateData=True, xVal = xTest[i], yVal = yTest[i])
                    score, matrizConfusion = confusionMatrix(p, xTest[i], yTest[i])
                    listEpocas.append(epoca)
                    listScores.append(score)
                  print("Matriz con ambas condiciones de parada, ", n, "  neuronas: y alpha ", a, " = : ")
                  print("De media a tardado en entrenanar = {:.2f} epocas".format(mean(listEpocas)))
                  print("Score medio en test = {:.2f}%".format(mean(listScores)))
            
        else:
            print("No se ha encontrado nombre del fichero")
        sys.exit(0)
    else:
      print("El comando",argv,"no se ha encontrado")
      sys.exit(0)