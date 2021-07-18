"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""
import datetime
import time

from sna import *


def main():
    test_network("graph/small1.csv", ",", 5, 6, 4, int)
    test_network("graph/small2.csv", ",", 5, 3, 5, int)
    test_network("graph/small3.csv", ",", 5, "a", "b")
    test_network("graph/facebook_combined.txt", " ", 5, 9, 3, int)
    test_network("graph/twitter.csv", ",", 5, "el_pais", "ElviraLindo")
    test_network("graph/barabasi.csv", ",", 5, 1, 2, int)
    test_network("graph/erdos.csv", ",", 5, 1, 2, int)


def test_network(file, delimiter, topn, u, v, parse=0):
    print("==================================================\nTesting " + file + " network")
    network = UndirectedSocialNetwork(file, delimiter=delimiter, parse=parse)
    print(len(network.users()), "users and", network.nedges(), "contact relationships")
    print("User", u, "has", network.degree(u), "contacts")

    # Métricas de usuarios
    print("-------------------------")
    test_metric(UserClusteringCoefficient(topn), network, u)

    # Métricas de arcos
    print("-------------------------")
    test_metric(Embeddedness(topn), network, (u, v))

    # Métricas globales de red
    print("-------------------------")
    test_global_metric(ClusteringCoefficient(), network)
    test_global_metric(AvgUserMetric(UserClusteringCoefficient()), network)
    test_global_metric(Assortativity(), network)


def test_metric(metric, network, example):
    start = time.process_time()
    print(metric, ":", metric.compute_all(network))
    print(str(metric) + "(" + str(example) + ") =", metric.compute(network, example))
    timer(start)


def test_global_metric(metric, network):
    start = time.process_time()
    print(metric, "=", metric.compute_all(network))
    timer(start)


def timer(start):
    print("--> elapsed time:", datetime.timedelta(seconds=round(time.process_time() - start)), "<--")


main()