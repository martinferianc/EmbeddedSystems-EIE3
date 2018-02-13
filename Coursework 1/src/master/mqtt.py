import paho.mqtt.client as mqtt
import time
import json

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("esys/#")

def on_message(client, userdata, msg):
    print(str(msg.payload))
    # Do rest of mqtt stuff
    #load json file
    data = json.loads(msg.payload)
    #get timestamp
    data['TIMESTAMP'] = time.ctime()
    #leave empty label key
    data['LABEL'] = 0
    tmp = data['DATA']
    tmp = {'ACX':tmp[0], 'ACY':tmp[1], 'ACZ':tmp[2], 'GYX':tmp[4], 'GYY':tmp[5], 'GYZ':tmp[6]}
    data['DATA'] = tmp
    with open('data_raw.txt','a') as outfile:
        json.dump(data,outfile)
        outfile.write('\n')

#Establish connection
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.0.3", 1883, 60)
client.loop_start()

while True:
    #Write to the team.json any changes
    # Deserialize the json here
    client.on_message = on_message
    pass
