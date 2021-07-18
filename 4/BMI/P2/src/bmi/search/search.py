"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""

import math
from abc import ABC, abstractmethod
from heapq import *

import time


def tf(freq):
    return 1 + math.log2(freq) if freq > 0 else 0


def idf(df, n):
    return math.log2((n + 1) / (df + 0.5))


class BasicParser:
    def parse(self, query):
        return query.lower().split(' ')


class SearchRanking:
    def __init__(self, cutoff):
        self.ranking = []
        heapify(self.ranking)
        self.cutoff = cutoff

    """
        El heap de ranking es un minHeap de tuplas (score, docid).
        La funcion heappush ya lo inserta ordenando de menor a mayor en el heap.
    """

    def push(self, docid, score):
        heappush(self.ranking, (score, docid))

    """
        El heap de ranking es un minHeap. La funcion nlargest devuelve los n
        elementos mas grandes del heap en forma de lista de tuplas (score, docid).
        Nosotros queremos las tuplas al revés, así que usamos reversed y esto lo
        devolvemos en un iterator para que no se queje el for del main.
        El cutoff hay que tenerlo en cuenta porque este ranking se usa para distintos modelos,
        no solo para el orientado a documentos, sino podríamos decidir si hacer push o no en funcion
        del tamaño del heap.
    """

    def __iter__(self):
        return iter([tuple(reversed(e)) for e in nlargest(
            min(len(self.ranking), self.cutoff),
            self.ranking)])


"""
    This is an abstract class for the search engines
"""
class Searcher(ABC):
    def __init__(self, index, parser):
        self.index = index
        self.parser = parser

    @abstractmethod
    def search(self, query, cutoff) -> SearchRanking:
        """ Returns a list of documents built as a pair of path and score """


class SlowVSMSearcher(Searcher):
    def __init__(self, index, parser=BasicParser()):
        super().__init__(index, parser)

    def search(self, query, cutoff):
        qterms = self.parser.parse(query)
        ranking = SearchRanking(cutoff)
        for docid in range(self.index.ndocs()):
            score = self.score(docid, qterms)
            if score:
                ranking.push(self.index.doc_path(docid), score)
        return ranking

    def score(self, docid, qterms):
        prod = 0
        for term in qterms:
            prod += tf(self.index.term_freq(term, docid)) \
                * idf(self.index.doc_freq(term), self.index.ndocs())
        mod = self.index.doc_module(docid)
        if mod:
            return prod / mod
        return 0


class TermBasedVSMSearcher(Searcher):
    def __init__(self, index, parser=BasicParser()):
        super().__init__(index, parser)

    def search(self, query, cutoff):
        query_terms = self.parser.parse(query)
        ranking = SearchRanking(cutoff)
        posting_values = {}
        scores = []

        for term in query_terms:
            for doc_id, _ in self.index.postings(term):
                if doc_id not in posting_values.keys():
                    posting_values[doc_id] = []
                posting_values[doc_id].append(term)

        for doc_id in posting_values.keys():
            score = self.score(doc_id, posting_values[doc_id])
            if score:
                ranking.push(self.index.doc_path(doc_id), score)

        return ranking

    def score(self, doc_id, query_terms):
        product = 0
        for term in query_terms:
            product += tf(self.index.term_freq(term, doc_id)) * \
                idf(self.index.doc_freq(term), self.index.ndocs())

        module = self.index.doc_module(doc_id)
        return (product / module) if module else 0


class DocBasedVSMSearcher(Searcher):
    def __init__(self, index, parser=BasicParser()):
        super().__init__(index, parser)

    def search(self, query, cutoff):
        ndocs = self.index.ndocs()
        qterms = self.parser.parse(query)
        indices = [0] * len(qterms)  # Cosecuentials
        postings = []
        acc = [0] * ndocs  # Accumulators
        ranking = SearchRanking(cutoff)

        for q in qterms:
            postings += [[p for p in self.index.postings(q)]]

        heap = []
        heapify(heap)
        for q in range(len(qterms)):
            heappush(heap, (postings[q][indices[q]], q))
            indices[q] += 1  # iterate cosecuentially

        currDoc = nsmallest(1, heap)[0][0][0]
        try:
            while(1):
                element = heappop(heap)
                docid = element[0][0]
                freq = element[0][1]
                q = element[1]
                score = tf(freq) * idf(self.index.doc_freq(qterms[q]), ndocs)

                if docid != currDoc:
                    acc[currDoc] /= self.index.doc_module(currDoc)
                    # insert only if not less than top if full
                    if len(ranking.ranking) < ranking.cutoff:
                        ranking.push(self.index.doc_path(
                            currDoc), acc[currDoc])
                    elif score > nsmallest(1, ranking.ranking)[0][0]:
                        ranking.push(self.index.doc_path(
                            currDoc), acc[currDoc])

                    currDoc = docid
                acc[currDoc] += score

                if indices[q] < len(postings[q]):
                    heappush(heap, (postings[q][indices[q]], q))
                    indices[q] += 1

        except Exception:  # El heap se ha vaciado
            acc[currDoc] /= self.index.doc_module(currDoc)
            if len(ranking.ranking) < ranking.cutoff:
                ranking.push(self.index.doc_path(currDoc), acc[currDoc])
            elif score > nsmallest(1, ranking.ranking)[0][0]:
                ranking.push(self.index.doc_path(currDoc), acc[currDoc])

        return ranking


class ProximitySearcher(Searcher):
    def __init__(self, index, parser=BasicParser()):
        super().__init__(index, parser)
    
    def search(self, query, cutoff):
        # We get all the documents that contain the given terms
        query_terms = self.parser.parse(query)
        ranking = SearchRanking(cutoff)

        # We get the first suitable doc_ids
        suitable_doc_ids = set()
        for doc_id, _ in self.index.postings(query_terms[0]):
            suitable_doc_ids.add(doc_id)

        # We now check for the remaining sets
        if len(query_terms) > 1:
            for term in query_terms[1:]:
                current_doc_ids = set()
                for doc_id, _ in self.index.postings(term):
                    current_doc_ids.add(doc_id)
                suitable_doc_ids &= current_doc_ids

        # We should now have only suitable doc_ids, containg all query terms
        # We now will get a cleaned up positional_postings
        # We convert it to a dict to ease how its used
        # {term1: {docid1: [positions], docid2: [positions], ...}, term2: ...}
        cleaned_positions = {}
        for term in query_terms:
            term_clean_positions = {}
            term_positions = self.index.positional_postings(term)
            for doc_positions in term_positions:
                if doc_positions[0] in suitable_doc_ids:
                    term_clean_positions[doc_positions[0]] = doc_positions[1]
            cleaned_positions[term] = term_clean_positions

        # We now have a clean positional postings dict with only relevant docs
        # These are the ones that will get a score different from 0
        for doc_id in suitable_doc_ids:
            ranking.push(self.index.doc_path(doc_id), 
                         self.score(cleaned_positions, doc_id, query_terms))

        return ranking

    def score(self, cleaned_positions, doc_id, query_terms):
        # Obtain the needed lists, already cleaned
        posList = []
        for term in query_terms: 
            posList.append(cleaned_positions[term][doc_id] + [math.inf])
        q_len = len(query_terms)
        # Weird to get till here, but if only one term, score is ocurrences
        if q_len == 1:
            return float(len(posList[0]) - 1)

        # Set initial state
        a = - 1
        score = 0
        p = [0] * q_len
        b = max([t_list[0] for t_list in posList])

        while b != math.inf:
            i = 0
            for j in range(q_len):
                # advance list till position b
                while posList[j][p[j]+1] <= b:
                    p[j] += 1
                # take the least position of all this lists in i variable
                if posList[j][p[j]] < posList[i][p[i]]:
                    i = j
            a = posList[i][p[i]]
            score += 1 / (b - a - q_len + 2)
            b = posList[i][p[i]+1]
            
        return score


class PagerankDocScorer():
    def __init__(self, graphfile, r, n_iter):
        self.n_iter = n_iter

        f = open(graphfile, "r")
        lines = f.read().splitlines()
        f.close()

        self.nodes = set()  # Elementos no ordenados y sin repetición
        self.outs = {}  # Clave: nodo, valor: lsita de a quienes apunta
        self.to_from_nodes = {}  # Clave: nodo, valor: lista de quienes le apunatn

        # Voy viendo donde apunta cada nodo y almacenando los nodos en si
        for line in lines:
            ele = line.split('\t')
            self.nodes.add(ele[0])
            self.nodes.add(ele[1])

            # Voy contando las salidas de cada nodo
            try:
                self.outs[ele[0]].append(ele[1])
            except:
                self.outs[ele[0]] = [ele[1]]

            try:
                self.to_from_nodes[ele[1]].append(ele[0])
            except:
                self.to_from_nodes[ele[1]] = [ele[0]]

            # Añadir sumideros
            try:
                if self.outs[ele[1]]:
                    pass
            except:
                self.outs[ele[1]] = []

            try:
                if self.to_from_nodes[ele[0]]:
                    pass
            except:
                self.to_from_nodes[ele[0]] = []

        # Guardamos los sumideros
        self.sumideros = []
        for node in self.outs.keys():
            if len(self.outs[node]) == 0:
                self.sumideros.append(node)

        self.n = len(self.nodes)
        self.r = r

    def rank(self, cutoff):
        p1 = self.r / self.n
        p2 = 1 - self.r
        res = []

        pageRankPrev = {}
        pageRankAct = {}

        # Les damos a todos un pageRank de 1 inicialmente
        for n in self.nodes:
            pageRankPrev[n] = 1

        # Iteramos hasta el limite
        for epoc in range(self.n_iter):
            for n in self.nodes:
                sum = 0
                # Sumamos pageRank de todos los nodos que van hacia este
                for origin in self.to_from_nodes[n]:
                    sum += pageRankPrev[origin] / len(self.outs[origin])

                # Finalmente sumamos los sumideros
                for i in self.sumideros:
                    sum += pageRankPrev[i] / self.n

                pageRankAct[n] = p1 + p2 * sum

            # Nos guardamos los valores de pageRank para siguiente vuelta
            for n in pageRankAct.keys():
                pageRankPrev[n] = pageRankAct[n]

        # Ordenamos por valor de pageRank
        ordered = dict(sorted(pageRankAct.items(),
                              key=lambda item: item[1], reverse=True))
        cont = 0
        for k, v in ordered.items():
            res.append((k, v))
            cont += 1
            if cont == cutoff:
                break
        # Devolvemos una lista [(nodo, pageRankScore), (nodo, pageRankScore), etc]
        return res
