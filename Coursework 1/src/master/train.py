import json
from algorithms.kmeans import KMeans

def preprocessData(testRatio):
    #features to X
    X = []
    Y = []

    with open('../../data_raw.txt','r') as f:
        for line in f:
            data = json.loads(line)
            X.append([data['ACX'],data['ACY'],data['ACZ'],data['GYX'],data['GYY'],data['GYZ']])
            Y.append(data['LABEL'])

    testSize = int(testRatio*len(Y))    

    train_X = X[0:testSize-1]
    train_Y = Y[0:testSize-1]
    
    test_X = X[testSize:]
    test_Y = Y[testSize:]

    return train_X, train_Y, test_X, test_Y

#routine for user to label data
def labelData():
    pass

def train(): 
    #labelData()
    train_X, train_Y, test_X, test_Y = preprocessData(0.1)
    c = KMeans( k=2, tol=0.00001, max_iter=3000)
    c.fit(train_X,train_Y)
    c.test(test_X, test_Y)  
