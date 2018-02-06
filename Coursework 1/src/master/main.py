from algorithms.kmeans import KMeans
import random
import _thread
import time
from www.web import create_app
import sys
import paho.mqtt.client as mqtt


DEBUG = True

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

    #Establish connection
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("192.168.0.10", 1883, 60)
    client.loop_start()

    if DEBUG:
        app.run(debug=True, host=HOST,port=PORT, threaded=True)

    def web_thread():
        app.run(debug=True, host=HOST,port=PORT, threaded=True)
    _thread.start_new_thread(web_thread,())


    # Main Loop
    while True:
        #Write to the team.json any changes
        # Deserialize the json here

        pass
