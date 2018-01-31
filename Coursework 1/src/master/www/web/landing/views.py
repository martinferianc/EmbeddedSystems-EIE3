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
    with open("../../data/team/team.json", 'r') as f:
        data = json.load(f)
        size = data["size"]
        team_name = data["name"]

        parsed = []
        for i in range(1,size+1):
            parsed.append(data[str(i)])

        return (parsed,team_name)

def load_log():
    with open("../../data/team/log.json", 'r') as f:
        data = json.load(f)
        parsed = []
        for i in range(0,len(data)):
            parsed.append(data[str(i)])
        return parsed

@landing.after_request
def add_header(r):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    r.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    r.headers["Pragma"] = "no-cache"
    r.headers["Expires"] = "0"
    r.headers['Cache-Control'] = 'public, max-age=0'
    return r

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
    log = load_log()
    return render_template('demo.html',logs = log, players=parsed,team_name=team_name,page_title='Demo')



@landing.route('/demo/data.html',methods = ['POST', 'GET'])
def team():
    data = None
    with open("../../data/team/team.json", 'r') as f:
        data = json.load(f)
    return render_template('data.html',data=json.dumps(data))


@landing.route('/demo/<int:id>',methods = ['POST', 'GET'])
def p1(id):
    change_on_field(int(request.path.split('/')[-1]))
    return redirect(url_for('.demo'))
