###############################################################################
#  Prácticas BMI Javier Lougedo y Alba Ramos                                  #
#  Práctica 1: Implementacion de un pequeño buscador con Whoosy               #
#  Documento: Librerias adicionales empleadas                                 #
###############################################################################


-----> LIBRERIAS ADICIONALES <-----


Además de las dos librerias comentadas, Whooshy y BeautifulSoup, se utilizan:


ZIPFILE
    Sirve para, como en el ejemplo del output.txt, poder acceder a los ficheros 
    comprimidos sin necesidad de descomprimirlos. Su instalación es vía el comando:
                    
                    pip install zipfileXY

    Siendo XY la version de Python del sistema (3.6 sería 36, la 3.8, 38, etc)

MATPLOTLIB
    Sirve para dibujar las gráficas de estadísticas de los términos. En principio no hace
    falta instalarla, ya que las gráficas se incluyen en la memoria, pero si se desea
    se puede descomentar la línea 52 del main.py y generarlas.
    Su instalación es vía el comando:
                    
                    pip install matplotlib
