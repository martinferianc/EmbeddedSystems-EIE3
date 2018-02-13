from algorithms.exceptions import EmptyDataError, EmptyCentroidsError
from algorithms.kmeans import KMeans
from postprocessing import postprocess_demo

import random
import time
from www.web import create_app
import sys
import paho.mqtt.client as mqtt
import _thread
import json
import csv

DEBUG = True
SAVE  = True

MODEL_NAME =  "kmeans" + "1.0"

if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
    BROKET_HOST = sys.argv[3]
    BROKET_PORT = int(sys.argv[4])

    # Initialization of data postprocessing and ML algorithm
    kmeans = KMeans(k=3)
    kmeans.load('/algorithms/model/{}'.format(MODEL_NAME))

    # Calibrate the sensors



    ### Establishing code for the broker
    # Establish the broker service
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe("esys/headaid/#")

    def on_message(client, userdata, msg):
        # Get the raw data
        data = json.loads(msg.payload.decode("utf-8"))

        # Package the data
        data['TIMESTAMP'] = time.ctime()
        tmp = data['DATA']
        tmp = {'ACX':tmp[0], 'ACY':tmp[1], 'ACZ':tmp[2], 'GYX':tmp[4], 'GYY':tmp[5], 'GYZ':tmp[6]}
        data['DATA'] = tmp

        processed_data = postprocess_data(data)
        label = kmeans.classify(processed_data)



    #Establish connection
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("192.168.10.7", 1883, 60)

    def con_thread():
        client.loop_start()
    _thread.start_new_thread(con_thread,())

    ### Establishing code for the web server
    app = create_app('dev')
    def web_thread():
        app.run(debug=False, host=HOST,port=PORT, threaded=True, use_reloader=False)
    _thread.start_new_thread(web_thread,())
