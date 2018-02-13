import json
from algorithms.kmeans import KMeans
import numpy as np

def preprocessData(testRatio):
    #features to X
    X = []
    Y = []

    with open('../../../data/data_raw.txt','r') as f:
        for line in f:
            data = json.loads(line)
            X.append([data['ACX'],data['ACY'],data['ACZ'],data['GYX'],data['GYY'],data['GYZ']])
            Y.append(data['LABEL'])

    testSize = int(testRatio*len(Y))

    train_X = np.array(X[0:testSize-1])
    train_Y = np.array(Y[0:testSize-1])

    test_X = np.array(X[testSize:])
    test_Y = np.array(Y[testSize:])

    return train_X, train_Y, test_X, test_Y

if __name__ == '__main__':
    train_X, train_Y, test_X, test_Y = preprocessData(0.1)
    c = KMeans(k=3, tol=0.00001, epochs=3000)
    c.fit(train_X,train_Y, save = True, file_path = "/model/kmeans1.pickle")
    c.test(test_X, test_Y)
