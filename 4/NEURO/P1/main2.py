import argparse
import sys

import getopt, sys

from redneuronal import RedNeuronal
from capa import Capa
from neurona import Neurona, TiposNeurona

def red_3_x_3(file):
  redMcCulloghPitts = RedNeuronal()

  # Capa de entrada
  capaEntrada = Capa()

  x1 = Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA)
  x2 = Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA)
  x3 = Neurona(umbral = 0.0, tipo=TiposNeurona.DIRECTA)


  # Capa oculta con 3 neuronas
  capaOculta = Capa()

  z1 = Neurona(umbral = 2.0, tipo=TiposNeurona.MCCULLOCH)
  z2 = Neurona(umbral = 2.0, tipo=TiposNeurona.MCCULLOCH)
  z3 = Neurona(umbral = 2.0, tipo=TiposNeurona.MCCULLOCH)


  # Capa de salida
  capaSalida = Capa()
  y = Neurona(umbral = 2.0, tipo=TiposNeurona.MCCULLOCH) 

  # Creadas las neuronas

  # Conectamos las neuronas entre ellas
  x1.conectar(z1, 1.0)
  x1.conectar(z3, 1.0)

  x2.conectar(z1, 1.0)
  x2.conectar(z2, 1.0)

  x3.conectar(z2, 1.0)
  x3.conectar(z3, 1.0)

  z1.conectar(y, 2.0)
  z2.conectar(y, 2.0)
  z3.conectar(y, 2.0)

  # Conectadas

  # Añadimos las neuronas a las capas
  capaEntrada.añadir(x1)
  capaEntrada.añadir(x2)
  capaEntrada.añadir(x3)

  capaOculta.añadir(z1)
  capaOculta.añadir(z2)
  capaOculta.añadir(z3)

  capaSalida.añadir(y)

  # Añadidas a las respectivas capas

  redMcCulloghPitts.añadir(capaEntrada)
  redMcCulloghPitts.añadir(capaOculta)
  redMcCulloghPitts.añadir(capaSalida)

  # Red construida
  redMcCulloghPitts.inicializar() # Inicializa todas las neuronas con 0

  with open (file, 'r') as f:
    reader = f.readlines()
    with open("salida_" + file, "a") as out:
      out.write("x1\t" + 
                "x2\t" + 
                "x3\t" + 
                "z1\t" + 
                "z2\t" + 
                "z3\t" + 
                "y\n")
      out.close()

      for row in reader:
        row = row.rstrip()
        fields = row.split(' ')

        with open("salida_" + file, "a") as out:
          # Inicializar la capa inicial
          x1.inicializar(float(fields[0]))
          x2.inicializar(float(fields[1]))
          x3.inicializar(float(fields[2]))
          
          y.disparar()
          y.inicializar(float(0))

          z1.disparar()
          z2.disparar()
          z3.disparar()

          z1.propagar()
          z2.propagar()
          z3.propagar()

          z1.inicializar(float(0))
          z2.inicializar(float(0))
          z3.inicializar(float(0))

          x1.disparar()
          x2.disparar()
          x3.disparar()
          
          x1.propagar()
          x2.propagar()
          x3.propagar()

          out.write(str(int(x1.f_x)) + "\t" + 
                    str(int(x2.f_x)) + "\t" + 
                    str(int(x3.f_x)) + "\t" + 
                    str(int(z1.f_x)) + "\t" + 
                    str(int(z2.f_x)) + "\t" + 
                    str(int(z3.f_x)) + "\t" + 
                    str(int(y.f_x))  + "\n")

          out.close()

  # Los tiempos para la ultima fila leida
  for i in range(2):
    with open("salida_" + file, "a") as out:
        # Inicializar la capa inicial
        x1.inicializar(float(0))
        x2.inicializar(float(0))
        x3.inicializar(float(0))

        y.disparar()
        y.inicializar(float(0))

        z1.disparar()
        z2.disparar()
        z3.disparar()

        z1.propagar()
        z2.propagar()
        z3.propagar()

        z1.inicializar(float(0))
        z2.inicializar(float(0))
        z3.inicializar(float(0))

        x1.disparar()
        x2.disparar()
        x3.disparar()
        
        x1.propagar()
        x2.propagar()
        x3.propagar()

        out.write(str(int(x1.f_x)) + "\t" + 
                  str(int(x2.f_x)) + "\t" + 
                  str(int(x3.f_x)) + "\t" + 
                  str(int(z1.f_x)) + "\t" + 
                  str(int(z2.f_x)) + "\t" + 
                  str(int(z3.f_x)) + "\t" + 
                  str(int(y.f_x))  + "\n")
        out.close()


if __name__ == "__main__":
  # Get full command-line arguments
  fullCmdArguments = sys.argv

  # Keep all but the first
  argumentList = fullCmdArguments[1:]

  if not argumentList:
    print("Argumentos:")
    print("\t-f: Fichero con los datos de la red")
    print("\t-ayuda_mp: Explicación de los argumentos necesario para ejecutar el programa")
    print("\t-compila_mp: Compila lo necesario para ejecutar el programa (No hace nada)")
    print("\t-ejecuta_mp: Ejemplo de ejecución")
    sys.exit(0)

  for argv in argumentList:
    if(argv == "-ayuda_mp"):
      print("python3 main.py -f NombreDelFichero")
      sys.exit(0)
    elif(argv == "-compila_mp"):
      print("Soy un programa en python, no necesito compilar")
      sys.exit(0)
    elif(argv == "-ejecuta_mp"):
      print("Ejecutado ejemplo: python3 main.py -f figura1.data")
      red_3_x_3("figura1.data")
      sys.exit(0)
    elif(argv == "-f"):
      fileIndex = argumentList.index("-f") + 1
      if len(argumentList) > fileIndex:
        red_3_x_3(argumentList[fileIndex])
      else:
        print("No se ha encontrado nombre del fichero")
      sys.exit(0)
    else:
      print("El comando",argv,"no se ha encontrado")
    
  
  # if args.ayuda:
  #     print("python3 main.py -f NombreDelFichero")
  #     sys.exit(-1)

  # if args.ejecuta:
  #     print("Ejecutado ejemplo: python3 main.py -f figura1.data")
  #     red_3_x_3("figura1.data")
  #     sys.exit(0)

  # # Aquí procesamos lo que se tiene que hacer con cada argumento
  # if file is None :
  #     print("No se ha especificado fichero de datos")
  #     sys.exit(-1)
  # else:
  #   red_3_x_3(file)
    

  # Red que iremos construyendo
  

  # # TODO: Probamos distintas entradas, leemos, propagamos, etc.

  # # TODO: lectura y salida por ficheros


  # Modelo de antes
  # Disparar
  # x1.disparar()
  # x2.disparar()
  # x3.disparar()
  # print("Disparada capa entrada, que sabemos que siempre dispara, f_x(): x1=", x1.f_x, ", x2=", x2.f_x, ", x3=", x3.f_x)

  # # Propagar los valores
  # x1.propagar()
  # x2.propagar()
  # x3.propagar()
  # print("Propagados valores a capa oculta: z1=", z1.valor, ", z2=", z2.valor, ", z3=", z3.valor)

  # # Disparar
  # z1.disparar()
  # z2.disparar()
  # z3.disparar()
  # print("Disparada capa oculta, f_x(): z1=", z1.f_x, ", z2=", z2.f_x, ", z3=", z3.f_x)

  # # Propagar los valores
  # z1.propagar()
  # z2.propagar()
  # z3.propagar()
  # print("Propagados valores a capa de salida, y=", y.valor)
  
  # # Volvemos a inicializar los valores de z
  # z1.inicializar(float(0))
  # z2.inicializar(float(0))
  # z3.inicializar(float(0))

  # # Disparo final
  # y.disparar()
  # print("Disparada salida, f_x() = ", y.f_x)
  # # Volvemos a inicializar los valores de y
  # y.inicializar(float(0))
  # print("################################################################################################")