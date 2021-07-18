"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""

import math
import os
from abc import ABC, abstractmethod


def tf(freq):
    return 1 + math.log2(freq) if freq > 0 else 0


def idf(df, n):
    return math.log2((n + 1) / (df + 0.5))


def compute_module(index):
    mod_array = []

    aux_idf_value = {}
    for t in index.all_terms():
        aux_idf_value[t] = idf(index.doc_freq(t), index.ndocs())

    for doc_id in range(index.ndocs()):
        aux_sum = 0
        for tup in index.doc_vector(doc_id):
            if tup.info[0] in aux_idf_value:
                aux_sum += (tf(tup.info[1]) * aux_idf_value[tup.info[0]]) ** 2

        mod_array.append(math.sqrt(aux_sum))

    return mod_array


"""
    This is an abstract class for the search engines
"""


class Searcher(ABC):
    def __init__(self, index, parser):
        self.index = index
        self.parser = parser

    @ abstractmethod
    def search(self, query, cutoff):
        """ Returns a list of documents built as a pair of path and score """
        pass


class VSMDotProductSearcher(Searcher):
    def __init__(self, index):
        self.index = index

    def search(self, query, cutoff):
        # Parseamos la query manualmente
        qterms = query.lower().split(' ')

        results = []

        for doc_id in range(self.index.ndocs()):
            tf_idf = 0
            for q_word in qterms:
                tf_idf += self.score(q_word, doc_id)
            if tf_idf > 0:
                results.append([self.index.doc_path(doc_id), tf_idf])
        results.sort(key=lambda results: results[1], reverse=True)
        return results[0:cutoff]

    def score(self, term, doc_id):
        return tf(self.index.term_freq(term, doc_id)) * idf(self.index.doc_freq(term), self.index.ndocs())


class VSMCosineSearcher(VSMDotProductSearcher):
    def __init__(self, index):
        self.index = index
        self.mod_array = compute_module(index)

    def score(self, term, doc_id):
        return (tf(self.index.term_freq(term, doc_id)) * idf(self.index.doc_freq(term), self.index.ndocs())) / self.mod_array[doc_id]
