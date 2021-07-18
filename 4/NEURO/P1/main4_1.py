from fileReader import *
from redneuronal import *
from capa import Capa
from neurona import Neurona, TiposNeurona
import argparse
import sys
import getopt, sys


if __name__ == "__main__":
  # Get full command-line arguments
  fullCmdArguments = sys.argv

  # Keep all but the first
  argumentList = fullCmdArguments[1:]

  if not argumentList:
    print("Argumentos:")
    print("Argumentos:")
    print("\t-p: Ejecución como Perceptrón")
    print("\t-a: Ejecución como Adaline")
    print("\t-compilar: Compila lo necesario para ejecutar el programa (No hace nada)")
    print("\t-ayuda_per: Explicación de los argumentos necesario para ejecutar el programa (Perceptrón)")
    print("\t-ejecuta_per: Ejemplo de ejecución (Perceptrón)")
    print("\t-ayuda_ada: Explicación de los argumentos necesario para ejecutar el programa (Adaline)")
    print("\t-ejecuta_ada: Ejemplo de ejecución (Adaline)")
    sys.exit(0)


  for argv in argumentList:
    if(argv == "-compilar"):
      print("Soy un programa en python, no necesito compilar")
      sys.exit(0)

    elif(argv == "-ayuda_per"):
      print("python3 main4_1.py -p")
      sys.exit(0)

    elif(argv == "-ejecuta_per"):
      print("Ejecutado ejemplo: python3 main4_1.py -p")
      xTrain, yTrain, xTest, yTest = leer1("problema_real1.txt", 70)
      p = perceptron(xTrain, yTrain, paciencia=15, alpha=0.5)
      sys.exit(0)

    elif(argv == "-ayuda_ada"):
      print("python3 main4_1.py -a")
      sys.exit(0)

    elif(argv == "-ejecuta_ada"):
      print("Ejecutado ejemplo: python3 main4_1.py -a")
      xTrain, yTrain, xTest, yTest = leer1("problema_real1.txt", 70)
      a = adaline(xTrain, yTrain, paciencia=20, alpha=0.2, tolerancia=0.05)
      sys.exit(0)

    elif(argv == "-p"):
        xTrain, yTrain, xTest, yTest = leer1("problema_real1.txt", 70)
        p = perceptron(xTrain, yTrain, paciencia=15, alpha=0.5)
        scoreP = score(p, xTest, yTest)
    
    elif(argv == "-a"):
        xTrain, yTrain, xTest, yTest = leer1("problema_real1.txt", 70)
        a = adaline(xTrain, yTrain, paciencia=15, alpha=0.5, tolerancia=0.05)
        scoreA = score(a, xTest, yTest)

    else:
      print("El comando",argv,"no se ha encontrado")
      sys.exit(0)
    