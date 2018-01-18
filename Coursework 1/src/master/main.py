from algorithms.team import Team
from algorithms.kmeans import KMeans
import random
import time
from www.web import create_app


app = create_app('dev')
app.run(debug=True, host="0.0.0.0",port=8083, threaded=True)
