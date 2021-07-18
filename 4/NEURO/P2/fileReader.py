from random import shuffle
from math import floor
from sklearn import preprocessing
import numpy as np

def leer1(filename, trainPerc, normalize=False):
    with open(filename, 'r') as f:
        reader = f.readlines()
        f.close()

        firstRow = reader[0].rstrip().split()
        nAtrs = int(firstRow[0])
        nClases = int(firstRow[1])
        reader = reader[1:] # Ya sin la primera linea

        shuffle(reader)
        idx = floor(trainPerc * len(reader) / 100)

        cont = 0
        
        xTrain = []
        yTrain = []
        xTest = []
        yTest = []

        for line in reader:
            line = line.rstrip().split()
            
            if cont <= idx:
                xTrain.append((line[:nAtrs]))
                yTrain.append((line[nAtrs:]))
            else:
                xTest.append((line[:nAtrs]))
                yTest.append((line[nAtrs:]))
            
            cont += 1

        if normalize:
            X = np.array(xTrain + xTest)
            scaler = preprocessing.StandardScaler().fit(X)

            xTrain = scaler.transform(np.array(xTrain)).tolist()
            xTest = scaler.transform(np.array(xTest)).tolist()

        for i in range(len(yTrain)):
            for j in range(len(yTrain[i])):
                if yTrain[i][j] == '0':
                    yTrain[i][j] = '-1'
        
        for i in range(len(yTest)):
            for j in range(len(yTest[i])):
                if yTest[i][j] == '0':
                    yTest[i][j] = '-1'

        return (xTrain, yTrain, xTest, yTest)


def leer2(filename):
    # Requiere un solo fichero donde los datos servirán como entrenamiento y test
    with open(filename, 'r') as f:
        reader = f.readlines()
        f.close()

        firstRow = reader[0].rstrip().split()
        nAtrs = int(firstRow[0])
        nClases = int(firstRow[1])
        reader = reader[1:] # Ya sin la primera linea

        x = []
        y = []

        for line in reader:
            line = line.rstrip().split()
            
            x.append((line[:nAtrs]))
            y.append((line[nAtrs:]))

        for i in range(len(y)):
            for j in range(len(y[i])):
                if y[i][j] == '0':
                    y[i][j] = '-1'
        
        return (x, y)


def leer3(filenameTrain, filenameTest):    
    xTrain, yTrain = leer2(filenameTrain)
    xTest, yTest = leer2(filenameTest)

    if(len(xTrain[0]) != len(xTest[0]) or len(yTrain[0]) != len(yTest[0])):
        raise Exception("Los ficheros no pertenencen al mismo dataset")

    return (xTrain, yTrain, xTest, yTest)