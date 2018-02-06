from algorithms.kmeans import KMeans
import random
import _thread
import time
from www.web import create_app
import sys


DEBUG = True

if __name__ == '__main__':
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])

    #Establish connection


    #### ALL ESTABLISHING CODE BEFORE THESE LINES ###
    app = create_app('dev')
    if DEBUG:
        app.run(debug=True, host=HOST,port=PORT, threaded=True)


    def web_thread():
        app.run(debug=True, host=HOST,port=PORT, threaded=True)
    _thread.start_new_thread(web_thread,())

    # Main Loop
    while True:
        #Write to the team.json any changes
        pass
