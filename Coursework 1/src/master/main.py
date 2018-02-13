from algorithms.exceptions import EmptyDataError, EmptyCentroidsError
from algorithms.kmeans import KMeans
from algorithms.postprocessing import postprocess_demo, load_calibaration
from algoriths.logging import log_event
from www.web import create_app

import random
import time
import sys
import paho.mqtt.client as mqtt
import _thread
import json
import csv

MODEL_NAME =  "kmeans" + "1.0"


def encapsulate_data(data):
    data["PLAYER"] = 0
    # Package the data
    data['TIMESTAMP'] = time.ctime()
    tmp = data['DATA']
    tmp = {'ACX':tmp[0], 'ACY':tmp[1], 'ACZ':tmp[2], 'GYX':tmp[4], 'GYY':tmp[5], 'GYZ':tmp[6]}
    data['DATA'] = tmp
    return data

if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
    BROKET_HOST = sys.argv[3]
    BROKET_PORT = int(sys.argv[4])

    # Initialization of data postprocessing and ML algorithm
    kmeans = KMeans(k=3)
    kmeans.load('/algorithms/model/{}'.format(MODEL_NAME))

    # Calibrate the sensors
    sensor = PostProcessing()
    sensor.load()


    ### Establishing code for the broker
    # Establish the broker service
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe("esys/headaid/#")

    def on_message(client, userdata, msg):
        # Get the raw data
        data = json.loads(msg.payload.decode("utf-8"))

        #Encapsulate the data into dictionary format
        data = encapsulate_data(data)

        processed_data = postprocess_data(data)
        label = kmeans.classify(processed_data)

        # The condition has been classified as bad
        if label != 0:
            log_event(data=data, label=label)


    ####### MQTT #######
    #Establish connection with the MQTT
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKET_HOST, BROKET_PORT, 60)

    def con_thread():
        client.loop_start()
    _thread.start_new_thread(con_thread,())

    ####### FLASK #######
    ### Establishing code for the web server
    app = create_app('dev')
    def web_thread():
        app.run(debug=False, host=HOST,port=PORT, threaded=True, use_reloader=False)
    _thread.start_new_thread(web_thread,())
