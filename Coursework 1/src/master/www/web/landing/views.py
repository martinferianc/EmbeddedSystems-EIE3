from . import landing

import os
import datetime
import flask
import logging
import json
import time

from flask import (
    abort,
    redirect,
    render_template,
    request,
    url_for,
    Response,
    session
)
logger = logging.getLogger("web.landing.views")

def make_error_response(description):
    return flask.Response(
        json.dumps({"status": "error", "message": description}),
        status=400,
        content_type="application/json")

def change_on_field(index):
    data = None
    with open('../../data/team/team.json', 'r') as outfile:
        data = json.load(open("../../data/team/team.json"))
    if data[str(index+1)]["on_field"] == "yes":
        data[str(index+1)]["on_field"] = "no"
    else:
        data[str(index+1)]["on_field"] = "yes"
    with open('../../data/team/team.json', 'w') as outfile:
        json.dump(data, outfile)

def load_data():
    data = json.load(open("../../data/team/team.json"))
    size = data["size"]
    team_name = data["name"]

    parsed = []
    for i in range(1,size+1):
        parsed.append(data[str(i)])

    return (parsed,team_name)


@landing.route('/', methods=['GET','POST'])
@landing.route('/home', methods=['GET','POST'])
@landing.route('/index', methods=['GET','POST'])
@landing.route('/index/', methods=['GET','POST'])
def index():
    return render_template('index.html', page_title='Home')

@landing.route('/demo', methods=['GET','POST'])
@landing.route('/demo/', methods=['GET','POST'])
def demo():
    parsed,team_name = load_data()
    return render_template('demo.html',players=parsed,team_name=team_name,page_title='Demo')

@landing.route('/demo/<int:id>',methods = ['POST', 'GET'])
def p1(id):
    change_on_field(int(request.path.split('/')[-1]))
    return redirect(url_for('.demo'))
