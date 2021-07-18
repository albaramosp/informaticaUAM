"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""

import whoosh
from whoosh.fields import Schema, TEXT, ID
from whoosh.formats import Format
from whoosh.qparser import QueryParser
from bmi.search.search import Searcher
from bmi.search.index import Index
from bmi.search.index import Builder
from bmi.search.index import TermFreq

# A schema in Whoosh is the set of possible fields in a document in
# the search space. We just define a simple 'Document' schema, with
# a path (a URL or local pathname) and a content.
SimpleDocument = Schema(
        path=ID(stored=True),
        content=TEXT(phrase=False,vector=Format))
ForwardDocument = Schema(
        path=ID(stored=True),
        content=TEXT(phrase=False,vector=Format))
PositionalDocument = Schema(
        path=ID(stored=True),
        content=TEXT(phrase=True,vector=Format))

class WhooshBuilder(Builder):
    def __init__(self, dir, schema=SimpleDocument):
        super().__init__(dir)
        self.whoosh_writer = whoosh.index.create_in(dir, schema).writer(procs=1, limitmb=16384, multisegment=True)
        self.dir = dir

    def index_document(self, p, text):
        self.whoosh_writer.add_document(path=p, content=text)

    def commit(self):
        self.whoosh_writer.commit()
        index = WhooshIndex(self.dir)
        index.save(self.dir)

class WhooshForwardBuilder(WhooshBuilder):
    def __init__(self, dir):
        super().__init__(dir, ForwardDocument)
    def commit(self):
        self.whoosh_writer.commit()
        index = WhooshForwardIndex(self.dir)
        index.save(self.dir)

class WhooshPositionalBuilder(WhooshBuilder):
    def __init__(self, dir):
        super().__init__(dir, PositionalDocument)
    def commit(self):
        self.whoosh_writer.commit()
        index = WhooshPositionalIndex(self.dir)
        index.save(self.dir)

class WhooshIndex(Index):
    def __init__(self, dir):
        super().__init__(dir)
        self.whoosh_reader = whoosh.index.open_dir(dir).reader()    
    def total_freq(self, term):
        return self.whoosh_reader.frequency("content", term)
    def doc_freq(self, term):
        return self.whoosh_reader.doc_frequency("content", term)
    def doc_path(self, docid):
        return self.whoosh_reader.stored_fields(docid)['path']
    def ndocs(self):
        return self.whoosh_reader.doc_count()
    def all_terms(self):
        return list(self.whoosh_reader.field_terms("content"))
    def postings(self, term):
        return self.whoosh_reader.postings("content", term).items_as("frequency") \
            if self.doc_freq(term) > 0 else []

class WhooshForwardIndex(WhooshIndex):
    def doc_vector(self, docID):
        if self.whoosh_reader.has_vector(docID, "content"):
            dv = list()
            for e in self.whoosh_reader.vector_as("frequency", docID, "content"):
                dv.append(TermFreq(e))
            return dv
        print("Error: no doc vector for %s!" % (str(docID)))      
        return list()
    def term_freq(self, term, docID) -> int:
        if self.whoosh_reader.has_vector(docID, "content"):
            v = self.whoosh_reader.vector(docID, "content")
            v.skip_to(term)
            if v.id() == term:
                return v.value_as("frequency")
        return 0

class WhooshPositionalIndex(WhooshForwardIndex):
    def positional_postings(self, term):
        return self.whoosh_reader.postings("content", term).items_as("positions") \
            if self.doc_freq(term) > 0 else []

class WhooshSearcher(Searcher):
    def __init__(self, dir):
        self.whoosh_index = whoosh.index.open_dir(dir)
        self.whoosh_searcher = self.whoosh_index.searcher()
        self.qparser = QueryParser("content", schema=self.whoosh_index.schema)
    def search(self, query, cutoff):
        return map(lambda scoredoc: (self.doc_path(scoredoc[0]), scoredoc[1]),
                   self.whoosh_searcher.search(self.qparser.parse(query), limit=cutoff).items())
    def doc_path(self, docid):
        return self.whoosh_index.reader().stored_fields(docid)['path']
