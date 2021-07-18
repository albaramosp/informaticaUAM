from fileReader import *
from redneuronal import *
from redneuronal import perceptron
from capa import Capa
from neurona import Neurona, TiposNeurona
import argparse
import sys
import getopt, sys
from random import uniform

if __name__ == "__main__":
  # Get full command-line arguments
  fullCmdArguments = sys.argv

  # Keep all but the first
  argumentList = fullCmdArguments[1:]

  if not argumentList:
    print("Argumentos:")
    print("\t-f: Fichero con los datos de la red")
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
      print("python3 main3.py -f NombreDelFichero -p")
      sys.exit(0)

    elif(argv == "-ejecuta_per"):
      print("Ejecutado ejemplo: python3 main3.py -f and.txt -p")
      xTrain, yTrain = leer2("and.txt")
      perceptron(xTrain, yTrain)
      sys.exit(0)

    elif(argv == "-ayuda_ada"):
      print("python3 main3.py -f NombreDelFichero -a")
      sys.exit(0)

    elif(argv == "-ejecuta_ada"):
      print("Ejecutado ejemplo: python3 main3.py -f and.txt -a")
      xTrain, yTrain = leer2("and.txt")
      adaline(xTrain, yTrain)
      sys.exit(0)

    elif(argv == "-f"):
        fileIndex = argumentList.index("-f") + 1
        typeIndex = argumentList.index("-f") + 2
        if len(argumentList) > fileIndex:
            xTrain, yTrain = leer2(argumentList[fileIndex])

            if len(argumentList) > typeIndex  and argumentList[typeIndex] == "-a":
                adaline(xTrain, yTrain)

            elif len(argumentList) > typeIndex  and argumentList[typeIndex] == "-p":
                perceptron(xTrain, yTrain)
            else:
                print("No se ha encontrado tipo de red a ejecutar")

        else:
            print("No se ha encontrado nombre del fichero")
        sys.exit(0)
    else:
      print("El comando",argv,"no se ha encontrado")
      sys.exit(0)