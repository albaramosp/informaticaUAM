"""
 Copyright (C) 2021 Pablo Castells, Alejandro Bellogín y Andrés Medina

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""

import itertools
import heapq
import math
import random
from abc import ABC, abstractmethod
from collections import OrderedDict


class Ratings:
    def __init__(self, file="", delim='\t', lines=None):
        if file != "":
            with open(file, "r") as f:
                lines = f.readlines()
            # {user1: {item1: rating, item2: rating...}, user2: {item1: rating...}...}
        self.ratings, self.ratingsByItem = self.lines_to_dict(lines, delim)

    def lines_to_dict(self, lines, delim='\t'):
        aux_ratings = OrderedDict()
        aux_ratingsByItem = OrderedDict()
        for line in lines:
            fields = line.split(delim)
            user = int(fields[0])
            item = int(fields[1])
            rating = float(fields[2])
            if user not in aux_ratings:
                aux_ratings[user] = {}
            if item not in aux_ratings[user]:
                aux_ratings[user][item] = rating
            if item not in aux_ratingsByItem:
                aux_ratingsByItem[item] = {}
            if user not in aux_ratingsByItem[item]:
                aux_ratingsByItem[item][user] = rating

        return aux_ratings, aux_ratingsByItem

    def dict_to_lines(self, aux_dict):
        lines = []
        for user in aux_dict:
            for item in aux_dict[user]:
                rating = aux_dict[user][item]
                lines.append("{}\t{}\t{}".format(user, item, rating))
        return lines

    def rate(self, user, item, rating):
        if user not in self.ratings:
            self.ratings[user] = {}

        if item not in self.ratings[user]:
            self.ratings[user][item] = rating

        if item not in self.ratingsByItem:
            self.ratingsByItem[item] = {}

        if user not in self.ratingsByItem[item]:
            self.ratingsByItem[item][user] = rating

    def rating(self, user, item):
        try:
            return self.ratings[user][item]
        except KeyError:
            return 0

    # Ratio entre 0 y 1
    def random_split(self, ratio, seed=None):
        aux_lines = self.dict_to_lines(self.ratings)

        random.seed(seed)
        random.shuffle(aux_lines)

        split_idx = int(ratio * len(aux_lines))

        train = Ratings(lines=aux_lines[:split_idx])
        test = Ratings(lines=aux_lines[split_idx:])

        return train, test

    def nratings(self):
        n = 0

        for user in self.ratings.keys():
            for item in self.ratings[user]:
                n += 1
        return n

    def users(self):
        res = set()
        for user in self.ratings.keys():
            res.add(user)
        return res

    def items(self):
        res = set()
        for item in self.ratingsByItem.keys():
            res.add(item)
        return res

    def user_items(self, user):
        try:
            return self.ratings[user]
        except KeyError:
            return []

    def item_users(self, item):
        try:
            return self.ratingsByItem[item]
        except KeyError:
            return []


class Ranking:
    class ScoredItem:
        """
        Clase utilizada para gestionar las comparaciones que se realizan dentro del heap
        """

        def __init__(self, element):
            self.element = element

        def __lt__(self, other):
            """
            En primer lugar se compara el score. En caso de que sean iguales (mismo score),
            se compara usando el itemid (se colocará más arriba el elemento con un itemid menor).
            """
            return self.element[0] < other.element[0] if self.element[0] != other.element[0] \
                else self.element[1] > other.element[1]

        def __eq__(self, other):
            return self.element == other.element

        def __str__(self):
            return str(self.element)

        def __repr__(self):
            return self.__str__()

    def __init__(self, topn):
        self.heap = []
        self.topn = topn
        self.changed = 0

    def add(self, item, score):
        scored_item = self.ScoredItem((score, item))
        if len(self.heap) < self.topn:
            heapq.heappush(self.heap, scored_item)
            self.changed = 1
        elif scored_item > self.heap[0]:
            heapq.heappop(self.heap)
            heapq.heappush(self.heap, scored_item)
            self.changed = 1

    def __iter__(self):
        if self.changed:
            self.ranking = []
            h = self.heap.copy()
            while h:
                self.ranking.append(heapq.heappop(h).element[::-1])
            self.changed = 0
        return reversed(self.ranking)

    def __repr__(self):
        r = "<"
        for item, score in self:
            r = r + str(item) + ":" + str(score) + " "
        return r[0:-1] + ">"


class Recommender(ABC):
    def __init__(self, training):
        self.training = training  # Ratings de cada usuario
        self.recommendation = {}

    def __repr__(self):
        return type(self).__name__

    @abstractmethod
    def score(self, user, item):
        """ Core scoring function of the recommendation algorithm """

    def recommend(self, topn):
        for user in self.training.users():
            ranking = Ranking(topn)

            for item in self.training.items():
                # No recomendarle algo que ya tenga
                if item not in self.training.user_items(user):
                    ranking.add(item, self.score(user, item))

            self.recommendation[user] = ranking
        return self.recommendation


class RandomRecommender(Recommender):
    def score(self, user, item):
        return random.random()


class MajorityRecommender(Recommender):
    def __init__(self, ratings, threshold=0):
        super().__init__(ratings)
        self.threshold = threshold

    def score(self, user, item):
        return sum(rating >= self.threshold for user, rating in self.training.item_users(item).items())


# Implementar un primer recomendador simple por rating promedio en una clase AverageRecommender. El
# recomendador sólo recomendará ítems que tengan un mínimo de ratings, mínimo que se indicará como parámetro
# en el constructor (con ello se mejora el acierto de la recomendación). Se proporciona una clase
# MajorityRecommender a modo de ejemplo en el que el estudiante podrá basarse. También se proporciona
# RandomRecommender, que se utiliza en ocasiones como referencia en experimentos.
class AverageRecommender(Recommender):
    def __init__(self, ratings, min=0):
        super().__init__(ratings)
        self.min = min

    def score(self, user, item):
        ratings = self.training.item_users(item).items()
        return sum(rating for usesr, rating in ratings)/len(ratings) if len(ratings) >= self.min else 0


# Implementar la clase UserKNNRecommender para realizar filtrado colaborativo basado
# en usuario (sin normalizar por la suma de similitudes). Se sugiere crear los vecindarios
# “offline” en el constructor del recomendador. Se recomienda asimismo utilizar la clase Ranking,
# que utiliza un heap de ránking, para construir los vecindarios
class UserKNNRecommender(Recommender):
    def __init__(self, ratings, sim, k):
        super().__init__(ratings)
        self.distances = {}
        self.sim = sim
        self.k = k

        users_set = set(self.training.users())
        # Crear vecindarios de k vecinos mas cercanos
        for user in self.training.users():
            users_set.discard(user)
            if user not in self.distances:
                self.distances[user] = Ranking(k)
            for other_user in users_set:
                if other_user not in self.distances:
                    self.distances[other_user] = Ranking(k)

                similarity = sim.sim(user, other_user)
                if similarity != 0.0:
                    self.distances[user].add(other_user, similarity)
                    self.distances[other_user].add(user, similarity)

    def score(self, user, item):
        summation = 0
        for (other_user, distance) in self.distances[user]:
            other_user_ratings = self.training.ratings[other_user]
            if item in other_user_ratings.keys():
                summation += distance * other_user_ratings[item]
        return summation


class NormUserKNNRecommender(UserKNNRecommender):
    def __init__(self, ratings, sim, k, min):
        super().__init__(ratings, sim, k)
        self.min = min

    def score(self, user, item):
        summation = 0
        denominator = 0
        n_ratings = 0
        for (other_user, distance) in self.distances[user]:
            other_user_ratings = self.training.ratings[other_user]
            if item in other_user_ratings.keys():
                n_ratings += 1
                summation += distance * other_user_ratings[item]
                denominator += distance
        if denominator == 0 or n_ratings < self.min:
            return 0
        return summation / denominator


class ItemNNRecommender(Recommender):
    def __init__(self, ratings, sim, k):
        super().__init__(ratings)
        self.distances = {}
        self.sim = sim
        self.k = k

        items_set = set(self.training.items())
        # Crear vecindarios de k vecinos mas cercanos
        for item in self.training.items():
            items_set.discard(item)
            if item not in self.distances:
                self.distances[item] = Ranking(k)
            for other_item in items_set:
                if other_item not in self.distances:
                    self.distances[other_item] = Ranking(k)

                similarity = sim.sim(item, other_item)
                if similarity != 0.0:
                    self.distances[item].add(other_item, similarity)
                    self.distances[other_item].add(item, similarity)

    def score(self, user, item):
        summation = 0
        for (other_item, distance) in self.distances[item]:
            other_item_ratings = self.training.ratings[other_item]
            if user in other_item_ratings.keys():
                summation += distance * other_item_ratings[user]
        return summation


class UserSimilarity(ABC):
    @ abstractmethod
    def sim(self, user1, user2):
        """ Computation of user-user similarity metric """


class CosineUserSimilarity(UserSimilarity):
    def __init__(self, train):
        self.train = train

    def sim(self, user1, user2):
        # Modulo de user1
        mod1 = 0
        for j in self.train.user_items(user1).keys():
            mod1 += math.pow(self.train.rating(user1, j), 2)
        mod1 = math.sqrt(mod1)

        # Modulo de user2
        mod2 = 0
        for j in self.train.user_items(user2).keys():
            mod2 += math.pow(self.train.rating(user2, j), 2)
        mod2 = math.sqrt(mod2)

        # Numerador
        numerador = 0
        for j in self.train.user_items(user1).keys():
            if j in self.train.user_items(user2).keys():
                numerador += self.train.rating(user1, j) * \
                    self.train.rating(user2, j)

        # Denominador
        denominador = mod1 * mod2

        return numerador / denominador


# En recomendación los ratings de test se usan como los juicios de relevancia de la búsqueda.
# Para eso tomamos como relevantes los valores de rating >= threshold.
class Metric(ABC):
    def __init__(self, test, cutoff):
        self.test = test
        self.cutoff = cutoff

    def __repr__(self):
        return type(self).__name__ + ("@" + str(self.cutoff) if self.cutoff != math.inf else "")

    # Esta función se puede dejar abstracta declarándola @abstractmethod,
    # pero también se puede meter algo de código aquí y el resto en las
    # subclases - a criterio del estudiante.
    def compute(self, recommendation):
        """ Completar """
        # TODO: poner aqui la parte donde calculamos los relevantes devueltos?


class Precision(Metric):
    def __init__(self, test, cutoff, threshold):
        self.threshold = threshold
        super().__init__(test, cutoff)

    def compute(self, recommendation):
        res = 0
        # Para P@n tienes que contar cuántos ítems del top n
        # tienen un rating de test >= threshold, y dividir por n.
        # Lo haces para la recomendación de cada usuario, y promedias por usuario.
        for user in recommendation.keys():
            n = 0
            relevantes = 0

            for rec in recommendation[user]:
                try:
                    if self.test.rating(user, rec[0]) >= self.threshold:
                        relevantes += 1
                except:
                    pass  # Si el usuario no está en test la métrica es 0
                n += 1

                if n == self.cutoff:
                    break

            p = relevantes / n
            res += p

        return res / len(recommendation.keys())


class Recall(Metric):
    def __init__(self, test, cutoff, threshold):
        self.threshold = threshold
        super().__init__(test, cutoff)

    def compute(self, recommendation):
        res = 0
        # Es decir, divides por el número total de relevantes del usuario,
        # para calcular el recall de cada usuario. Luego sumas recall
        # y divides por el número de usuarios.
        for user in recommendation.keys():
            n = 0
            relevantesDevueltos = 0
            relevantesTotales = 0

            for rec in recommendation[user]:
                try:
                    if self.test.rating(user, rec[0]) >= self.threshold:
                        relevantesDevueltos += 1
                except:
                    pass
                n += 1

                if n == self.cutoff:
                    break
            try:
                for item in self.test.user_items(user):
                    if self.test.rating(user, item) >= self.threshold:
                        relevantesTotales += 1
                r = relevantesDevueltos / relevantesTotales
            except:
                r = 0
                pass  # Si el usuario no está en test la métrica es 0

            res += r

        return res / len(recommendation.keys())


class F1(Metric):
    def __init__(self, test, cutoff, threshold):
        self.threshold = threshold
        super().__init__(test, cutoff)

    def compute(self, recommendation):
        res = 0
        p = Precision(self.test, self.cutoff,
                      self.threshold).compute(recommendation)
        r = Recall(self.test, self.cutoff,
                   self.threshold).compute(recommendation)

        res = 0
        for user in recommendation.keys():
            if (p+r) == 0:
                res = 0
            else:
                f1 = (2*p*r) / (p + r)
            res += f1

        return round(res / len(recommendation.keys()), 2)


class MAP(Metric):
    def __init__(self, test, cutoff, threshold):
        self.threshold = threshold
        super().__init__(test, cutoff)

    def compute(self, recommendation):
        res = 0
        for user in recommendation.keys():
            i = 1
            relevantesTotales = 0
            numerador = 0

            # Calcular P@i, para cada i relevante
            for rec in recommendation[user]:
                try:
                    if self.test.rating(user, rec[0]) >= self.threshold:
                        p = Precision(self.test, i, self.threshold).compute(
                            recommendation)
                        numerador += p
                except:
                    pass
                i += 1

                if i > self.cutoff:
                    break

            # Calcular relevantes totales del usuario
            try:
                for item in self.test.user_items(user):
                    if self.test.rating(user, item) >= self.threshold:
                        relevantesTotales += 1
                r = numerador / relevantesTotales
            except:
                r = 0
                pass  # Si el usuario no está en test la métrica es 0

            res += r

        return round(res / len(recommendation.keys()), 2)


def student_test():
    training = Ratings("data/toy-train.dat", '\t')
    test = Ratings("data/toy-test.dat", '\t')
    recommenders = [RandomRecommender(training), MajorityRecommender(training, threshold=4), AverageRecommender(
        training, min=3), UserKNNRecommender(training, CosineUserSimilarity(training), 5)]
    metrics = [Precision(test, cutoff=4, threshold=4), Recall(test, cutoff=4, threshold=4), F1(
        test, cutoff=4, threshold=4), MAP(test, cutoff=4, threshold=4)]

    for recommender in recommenders:
        print("Evaluating", recommender)
        recommendation = recommender.recommend(5)
        for metric in metrics:
            print("   ", metric, "=", metric.compute(recommendation))
