from algorithms.kmeans import KMeans
import random
import time
from www.web import create_app
import sys
#import paho.mqtt.client as mqtt
#import _thread
import json
import csv

DEBUG = True
SAVE  = True

def saveData(msg):
    #load json file
    data = json.loads(msg.payload)
    #get timestamp
    data['TIMESTAMP'] = time.ctime()
    #leave empty label key
    data['LABEL'] = 0     
    tmp = data['DATA']
    tmp = {'ACX':tmp[0], 'ACY':tmp[1], 'ACZ':tmp[2], 'GYX':tmp[4], 'GYY':tmp[5], 'GYZ':tmp[6]}
    data['DATA'] = tmp
    with open('../../data/data_raw.txt','a') as outfile:
        json.dump(data,outfile)
        outfile.write('\n')

if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])

    #### ALL ESTABLISHING CODE BEFORE THESE LINES ###
    app = create_app('dev')

    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe("esys/headaid/#")

    def on_message(client, userdata, msg):
        print(str(msg.payload))
        # Deserialize the JSON here
        # Write to the team.json any changes
        
        if SAVE:
            saveData(msg)            
        
        return
      

    #Establish connection
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    def con_thread():
        client.loop_start()
        client.connect("172.20.10.7", 1883, 60)
    _thread.start_new_thread(con_thread,())

    def web_thread():
        app.run(debug=False, host=HOST,port=PORT, threaded=True, use_reloader=False)
    _thread.start_new_thread(web_thread,())

    # Main Loop
    while True:
        #print("potato")
        pass

