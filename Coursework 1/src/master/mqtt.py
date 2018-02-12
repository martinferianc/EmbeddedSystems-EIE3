import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("esys/#")

def on_message(client, userdata, msg):
     print(str(msg.payload))
     # Do rest of mqtt stuff

#Establish connection
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("172.20.10.7", 1883, 60)
client.loop_start()

while True:
    #Write to the team.json any changes
    # Deserialize the json here
    client.on_message = on_message
    pass
