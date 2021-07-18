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
      print("python3 main4_2.py -p")
      sys.exit(0)

    elif(argv == "-ejecuta_per"):
      print("Ejecutado ejemplo: python3 main4_2.py -p")
      xTrain, yTrain, xTest, yTest = leer3("problema_real2.txt", "problema_real2_no_etiquetados.txt")
      p = perceptron(xTrain, yTrain, paciencia=15, alpha=1)
      predToFile(p, xTest, yTest, "prediccion_perceptron.txt")

    elif(argv == "-ayuda_ada"):
      print("python3 main4_2.py -a")
      sys.exit(0)

    elif(argv == "-ejecuta_ada"):
      print("Ejecutado ejemplo: python3 main4_2.py -a")
      xTrain, yTrain, xTest, yTest = leer3("problema_real2.txt", "problema_real2_no_etiquetados.txt")
      a = adaline(xTrain, yTrain, paciencia=50, alpha=0.1, tolerancia=0.05)
      predToFile(a, xTest, yTest, "prediccion_adaline.txt")
      sys.exit(0)

    elif(argv == "-p"):
        # Hacemos validacion aleatoria k = 10
        xTrain, yTrain, xTest, yTest = leer3("problema_real2.txt", "problema_real2_no_etiquetados.txt")
        # listScores = []

        # for i in range(10):
        #   xTrain2, yTrain2, xTest2, yTest2 = leer1("problema_real2.txt", 70)
        #   p = perceptron(xTrain2, yTrain2, paciencia=15, alpha=1)
        #   listScores.append(score(p, xTest2, yTest2))

        # print("Score en val = ",
        #       str("{:.4f}%".format(np.mean(listScores))) + " ± " +
        #       str("{:.4f}".format(np.std(listScores))))

        p = perceptron(xTrain, yTrain, paciencia=15, alpha=1)
        predToFile(p, xTest, yTest, "prediccion_perceptron.txt")
    
    elif(argv == "-a"):
        # Hacemos validacion aleatoria k = 10
        xTrain, yTrain, xTest, yTest = leer3("problema_real2.txt", "problema_real2_no_etiquetados.txt")
        # listScores = []

        # for i in range(10):
        #   xTrain2, yTrain2, xTest2, yTest2 = leer1("problema_real2.txt", 70)
        #   a = adaline(xTrain2, yTrain2, paciencia=50, alpha=0.1, tolerancia=0.05)
        #   listScores.append(score(a, xTest2, yTest2))

        # print("Score en val = ",
        #       str("{:.4f}%".format(np.mean(listScores))) + " ± " +
        #       str("{:.4f}".format(np.std(listScores))))
        a = adaline(xTrain, yTrain, paciencia=50, alpha=0.1, tolerancia=0.05)
        predToFile(a, xTest, yTest, "prediccion_adaline.txt")

    else:
      print("El comando",argv,"no se ha encontrado")
      sys.exit(0)
    