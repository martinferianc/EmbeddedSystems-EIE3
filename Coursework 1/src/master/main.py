from algorithms.kmeans import KMeans
import random
import _thread
import time
from www.web import create_app
import sys


if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = sys.argv[2]

    #Establish connection

    #Filtering of the data


    #### ALL ESTABLISHING CODE BEFORE THESE LINES ###
    app = create_app('dev')

    def web_thread():
        app.run(debug=False, host=HOST,port=PORT, threaded=True)
    _thread.start_new_thread(web_thread,())

    # Main Loop
    while True:
        #Write to the team.json any changes
        pass
