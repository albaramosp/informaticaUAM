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
from bmi.search.index import (
        RAMIndex, RAMIndexBuilder,
        DiskIndex, DiskIndexBuilder,
        PositionalIndex, PositionalIndexBuilder,
    )
from bmi.search.search import (
        SlowVSMSearcher,
        TermBasedVSMSearcher, DocBasedVSMSearcher,
        ProximitySearcher, 
        PagerankDocScorer,
    )
from bmi.search.whooshy import (
        WhooshBuilder, WhooshIndex, 
        WhooshForwardBuilder, WhooshForwardIndex, 
        WhooshPositionalBuilder, WhooshPositionalIndex, 
        WhooshSearcher,
    )

def main():
    index_root_dir = "./index/"
    collections_root_dir = "./data/collections/"
    test_collection (collections_root_dir + "toy1/", index_root_dir + "toy1/", "cc", ["aa dd", "aa"], False)
    test_collection (collections_root_dir + "toy2/", index_root_dir + "toy2/", "aa", ["aa cc", "bb aa"], False)
    test_collection (collections_root_dir + "urls.txt", index_root_dir + "urls/", "wikipedia", ["information probability", "probability information", "higher probability"], True)
    test_collection (collections_root_dir + "docs1k.zip", index_root_dir + "docs1k/", "seat", ["obama family tree"], True)
    test_collection (collections_root_dir + "docs10k.zip", index_root_dir + "docs10k/", "seat", ["obama family tree"], True)
    test_pagerank("./data/", 5)

def test_collection(collection_path: str, index_path: str, word: str, queries: list, analyse_performance: bool):
    print("=================================================================")
    print("Testing indices and search on " + collection_path)

    # We now test building different implementations of an index
    test_build(WhooshBuilder(index_path + "whoosh"), collection_path)
    test_build(WhooshForwardBuilder(index_path + "whoosh_fwd"), collection_path)
    test_build(WhooshPositionalBuilder(index_path + "whoosh_pos"), collection_path)
    test_build(RAMIndexBuilder(index_path + "ram"), collection_path)
    test_build(DiskIndexBuilder(index_path + "disk"), collection_path)
    test_build(PositionalIndexBuilder(index_path + "pos"), collection_path)

    # We now inspect all the implementations
    indices = [
            WhooshIndex(index_path + "whoosh"),
            WhooshForwardIndex(index_path + "whoosh_fwd"), 
            WhooshPositionalIndex(index_path + "whoosh_pos"), 
            RAMIndex(index_path + "ram"),
            DiskIndex(index_path + "disk"),
            PositionalIndex(index_path + "pos"),
            ]
    for index in indices:
        test_read(index, word)

    for query in queries:
        print("------------------------------")
        print("Checking search results for %s" % (query))
        # Whoosh searcher can only work with its own indices
        test_search(WhooshSearcher(index_path + "whoosh"), WhooshIndex(index_path + "whoosh"), query, 5)
        test_search(WhooshSearcher(index_path + "whoosh_fwd"), WhooshForwardIndex(index_path + "whoosh_fwd"), query, 5)
        test_search(WhooshSearcher(index_path + "whoosh_pos"), WhooshPositionalIndex(index_path + "whoosh_pos"), query, 5)
        test_search(ProximitySearcher(WhooshPositionalIndex(index_path + "whoosh_pos")), WhooshPositionalIndex(index_path + "whoosh_pos"), query, 5)
        for index in indices:
            # our searchers should work with any other index
            test_search(SlowVSMSearcher(index), index, query, 5)
            test_search(TermBasedVSMSearcher(index), index, query, 5)
            test_search(DocBasedVSMSearcher(index), index, query, 5)
        test_search(ProximitySearcher(PositionalIndex(index_path + "pos")), PositionalIndex(index_path + "pos"), query, 5)

    # if we keep the list in memory, there may be problems with accessing the same index twice
    indices = list()

    if analyse_performance:
        # let's analyse index performance
        test_index_performance(collection_path, index_path)
        # let's analyse search performance
        for query in queries:
            test_search_performance(collection_path, index_path, query, 5)

def test_build(builder, collection):
    stamp = time.time()
    print("Building index with", type(builder))
    print("Collection:", collection)
    # this function should index the recieved collection and add it to the index
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
    # more tests
    doc_id = 0
    print("  Frequency of word \"" + word + "\" in document " + str(doc_id) + " - " + index.doc_path(doc_id) + ": " + str(index.term_freq(word, doc_id)))
    print("  Total frequency of word \"" + word + "\" in the collection: " + str(index.total_freq(word)) + " occurrences over " + str(index.doc_freq(word)) + " documents")
    print("  Docs containing the word '" + word + "':", index.doc_freq(word))
    print("    First two documents:", [(doc, freq) for doc, freq in index.postings(word)][0:2])
    print("Done (", time.time() - stamp, "seconds )")
    print()


def test_search (engine, index, query, cutoff):
    stamp = time.time()
    print("  " + engine.__class__.__name__ + " with index " + index.__class__.__name__ + " for query '" + query + "'")
    for path, score in engine.search(query, cutoff):
        print(score, "\t", path)
    print()
    print("Done (", time.time() - stamp, "seconds )")
    print()

def disk_space(index_path: str) -> int:
    space = 0
    for f in os.listdir(index_path):
      p = os.path.join(index_path, f)
      if os.path.isfile(p):
        space += os.path.getsize(p)
    return space

def test_index_performance (collection_path: str, base_index_path: str):
    print("----------------------------")
    print("Testing index performance on " + collection_path + " document collection")

    print("  Build time...")
    start_time = time.time()
    b = WhooshBuilder(base_index_path + "whoosh")
    b.build(collection_path)
    b.commit()
    print("\tWhooshIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    b = WhooshForwardBuilder(base_index_path + "whoosh_fwd")
    b.build(collection_path)
    b.commit()
    print("\tWhooshForwardIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    b = WhooshPositionalBuilder(base_index_path + "whoosh_pos")
    b.build(collection_path)
    b.commit()
    print("\tWhooshPositionalIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    b = RAMIndexBuilder(base_index_path + "ram")
    b.build(collection_path)
    b.commit()
    print("\tRAMIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    b = DiskIndexBuilder(base_index_path + "disk")
    b.build(collection_path)
    b.commit()
    print("\tDiskIndex: %s seconds ---" % (time.time() - start_time))

    print("  Load time...")
    start_time = time.time()
    WhooshIndex(base_index_path + "whoosh")
    print("\tWhooshIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    WhooshForwardIndex(base_index_path + "whoosh_fwd")
    print("\tWhooshForwardIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    WhooshPositionalIndex(base_index_path + "whoosh_pos")
    print("\tWhooshPositionalIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    RAMIndex(base_index_path + "ram")
    print("\tRAMIndex: %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    DiskIndex(base_index_path + "disk")
    print("\tDiskIndex: %s seconds ---" % (time.time() - start_time))

    print("  Disk space...")
    print("\tWhooshIndex: %s space ---" % (disk_space(base_index_path + "whoosh")))
    print("\tWhooshForwardIndex: %s space ---" % (disk_space(base_index_path + "whoosh_fwd")))
    print("\tWhooshPositionalIndex: %s space ---" % (disk_space(base_index_path + "whoosh_pos")))
    print("\tRAMIndex: %s space ---" % (disk_space(base_index_path + "ram")))
    print("\tDiskIndex: %s space ---" % (disk_space(base_index_path + "disk")))


def test_search_performance (collection_name: str, base_index_path: str, query: str, cutoff: int):
    print("----------------------------")
    print("Testing search performance on " + collection_name + " document collection with query: '" + query + "'")
    whoosh_index = WhooshIndex(base_index_path + "whoosh")
    ram_index = RAMIndex(base_index_path + "ram")
    disk_index = DiskIndex(base_index_path + "disk")

    start_time = time.time()
    test_search(WhooshSearcher(base_index_path + "whoosh"), whoosh_index, query, cutoff)
    print("--- Whoosh on Whoosh %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    test_search(SlowVSMSearcher(whoosh_index), whoosh_index, query, cutoff)
    print("--- SlowVSM on Whoosh %s seconds ---" % (time.time() - start_time))

    # let's test some combinations of ranking + index implementations
    start_time = time.time()
    test_search(TermBasedVSMSearcher(whoosh_index), whoosh_index, query, cutoff)
    print("--- TermVSM on Whoosh %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    test_search(TermBasedVSMSearcher(ram_index), ram_index, query, cutoff)
    print("--- TermVSM on RAM %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    test_search(TermBasedVSMSearcher(disk_index), disk_index, query, cutoff)
    print("--- TermVSM on Disk %s seconds ---" % (time.time() - start_time))

    start_time = time.time()
    test_search(DocBasedVSMSearcher(disk_index), disk_index, query, cutoff)
    print("--- DocVSM on Disk %s seconds ---" % (time.time() - start_time))

def test_pagerank(graphs_root_dir, cutoff):
    print("----------------------------")
    print("Testing PageRank")
    # we separate this function because it cannot work with all the collections
    start_time = time.time()
    for path, score in PagerankDocScorer(graphs_root_dir + "toy-graph1.dat", 0.5, 50).rank(cutoff):
        print(score, "\t", path)
    print()
    print("--- Pagerank with toy_graph_1 %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    for path, score in PagerankDocScorer(graphs_root_dir + "toy-graph2.dat", 0.6, 50).rank(cutoff):
        print(score, "\t", path)
    print()
    print("--- Pagerank with toy_graph_2 %s seconds ---" % (time.time() - start_time))
    start_time = time.time()
    for path, score in PagerankDocScorer(graphs_root_dir + "1k-links.dat", 0.2, 50).rank(cutoff):
        print(score, "\t", path)
    print()
    print("--- Pagerank with simulated links for doc1k %s seconds ---" % (time.time() - start_time))

main()
