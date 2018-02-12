import numpy as np

class KMeans():
    def __init__(self, k=2, tol=0.001, max_iter=300):
        self.k = k
        self.tol = tol
        self.max_iter = max_iter

    def fit(self,X,Y):
        self.centroids = {}
        for i in range(self.k):
            self.centroids[i] = X[i]

        for i in range(self.max_iter):
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
            if optimized == True:
                break

    #function to save trained centroids
    def saveCentroids(self):
        with open('trained_centroids.txt','w') as f:
            f.writelines(self.centroids)
  
    #function to load trained centroids
    def loadCentroids(self):
        i = 0
        for line in open('trained_centroids.txt','r'):
            self.centroids[centroid] = float(line)
            i+=1

    def classify(self,X):
        distances = [np.linalg.norm(X[index] - self.centroids[centroid]) for centroid in self.centroids]
        classification = distances.index(min(distances))
        return classification

    def test(self,X,Y):
        error = 0
        for index in range(X.shape[0]):
            distances = [np.linalg.norm(X[index] - self.centroids[centroid]) for centroid in self.centroids]
            classification = distances.index(min(distances))
            if classification!=Y[index]:
                error+=1
        print("Accuracy of this run: {}, Test set: {} examples".format(1-error/len(X),len(X)))


#train_X, train_Y, test_X, test_Y = preprocess_data("../../data/titanic/train.csv",0.1)
#c = KMeans( k=2, tol=0.00001, max_iter=3000)
#c.fit(train_X,train_Y)
#c.test(test_X, test_Y)
