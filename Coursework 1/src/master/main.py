from algorithms.exceptions import EmptyDataError, EmptyCentroidsError
from algorithms.kmeans import KMeans
from algorithms.postprocessing import PostProcessing, encapsulate_data
from algorithms.log import log_event, check_on_field
from www.web import create_app

import hashlib
import random
import time
import sys
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

import _thread
import json

# Define static variables
N_PLAYERS = 20
MODEL_NAME =  "kmeans" + "1.0"
SENSOR_STATES = [0 for i in range(20)]

# Decrypt the message for security reasons
def decrypt(val):
    temp = int(0)
    temp = temp | ((val << 0)  & 0x000000FF)
    temp = temp | ((val << 16) & 0xFF000000)
    temp = temp | ((val << 0)  & 0x00FF0000)
    temp = temp | ((val >> 16) & 0x0000FF00)
    return temp




if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
    BROKER_HOST = sys.argv[3]
    BROKER_PORT = int(sys.argv[4])
    print("Initializing the web server & MQTT broker")
    print("BROKER HOST {}".format(BROKER_HOST))
    print("BROKER PORT {}".format(BROKER_PORT))
    print("HOST {}".format(HOST))
    print("PORT {}".format(PORT))

    # Initialization of data postprocessing and ML algorithm
    kmeans = KMeans(k=3)
    kmeans.load('algorithms/model/{}.pickle'.format(MODEL_NAME))
    print(kmeans.centroids)

    # Calibrate the sensors
    sensor = PostProcessing()
    sensor.load_gyro_calibration(file_path="algorithms/calibration/calibration_values.txt")


    ### Establishing code for the broker
    # Establish the broker service
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe("esys/HeadAid/sensor")

    def on_message(client, userdata, msg):
        # Get the raw data
        temp = {}

        data = msg.payload.decode("utf-8")
        data = data.strip("[").strip("]").split(",")

        data = [decrypt(int(d)) for d in data]

        temp['PLAYER']          = data[3]
        temp['DEVICE ADDRESS']  = data[4]

        # Manipulate the data to reflect the compression of the sent data
        temp['DATA'] = []
        temp['DATA'].append(data[0]     & 0xFFFF)
        temp['DATA'].append(data[0]>>16 & 0xFFFF)
        temp['DATA'].append(data[1]     & 0xFFFF)
        temp['DATA'].append(0)
        temp['DATA'].append(data[1]>>16 & 0xFFFF)
        temp['DATA'].append(data[2]     & 0xFFFF)
        temp['DATA'].append(data[2]>>16 & 0xFFFF)

        data = temp


        #Encapsulate the data into dictionary format
        data = encapsulate_data(data)

        processed_data = sensor.postprocess_data(data)
        label = kmeans.classify(processed_data)

        # The condition has been classified as bad
        if label < 2:
            log_event(data=data, label=label)

    ####### MQTT #######
    #Establish connection with the MQTT
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    def con_thread():
        client.loop_start()
        client.connect(BROKER_HOST, BROKER_PORT, 60)
    _thread.start_new_thread(con_thread,())


    ####### FLASK #######
    ### Establishing code for the web server
    app = create_app('dev')
    def web_thread():
        app.run(debug=False, host=HOST,port=PORT, threaded=True, use_reloader=False)
    _thread.start_new_thread(web_thread,())

    def turn_sensor(value,sensor):
        client.publish("esys/HeadAid/on_field_status{}".format(sensor), str(value))

    # Necessary while loop for the code to keep on executing
    while True:
        # Delay to reduce the checking if the players are on the field, selected by the coach
        time.sleep(1)
        players_on_field = check_on_field()
        for i in range(N_PLAYERS):
            if players_on_field[i] != SENSOR_STATES[i]:
                SENSOR_STATES[i] = players_on_field[i]
                # Turn on or off the sensor in case player state has been changed to conserve power
                turn_sensor(SENSOR_STATES[i],i)
