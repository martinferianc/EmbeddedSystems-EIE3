import math
import numpy as np
import json

# These functions calculate the basic threshold for the sensors to operate based on the past data that we have collected
def getMagnitude(val):
  return abs(val[0])+abs(val[1])+abs(val[2])

def getAvg(vals):
  return np.mean(np.array(vals))

def intSigned(val):
    if val > 32768:
        return val - 65536
    return val

if __name__=="__main__":
    #get data from readings
    accel = []
    gyro = []

    with open('../../../data/data_raw.txt','r') as f:
        for line in f:
            data = json.loads(line)['DATA']
            data['ACX'] = intSigned(data['ACX'])
            data['ACY'] = intSigned(data['ACY'])
            data['ACZ'] = intSigned(data['ACZ'])

            data['GYX'] = intSigned(data['GYX'])
            data['GYY'] = intSigned(data['GYY'])
            data['GYZ'] = intSigned(data['GYZ'])

            accel.append(getMagnitude([data['ACX'],data['ACY'],data['ACZ']]))
            gyro.append(getMagnitude([data['GYX'],data['GYY'],data['GYZ']]))

    #sort lists
    accel.sort()
    gyro.sort()

    accelBottom = accel[0:int(len(accel)*0.1)]

    gyroBottom = gyro[0:int(len(gyro)*0.1)]

    accelThreshold = getAvg(accelBottom)
    gyroThreshold  = getAvg(gyroBottom)

    print('acceleration threshold = ',accelThreshold)
    print('gyroscope threshold    = ',gyroThreshold)
