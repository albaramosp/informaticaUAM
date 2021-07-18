"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""
import whoosh
import os
from bmi.search.index import Index, Builder, TermFreq
from matplotlib import pyplot as plt


def compute_terms_freqs(index):
    terms_freqs = index.all_terms_with_freq()
    terms_freqs.sort(key=lambda tup: tup[1], reverse=True)
    terms = []
    freqs = []
    for term in terms_freqs:
        terms.append(term[0])
        freqs.append(term[1])
    return terms, freqs


def compute_docs_freqs(index):
    terms = index.all_terms()
    doc_freqs = []
    for term in terms:
        doc_freqs.append((term, index.doc_freq(term)))
    doc_freqs.sort(key=lambda tup: tup[1], reverse=True)

    terms = []
    freqs = []
    for term in doc_freqs:
        terms.append(term[0])
        freqs.append(term[1])
    return terms, freqs


def terms_stats(index):
    output_dir = "stats_output/"
    collection_name = os.path.basename(index.index_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    terms, total_freqs = compute_terms_freqs(index)

    f = plt.figure()
    plt.plot(terms, total_freqs)

    plt.xscale("log")
    plt.xlabel("Terms ordered by frequency")
    plt.yscale("log")
    plt.ylabel("Frequency of term in collection")

    plt.title("Total term frequency on {} collection".format(collection_name))
    f.savefig(os.path.join(
        output_dir, '{}-termFreq.pdf'.format(collection_name)))

    terms, doc_freqs = compute_docs_freqs(index)

    f = plt.figure()
    plt.plot(terms, doc_freqs)

    plt.xscale("log")
    plt.xlabel("Terms ordered by doc frequency")
    plt.yscale("log")
    plt.ylabel("Number of docs containing term")

    plt.title("Total doc frequency on {} collection".format(collection_name))
    f.savefig(os.path.join(
        output_dir, '{}-docFreq.pdf'.format(collection_name)))
