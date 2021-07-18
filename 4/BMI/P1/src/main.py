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
import time
from bmi.search.search import VSMDotProductSearcher, VSMCosineSearcher
from bmi.search.whooshy import WhooshBuilder, WhooshIndex, WhooshSearcher
from bmi.search.statistics import terms_stats


def main():
    index_root_dir = "./index/"
    collections_root_dir = "./data/collections/"
    test_collection(collections_root_dir + "toy",
                    index_root_dir + "toy", "cc", "aa dd")
    test_collection(collections_root_dir + "urls.txt", index_root_dir +
                    "urls", "wikipedia", "information probability")
    test_collection(collections_root_dir + "docs1k.zip",
                    index_root_dir + "docs", "seat", "obama family tree")


def clear(index_path: str):
    if os.path.exists(index_path):
        shutil.rmtree(index_path)
    else:
        print("Creating " + index_path)
    os.makedirs(index_path)


def test_collection(collection_path: str, index_path: str, word: str, query: str):
    start_time = time.time()
    print("=================================================================")
    print("Testing indices and search on " + collection_path)

    # Let's create the folder if it did not exist
    # and delete the index if it did
    clear(index_path)

    # We now test building an index
    test_build(WhooshBuilder(index_path), collection_path)

    # We now inspect the index
    index = WhooshIndex(index_path)
    test_read(index, word)
    # terms_stats(index) #TODO: descomentar para generar gráficas

    print("------------------------------")
    print("Checking search results")
    test_search(WhooshSearcher(index_path), query, 5)
    test_search(VSMDotProductSearcher(WhooshIndex(index_path)), query, 5)
    test_search(VSMCosineSearcher(WhooshIndex(index_path)), query, 5)


def test_build(builder, collection):
    stamp = time.time()
    print("Building index with", type(builder))
    print("Collection:", collection)
    # this function should index the received collection and add it to the index
    builder.build(collection)
    # when we commit, the information in the index becomes persistent
    # we can also save any extra information we may need
    # (and that cannot be computed until the entire collection is scanned/indexed)
    builder.commit()
    print("Done (", time.time() - stamp, "seconds )")
    print()


def test_read(index, word):
    stamp = time.time()
    print("Reading index with", type(index))
    print("Collection size:", index.ndocs())
    print("Vocabulary size:", len(index.all_terms()))
    terms = index.all_terms_with_freq()
    terms.sort(key=lambda tup: tup[1], reverse=True)
    print("  Top 5 most frequent terms:")
    for term in terms[0:5]:
        print("\t" + term[0] + "\t" + str(term[1]) +
              "=" + str(index.total_freq(term)))
    print()
    # more tests
    doc_id = 0
    # this method provides a vector of tuples
    # for each doc: (term, freq), encapsulated in a TermFreq object (in module index)
    vector = index.doc_vector(doc_id)
    initialTerm = len(vector) / 2
    nTerms = 5
    print("  A few term frequencies for doc_id = " +
          str(doc_id) + " - " + index.doc_path(doc_id) + ": ")
    i = 0
    for f in vector:
        i += 1
        if i >= initialTerm and i < (initialTerm + nTerms):
            print("\t\t%s (%f)" % (f.term(), f.freq()))
    print()
    print("  Frequency of word \"" + word + "\" in document " + str(doc_id) +
          " - " + index.doc_path(doc_id) + ": " + str(index.term_freq(word, doc_id)))
    print("  Total frequency of word \"" + word + "\" in the collection: " +
          str(index.total_freq(word)) + " occurrences over " + str(index.doc_freq(word)) + " documents")
    print("  Docs containing the word'" + word + "':", index.doc_freq(word))
    print("    First two documents:", [(doc, freq)
                                       for doc, freq in index.postings(word)][0:2])
    print("Done (", time.time() - stamp, "seconds )")
    print()


def test_search(engine, query, cutoff):
    stamp = time.time()
    print("  " + engine.__class__.__name__ + " for query '" + query + "'")
    for path, score in engine.search(query, cutoff):
        print(score, "\t", path)
    print()
    print("Done (", time.time() - stamp, "seconds )")
    print()


main()
