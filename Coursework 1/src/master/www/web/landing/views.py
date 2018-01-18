from . import landing

import os
import datetime
import flask
import logging
import json

from flask import (
    #Flask,
    abort,
    redirect,
    render_template,
    request,
    url_for,
    Response,
    session
)
data = json.load(open("../../data/team/team.json"))
logger = logging.getLogger("web.landing.views")

def make_error_response(description):
    return flask.Response(
        json.dumps({"status": "error", "message": description}),
        status=400,
        content_type="application/json")

@landing.route('/', methods=['GET'])
@landing.route('/home', methods=['GET','POST'])
def index():
    return render_template('index.html', page_title='Home')

@landing.route('/demo', methods=['GET','POST'])
def demo():
    data = json.load(open("../../data/team/team.json"))
    size = data["size"]
    parsed = []
    for i in range(1,size+1):
        parsed.append(data[str(i)])
    return render_template('demo.html',players=parsed,page_title='Demo')
