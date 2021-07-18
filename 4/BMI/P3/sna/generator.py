import networkx as nx
import matplotlib.pyplot as plt

def main():
    n = 2000  
    p = 0.1 # probability of making friends
    g_er = nx.erdos_renyi_graph(n, p) # Erdos Renyi

    #print(g_er.edges())

    print(len(g_er.edges()))

    with open("./graph/er.csv", "w") as f:
        for pair in g_er.edges():
            f.write(str(pair[0]) + "," + str(pair[1]) + "\n")
    # return
    # nx.draw(g_er)
    # plt.savefig("g_er.png")

    # n = 2000 
    # m = 3
    # g_ba = nx.barabasi_albert_graph(n, m) # Barabasi Albert

    # print(len(g_ba.edges()))
    # with open("./graph/ba.csv", "w") as f:
    #     for pair in g_ba.edges():
    #         f.write(str(pair[0]) + "," + str(pair[1]) + "\n")

    # nx.draw(g_ba)
    # plt.savefig("g_ba.png")
    

main()