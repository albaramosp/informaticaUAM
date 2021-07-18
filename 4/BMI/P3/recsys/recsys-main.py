"""
 Copyright (C) 2021 Pablo Castells y Alejandro Bellogín

 Este código se ha implementado para la realización de las prácticas de
 la asignatura "Búsqueda y minería de información" de 4º del Grado en
 Ingeniería Informática, impartido en la Escuela Politécnica Superior de
 la Universidad Autónoma de Madrid. El fin del mismo, así como su uso,
 se ciñe a las actividades docentes de dicha asignatura.
"""

import datetime
import time
import itertools

from recsys import *


def main():
    print("=========================\nToy test")
    toy_test("data/toy", '\t')
    print("=========================\nTesting toy dataset")
    test_dataset("data/toy-ratings.dat", 1, 2, k=4, min=2, topn=4, cutoff=4)
    print("=========================\nTesting MovieLens \"latest-small\" dataset")
    test_dataset("data/ratings.csv", 35, 1240, k=10,
                 min=3, topn=5, cutoff=5, delimiter=',')


# First tests on toy dataset, using a pre-constructed data split
def toy_test(dataset, separator='\t'):
    training = Ratings(dataset + "-train.dat", separator)
    test = Ratings(dataset + "-test.dat", separator)
    metrics = [Precision(test, cutoff=4, threshold=4),
               Recall(test, cutoff=4, threshold=4)]
    evaluate_recommenders(training, metrics, k=4, min=2, topn=4)


# More complete testing on a generic dataset
def test_dataset(ratings_file, user, item, k, min, topn, cutoff, delimiter='\t'):
    ratings = Ratings(ratings_file, delimiter)
    # Test Ratings class on the dataset
    test_data(ratings, user, item)
    # Run some recommenders on the entire rating data as input - no evaluation
    test_recommenders(ratings, k, min, topn)
    # Now produce a rating split to re-run the recommenders on the training data and evaluate them with the test data
    train, test = ratings.random_split(0.8)
    metrics = [Precision(test, cutoff, threshold=4),
               Recall(test, cutoff, threshold=4)]
    # Double top n to test a slightly deeper ranking
    evaluate_recommenders(train, metrics, k, min, 2 * topn)


# Test the rating data handling code (Ratings class)
def test_data(ratings, user, item):
    print("-------------------------\nTesting the data structures")
    print(ratings.nratings(), "ratings by", len(ratings.users()),
          "users on", len(ratings.items()), "items")
    print("Ratings of user", user, ":", ratings.user_items(user))
    print("Ratings of item", item, ":", ratings.item_users(item))


# Run some recommenders on the some rating data as input - no evaluation
def test_recommenders(ratings, k, min, topn):
    print("-------------------------")
    start = time.process_time()
    test_recommender(RandomRecommender(ratings), topn)
    test_recommender(MajorityRecommender(ratings, threshold=4), topn)
    test_recommender(AverageRecommender(ratings, min), topn)
    timer(start)
    start = time.process_time()
    print("Creating user cosine similarity")
    sim = CosineUserSimilarity(ratings)
    timer(start)
    start = time.process_time()
    print("Creating kNN recommender")
    knn = UserKNNRecommender(ratings, sim, k)
    timer(start)
    start = time.process_time()
    test_recommender(UserKNNRecommender(ratings, sim, k), topn)
    timer(start)
    start = time.process_time()
    test_recommender(NormUserKNNRecommender(ratings, sim, k, min), topn)
    timer(start)


# Run one recommender on the some rating data as input - no evaluation
def test_recommender(recommender, topn):
    print("Testing", recommender, "(top", str(topn) + ")")
    recommendation = recommender.recommend(topn)
    for user in itertools.islice(recommendation, 4):
        print("    User", user, "->", recommendation[user])


# Create some recommenders and send them for evaluation for a list of given metrics
def evaluate_recommenders(training, metrics, k, min, topn):
    print("-------------------------")
    start = time.process_time()
    evaluate_recommender(RandomRecommender(training), topn, metrics)
    evaluate_recommender(MajorityRecommender(
        training, threshold=4), topn, metrics)
    evaluate_recommender(AverageRecommender(training, min), topn, metrics)
    sim = CosineUserSimilarity(training)
    knn = UserKNNRecommender(training, sim, k)
    evaluate_recommender(knn, topn, metrics)
    evaluate_recommender(NormUserKNNRecommender(
        training, sim, k, min), topn, metrics)


# Run one recommender and evaluate a list of metrics on its output
def evaluate_recommender(recommender, topn, metrics):
    print("Evaluating", recommender)
    recommendation = recommender.recommend(topn)
    for metric in metrics:
        print("   ", metric, "=", metric.compute(recommendation))


def timer(start):
    print("--> elapsed time:",
          datetime.timedelta(seconds=round(time.process_time() - start)), "<--")


main()
