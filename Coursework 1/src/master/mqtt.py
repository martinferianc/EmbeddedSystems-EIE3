import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("esys/headaid/#")

def on_message(client, userdata, msg):
     print(str(msg.payload))
     # Do rest of mqtt stuff

#Establish connection
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.0.10", 1883, 60)
client.loop_start()

while True:
    #Write to the team.json any changes
    # Deserialize the json here
    pass
