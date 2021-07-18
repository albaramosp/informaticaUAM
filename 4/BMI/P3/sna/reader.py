from collections import OrderedDict

def main():
    nodos_grados = {}

    with open("./graph/ba.csv", "r") as f:
        lines = f.readlines()

        for line in lines:
            line = line.split('\n')[0].split(',')

            try:
                nodos_grados[line[0]] += 1
            except:
                nodos_grados[line[0]] = 1
            
            try:
                nodos_grados[line[1]] += 1
            except:
                nodos_grados[line[1]] = 1

    # Ya tenemos cada nodo con su grado
    grados_cantidad = {}
    for k in nodos_grados.keys():
        #print("Nodo ", k, " tiene grado ",nodos_grados[k])
        try:
            grados_cantidad[nodos_grados[k]] += 1
        except:
            grados_cantidad[nodos_grados[k]] = 1
    
    # Ya tenemos cada grado con la cantidad de nodos que lo tienen
    grados_probas = {}
    total_nodos = len(nodos_grados.keys())
    grados_cantidad = OrderedDict(sorted(grados_cantidad.items()))
    with open("./graph/grados_ba.txt", "w") as f:
        for k in grados_cantidad.keys():
            # grados_probas[k] = grados_cantidad[k] / total_nodos
            f.write(str(k) + " " +  str(grados_cantidad[k]) + "\n")


main()