import matplotlib.pyplot as plt
import math
import json
from kmeans import KMeans
from datetime import datetime
import time

# This is a helper file to visualize our cluster centroids to check if our ML algorithm works correctly
def magnitude(x,y,z):
    return math.sqrt(x**2+y**2+z**2)

def intSigned(val):
    if val > 32768:
        return val - 65536
    return val

if __name__ == "__main__":

    X = []
    Y = []
    T = []

    with open('../../../data/data_processed.txt','r') as f:
        for line in f:
            data = json.loads(line)
            data['ACX'] = intSigned(data['ACX'])
            data['ACY'] = intSigned(data['ACY'])
            data['ACZ'] = intSigned(data['ACZ'])

            data['GYX'] = intSigned(data['GYX'])
            data['GYY'] = intSigned(data['GYY'])
            data['GYZ'] = intSigned(data['GYZ'])

            accelMag = magnitude(data['ACX'],data['ACY'],data['ACZ'])
            gyroMag = magnitude(data['GYX'],data['GYY'],data['GYZ'])

            X.append(accelMag)
            Y.append(gyroMag)

    with open('../../../data/data_raw.txt','r') as f:
        for line in f:
            data = json.loads(line)
            t = datetime.strptime(data['TIMESTAMP'],'%a %b %d %H:%M:%S %Y')
            T.append(time.mktime(t.timetuple()))
            # Mon Feb 12 18:05:51 2018

    km = KMeans(k=3)
    km.load('./model/{}.pickle'.format("kmeans"+"1.0"))

    plt.plot(T,X)
    plt.show()

    sz = [0.1 for x in range(len(X))]
    c  = ['b' for x in range(len(X))]

    X.append(magnitude(km.centroids[0][0],km.centroids[0][1],km.centroids[0][2]))
    X.append(magnitude(km.centroids[1][0],km.centroids[1][1],km.centroids[1][2]))
    X.append(magnitude(km.centroids[2][0],km.centroids[2][1],km.centroids[2][2]))

    Y.append(magnitude(km.centroids[0][3],km.centroids[0][4],km.centroids[0][5]))
    Y.append(magnitude(km.centroids[1][3],km.centroids[1][4],km.centroids[1][5]))
    Y.append(magnitude(km.centroids[2][3],km.centroids[2][4],km.centroids[2][5]))

    c.extend(['r','r','r'])
    sz.extend([3,3,3])

    plt.scatter(X,Y,sz,c)
    plt.show()
