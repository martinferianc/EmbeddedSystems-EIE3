import json
import numpy as np
from kmeans import KMeans

MODEL_NAME =  "kmeans" + "1.0"

def preprocessData(testRatio, file_path='../../../data/data_processed.txt'):
    #features to X
    X = []
    with open(file_path,'r') as f:
        for line in f:
            data = json.loads(line)
            X.append([data['ACMAG'],data['GYMAG']])

    testSize = int(testRatio*len(X))
    test_X = np.array(X[0:testSize-1])

    train_X = np.array(X[testSize:])


    return train_X, test_X

if __name__ == '__main__':
    train_X,test_X = preprocessData(0.1)
    c = KMeans(k=3, tol=0.00001, epochs=3000)
    c.fit(train_X,[], save = True, file_path = "model/{}.pickle".format(MODEL_NAME))
    #c.load(file_path = "model/{}.pickle".format(MODEL_NAME))
