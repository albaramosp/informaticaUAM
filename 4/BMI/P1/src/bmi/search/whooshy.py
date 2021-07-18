"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""
import time

import os
import shutil
import math
from bs4 import BeautifulSoup
from zipfile import ZipFile
from urllib.request import urlopen

import whoosh
from whoosh.fields import Schema, TEXT, ID
from whoosh.formats import Format
from whoosh.qparser import QueryParser

from bmi.search.search import Searcher, idf, tf
from bmi.search.index import Index, Builder, TermFreq

# A schema in Whoosh is the set of possible fields in a document in
# the search space. We just define a simple 'Document' schema
Document = Schema(path=ID(stored=True),
                  content=TEXT(vector=Format))


class WhooshBuilder(Builder):
    def __init__(self, index_path):
        self.index_path = index_path
        if os.path.exists(index_path):
            shutil.rmtree(index_path)
        os.makedirs(index_path)
        self.writer = whoosh.index.create_in(index_path, Document).writer()

    def build(self, collection_path):
        # If its a file, it could be a urls.txt ir zip
        if os.path.isfile(collection_path):
            if collection_path.endswith(".txt"):
                with open(collection_path) as file:
                    content = file.readlines()
                    # We access each of the referenced urls
                    for url in content:
                        url = url.strip()
                        self.writer.add_document(
                            path=url, content=BeautifulSoup(urlopen(url).read(), "html.parser").text)
                return
            elif collection_path.endswith(".zip"):
                compressed_urls = ZipFile(collection_path, "r")
                for url in compressed_urls.namelist():
                    self.writer.add_document(path=url, content=BeautifulSoup(
                        compressed_urls.read(url), "html.parser").text)
                return
        # Else, its a dir with files. Testing purposes only, not for HTML dirs
        else:
            dir_files = os.listdir(collection_path)
            for document in sorted(dir_files):
                path = os.path.join(collection_path, document)
                with open(path, 'r') as f:
                    self.writer.add_document(path=path, content=f.read())

    def commit(self):
        self.writer.commit()


class WhooshIndex(Index):
    def __init__(self, path):
        self.index = whoosh.index.open_dir(path)
        self.index_path = path
        self.reader = whoosh.index.open_dir(path).reader()

    def doc_vector(self, doc_id):
        vector = []
        raw_vec = self.reader.vector_as("frequency", doc_id, "content")

        for t in raw_vec:
            term = t[0]
            freq = t[1]
            t = (term, freq)  # La frecuencia de term en el documento doc_id
            # Cada elemento del vector es un TermFreq
            vector.append(TermFreq(t))

        return vector

    def ndocs(self):
        return self.reader.doc_count()

    def all_terms(self):
        terms = self.reader.all_terms()
        self.terms = []
        for schema_field, term in terms:
            if schema_field != "content":
                continue
            self.terms.append(term.decode("utf-8"))
        return self.terms  # Only terms now, on content
    
    def all_terms_with_freq(self):
        res = []
        for t in self.terms:
            res.append(
                (t, self.reader.frequency("content", t)))

        return res  # (t1, f1), (t2, f2)..

    def term_freq(self, word, doc_id):
        raw_vec = self.reader.vector(doc_id, "content")
        raw_vec.skip_to(word)
        return raw_vec.value_as("frequency") if raw_vec.id() == word else 0

    def total_freq(self, word):
        return self.reader.frequency("content", word)

    def doc_path(self, doc_id):
        return self.reader.stored_fields(doc_id)['path']

    def doc_freq(self, term):
        return self.reader.doc_frequency("content", term)

    def get_documents(self):
        return self.index.searcher().documents()

    def postings(self, word):
        res = []
        try:
            pr = self.reader.postings("content", word)
        except:
            return res

        cont = 0

        for cont in range(self.ndocs()):
            try:
                pr.skip_to(cont)
                if pr.id() == cont:  # El documento contiene el termino
                    freq = self.term_freq(word, cont)
                    res.append((cont, freq))
            except:
                pass

        return res


class WhooshSearcher(Searcher):
    def __init__(self, index_path):
        index = whoosh.index.open_dir(index_path)
        qparser = QueryParser("content", schema=Document)
        super().__init__(index, qparser)
        self.searcher = self.index.searcher()

    def search(self, query, cutoff):
        res = []
        cont = 0
        for docid, score in self.searcher.search(self.parser.parse(query)).items():
            if cont >= cutoff:
                break
            path = self.index.reader().stored_fields(docid)['path']
            res.append((path, score))
            cont += 1

        return res
