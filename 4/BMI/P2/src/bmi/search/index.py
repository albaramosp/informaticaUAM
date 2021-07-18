"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""
import os
import shutil
from bs4 import BeautifulSoup
import zipfile
from urllib.request import urlopen
import math
import pickle
import collections
import re
import ast
import bmi.search.search as s


class Config(object):
    # variables de clase
    NORMS_FILE = "docnorms.dat"
    PATHS_FILE = "docpaths.dat"
    INDEX_FILE = "serialindex.dat"
    DICTIONARY_FILE = "dictionary.dat"
    POSTINGS_FILE = "postings.dat"


class BasicParser:
    @staticmethod
    def parse(text):
        return re.findall(r"[^\W\d_]+|\d+", text.lower())


class TermFreq():
    def __init__(self, t):
        self.info = t

    def term(self):
        return self.info[0]

    def freq(self):
        return self.info[1]


class Index:
    def __init__(self, dir_path=None):
        self.dir_path = dir_path
        self.docmap = []
        self.modulemap = {}
        if dir_path:
            self.open(dir_path)  # Cargar el índice si existe

    def add_doc(self, path):
        self.docmap.append(path)  # Assumed to come in order

    def doc_path(self, docid):
        return self.docmap[docid]

    def doc_module(self, docid):
        if docid in self.modulemap:
            return self.modulemap[docid]
        return None

    def ndocs(self):
        return len(self.docmap)

    def doc_freq(self, term):
        return len(self.postings(term))

    def term_freq(self, term, docID):
        post = self.postings(term)
        if post is None:
            return 0
        for posting in post:
            if posting[0] == docID:
                return posting[1]
        return 0

    def total_freq(self, term):
        freq = 0
        for posting in self.postings(term):
            freq += posting[1]
        return freq

    def doc_vector(self, docID):
        # used in forward indices
        return list()

    def postings(self, term):
        # used in more efficient implementations
        return list()

    def positional_postings(self, term):
        # used in positional implementations
        return list()

    def all_terms(self):
        return list()

    def save(self, dir_path):
        if not self.modulemap:
            self.compute_modules()
        p = os.path.join(dir_path, Config.NORMS_FILE)
        with open(p, 'wb') as f:
            pickle.dump(self.modulemap, f)

    def open(self, dir_path):
        try:
            p = os.path.join(dir_path, Config.NORMS_FILE)
            with open(p, 'rb') as f:
                self.modulemap = pickle.load(f)
        except OSError:
            # the file may not exist the first time
            pass

    def compute_modules(self):
        for term in self.all_terms():
            idf = s.idf(self.doc_freq(term), self.ndocs())
            post = self.postings(term)
            if post is None:
                continue
            for docid, freq in post:
                if docid not in self.modulemap:
                    self.modulemap[docid] = 0
                self.modulemap[docid] += math.pow(s.tf(freq) * idf, 2)
        for docid in range(self.ndocs()):
            self.modulemap[docid] = math.sqrt(
                self.modulemap[docid]) if docid in self.modulemap else 0


class Builder:
    def __init__(self, dir_path, parser=BasicParser()):
        if os.path.exists(dir_path):
            shutil.rmtree(dir_path)
        os.makedirs(dir_path)

    def build(self, path):
        if zipfile.is_zipfile(path):
            self.index_zip(path)
        elif os.path.isdir(path):
            self.index_dir(path)
        else:
            self.index_url_file(path)

    def index_zip(self, filename):
        file = zipfile.ZipFile(
            filename, mode='r', compression=zipfile.ZIP_DEFLATED)
        for name in file.namelist():
            with file.open(name, "r", force_zip64=True) as f:
                self.index_document(name, BeautifulSoup(
                    f.read().decode("utf-8"), "html.parser").text)
        file.close()

    def index_dir(self, dir_path):
        for subdir, dirs, files in os.walk(dir_path):
            for file in files:
                path = os.path.join(dir_path, file)
                with open(path, "r") as f:
                    self.index_document(path, f.read())

    def index_url_file(self, file):
        with open(file, "r") as f:
            self.index_urls(line.rstrip('\n') for line in f)

    def index_urls(self, urls):
        for url in urls:
            self.index_document(url, BeautifulSoup(
                urlopen(url).read().decode("utf-8"), "html.parser").text)

    def index_document(self, path, text):
        pass

    def commit(self):
        pass


class RAMIndex(Index):
    def __init__(self, dir_path=None):
        self.terms_dict = {}  # Indice en si: {term: postings}
        self.postings_path = os.path.join(dir_path, Config.POSTINGS_FILE)
        super().__init__(dir_path)

    def postings(self, term):
        return self.terms_dict[term]

    def all_terms(self):
        return self.terms_dict.keys()

    def save(self, dir_path):
        with open(os.path.join(dir_path, Config.PATHS_FILE), 'wb') as f:
            pickle.dump(self.docmap, f)
        with open(os.path.join(dir_path, Config.DICTIONARY_FILE), 'wb') as f:
            self.terms_dict = collections.OrderedDict(
                sorted(self.terms_dict.items()))  # Orden alfabetico
            pickle.dump(self.terms_dict, f)
        super().save(dir_path) 

    def open(self, dir_path):
        super().open(dir_path)
        try:
            with open(os.path.join(dir_path, Config.PATHS_FILE), 'rb') as f:
                self.docmap = pickle.load(f)
            with open(os.path.join(dir_path, Config.DICTIONARY_FILE), 'rb') as f:
                self.terms_dict = pickle.load(f)

        except OSError:
            # the file may not exist the first time
            pass


class RAMIndexBuilder(Builder):
    def __init__(self, dir_path, parser=BasicParser()):
        super().__init__(dir_path, parser)
        self.path = dir_path
        self.index = RAMIndex(dir_path)

    def commit(self):
        self.index.save(self.path)  
        with open(self.path + Config.INDEX_FILE, "wb") as f:
            pickle.dump(self.index, f)  # Guardamos el indice en si

    def index_document(self, path, text):
        new_doc_id = self.index.ndocs()

        self.index.add_doc(path)

        text = BasicParser.parse(text)
        doc_term_freq = {}  # Diccionario term:freq en este documento

        for pos, term in enumerate(text):
            if self.index.terms_dict.get(term) is None:
                doc_term_freq[term] = 0
                self.index.terms_dict[term] = []

            elif doc_term_freq.get(term) is None:
                doc_term_freq[term] = 0

            doc_term_freq[term] += 1

        for term in doc_term_freq.keys():
            self.index.terms_dict[term].append(
                [new_doc_id, doc_term_freq[term]])


class DiskIndex(Index):
    def __init__(self, dir_path=None):
        self.postings_dict = {}  # term:[postings]
        self.terms_dict = {}  # Indice en si: {term: offsetPostings}
        self.postings_path = os.path.join(dir_path, Config.POSTINGS_FILE)
        self.count = 0
        super().__init__(dir_path)

    def postings(self, term):
        res_postings = []
        with open(self.postings_path, 'r') as f:
            f.seek(self.terms_dict[term])

            doc_amount, raw_postings = f.readline().strip().split("=")
            raw_postings = raw_postings.split(" ")

            for i in range(0, int(doc_amount) * 2, 2):
                res_postings.append(
                    [int(raw_postings[i]), int(raw_postings[i+1])])

        return res_postings

    def all_terms(self):
        return self.terms_dict.keys()

    def doc_freq(self, term):
        with open(self.postings_path, 'r') as f:
            f.seek(self.terms_dict[term])
            result = f.readline().split("=")[0]
        return int(result)

    def save(self, dir_path):
        with open(os.path.join(dir_path, Config.PATHS_FILE), 'wb') as f:
            pickle.dump(self.docmap, f)

        with open(os.path.join(dir_path, Config.POSTINGS_FILE), 'w') as f:
            for term, postings in self.postings_dict.items():
                self.terms_dict[term] = f.tell()
                n_postings = len(postings)
                f.write(str(n_postings) + "=")
                for p in postings:
                    f.write(str(p[0]) + " ")
                    f.write(str(p[1]) + " ")
                f.write("\n")

        with open(os.path.join(dir_path, Config.DICTIONARY_FILE), 'wb') as f:
            self.terms_dict = collections.OrderedDict(
                sorted(self.terms_dict.items()))  # Orden alfabetico
            pickle.dump(self.terms_dict, f)

        super().save(dir_path)

    def open(self, dir_path):
        super().open(dir_path)
        self.postings_dict = {}
        try:
            with open(os.path.join(dir_path, Config.PATHS_FILE), 'rb') as f:
                self.docmap = pickle.load(f)
            with open(os.path.join(dir_path, Config.DICTIONARY_FILE), 'rb') as f:
                self.terms_dict = pickle.load(f)
            # Los offsets se acceden en tiempo de consulta, no van a RAM
        except OSError:
            # the file may not exist the first time
            pass


class CachedDiskIndex(DiskIndex):
    def __init__(self, dir_path=None):
        super().__init__(dir_path)

    def postings(self, term):
        if term in self.postings_dict:
            return self.postings_dict[term]
        with open(self.postings_path, 'r') as f:
            f.seek(self.terms_dict[term])

            doc_amount, raw_postings = f.readline().strip().split("=")
            raw_postings = raw_postings.split(" ")

            for i in range(0, int(doc_amount) * 2, 2):
                self.postings_dict[term].append(
                    [int(raw_postings[i]), int(raw_postings[i+1])])

        return self.postings_dict[term]


class DiskIndexBuilder(Builder):
    def __init__(self, dir_path, parser=BasicParser()):
        super().__init__(dir_path, parser)
        self.path = dir_path
        self.index = DiskIndex(dir_path)

    def commit(self):
        self.index.save(self.path)  # Guardamos las estructuras del indice
        with open(self.path + Config.INDEX_FILE, "wb") as f:
            pickle.dump(self.index, f)  # Guardamos el indice en si

    def index_document(self, path, text):
        new_doc_id = self.index.ndocs()

        self.index.add_doc(path)

        text = BasicParser.parse(text)
        doc_term_freq = {}  # Diccionario term:freq en este documento

        for pos, term in enumerate(text):
            if self.index.postings_dict.get(term) is None:
                doc_term_freq[term] = 0
                self.index.postings_dict[term] = []

            elif doc_term_freq.get(term) is None:
                doc_term_freq[term] = 0

            doc_term_freq[term] += 1

        for term in doc_term_freq.keys():
            self.index.postings_dict[term].append(
                [new_doc_id, doc_term_freq[term]])


class PositionalIndex(RAMIndex):
    def __init__(self, dir_path=None):
        self.terms_dict = {}
        super().__init__(dir_path)
    
    def positional_postings(self, term):
        dict = self.terms_dict[term]
        res = [] 
        for doc, pos in dict.items():
            res.append(tuple((doc, pos))) 
        
        return res

    def postings(self, term):
        dict = self.terms_dict[term]
        res = []
        for doc, pos in dict.items():
            res.append((doc, len(pos)))
        return res


class PositionalIndexBuilder(RAMIndexBuilder):
    def __init__(self, dir_path, parser=BasicParser()):
        super().__init__(dir_path, parser)
        self.index = PositionalIndex(dir_path)

    def index_document(self, path, text):
        new_doc_id = self.index.ndocs()

        self.index.add_doc(path)

        text = BasicParser.parse(text)
        doc_term_pos = {} # Diccionario term: [pos1, pos2...]

        for pos, term in enumerate(text):
            if self.index.terms_dict.get(term) is None:
                doc_term_pos[term] = []
                self.index.terms_dict[term] = {}

            elif doc_term_pos.get(term) is None:
                doc_term_pos[term] = []

            doc_term_pos[term].append(pos)
        
        for term in doc_term_pos.keys():
            self.index.terms_dict[term][new_doc_id] = doc_term_pos[term]

