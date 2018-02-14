import numpy as np
import pickle
<<<<<<< HEAD
from exceptions import EmptyCentroidsError, EmptyDataError
=======
from .exceptions import EmptyCentroidsError, EmptyDataError
>>>>>>> f52db219b51a6e2f6ddd11882f0c3eb28d5187f0

class KMeans():
    def __init__(self, k=2, tol=0.001, epochs=300):
        self.k = k
        self.tol = tol
        self.epochs = epochs
        self.centroids = None
    # Does the unsupervised training of the kmeans algorithm
    def fit(self,X,Y, save = False, file_path=""):
        if X.size == 0:
            raise EmptyDataError("Can not train the data is empty")
        print("#####################")
        print("Beginning training on data of shape: {}, max epochs: {} and error tolerance: {}".format(X.shape, self.epochs, self.tol))

        self.centroids = {}
        epochs =0
        for i in range(self.k):
            self.centroids[i] = X[i]

        for i in range(self.epochs):
            self.classifications = {}

            for i in range(self.k):
                self.classifications[i] = []
            for index in range(X.shape[0]):
                distances = [np.linalg.norm(X[index]-self.centroids[centroid]) for centroid in self.centroids]
                classification = distances.index(min(distances))
                self.classifications[classification].append(X[index])
            prev_centroids = dict(self.centroids)
            for classification in self.classifications:
                self.centroids[classification] = np.average(self.classifications[classification],axis = 0)
            optimized = True
            for c in self.centroids:
                original_centroid = prev_centroids[c]
                current_centroid = self.centroids[c]
                if np.sum((current_centroid-original_centroid)/original_centroid*100.0)>self.tol:
                    optimized = False
            epochs+=1
            if optimized == True:
                break
        print("#####################")
        print("Finished training in {} epochs".format(epochs))
        if save:
            print("Saving model in {}".format(file_path))
            self.save(file_path)

    # Pickles the centroids
    def save(self, file_path):
        with open(file_path, 'wb') as handle:
            print("Saving model in {}".format(file_path))
            pickle.dump((self.centroids,self.k), handle, protocol=pickle.HIGHEST_PROTOCOL)
            return True

    # Loads the centroids
    def load(self, file_path):
        with open(file_path, 'rb') as handle:
            print("Loading model in {}".format(file_path))
            (self.centroids,self.k) = pickle.load(handle)
        return True

    # Classifies the data
    def classify(self,X):
        if self.centroids == None:
            raise EmptyCentroidsError("No centroids were trained or loaded!")
        distances = [np.linalg.norm(X - self.centroids[centroid]) for centroid in self.centroids]
        classification = distances.index(min(distances))
        return classification
    # Calculate the accuracy against validation data
    def test(self,X,Y):
        error = 0
        for index in range(X.shape[0]):
            distances = [np.linalg.norm(X[index] - self.centroids[centroid]) for centroid in self.centroids]
            classification = distances.index(min(distances))
            if classification!=Y[index]:
                error+=1
        print("Accuracy of this run: {}, Test set: {} examples".format(1-error/len(X),len(X)))
