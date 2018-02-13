import json
from kmeans import KMeans
import numpy as np

MODEL_NAME =  "kmeans" + "1.0"

def preprocessData(testRatio):
    #features to X
    X = []
    Y = []

    with open('../../../data/data_raw.txt','r') as f:
        for line in f:
            data = json.loads(line)
            sensor_data = data["DATA"]
            X.append([sensor_data['ACX'],sensor_data['ACY'],sensor_data['ACZ'],sensor_data['GYX'],sensor_data['GYY'],sensor_data['GYZ']])
            Y.append(data['LABEL'])

    testSize = int(testRatio*len(Y))

    test_X = np.array(X[0:testSize-1])
    test_Y = np.array(Y[0:testSize-1])

    train_X = np.array(X[testSize:])
    train_Y = np.array(Y[testSize:])


    return train_X, train_Y, test_X, test_Y

if __name__ == '__main__':
    train_X, train_Y, test_X, test_Y = preprocessData(0.1)
    c = KMeans(k=3, tol=0.00001, epochs=3000)
    c.fit(train_X,train_Y, save = True, file_path = "model/{}.pickle".format(MODEL_NAME))
    #c.load(file_path = "model/{}.pickle".format(MODEL_NAME))
    label = c.classify([65528,  24,  0,  2015,  65432,  65512])
    print(label)
    print(c.centroids)
    #c.test(test_X, test_Y)
