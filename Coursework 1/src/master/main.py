from algorithms.team import Team
from algorithms.kmeans import KMeans
import random
import _thread
import time
from www.web import create_app




HOST = "0.0.0.0"
PORT = 8086
#Connection

#Filtering

#Write to json


#ALL ESTABLISHING CODE BEFORE THESE LINES
app = create_app('dev')

def flask_thread():
    app.run(debug=False, host=HOST,port=PORT, threaded=True)
_thread.start_new_thread(flask_thread,())

# Main Loop
while True:
    pass
