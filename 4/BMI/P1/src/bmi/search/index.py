"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""


class TermFreq():
    def __init__(self, t):
        self.info = t

    def term(self):
        return self.info[0]

    def freq(self):
        return self.info[1]


class Index:
    def __init__(self, path):
        pass

    def doc_vector(self, doc_id):
        pass

    def ndocs(self):
        pass
    
    def all_terms(self):
        pass

    def all_terms_with_freq(self):
        pass

    def term_freq(self, word, doc_id):
        pass

    def total_freq(self, word):
        pass

    def doc_path(self, doc_id):
        pass

    def doc_freq(self, term):
        pass

    def postings(self, word):
        pass


class Builder:
    def __init__(self, idx_path):
        pass

    def build(self, collection_dir):
        pass

    def commit(self):
        pass