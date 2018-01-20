from . import landing

import os
import datetime
import flask
import logging
import json
import time

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

def load_json_data():
    data = json.load(open("/team/team.json"))
    size = data["size"]
    team_name = data["name"]

    t = time.strftime('%H:%M:%S')

    parsed = []
    for i in range(1,size+1):
        parsed.append(data[str(i)])

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

    t = time.strftime('%H:%M:%S')

    parsed = []
    for i in range(1,size+1):
        parsed.append(data[str(i)])

    return (parsed,t,team_name)


@landing.route('/', methods=['GET'])
@landing.route('/home', methods=['GET','POST'])
def index():
    return render_template('index.html', page_title='Home')


@landing.route('/demo', methods=['GET','POST'])
@landing.route('/demo/', methods=['GET','POST'])
def demo():
    parsed,t,team_name = load_data()
    return render_template('demo.html',players=parsed,team_name=team_name,time=t,page_title='Demo')

@landing.route('/demo/0',methods = ['POST', 'GET'])
@landing.route('/demo/1',methods = ['POST', 'GET'])
@landing.route('/demo/2',methods = ['POST', 'GET'])
@landing.route('/demo/3',methods = ['POST', 'GET'])
@landing.route('/demo/4',methods = ['POST', 'GET'])
@landing.route('/demo/5',methods = ['POST', 'GET'])
@landing.route('/demo/6',methods = ['POST', 'GET'])
@landing.route('/demo/7',methods = ['POST', 'GET'])
@landing.route('/demo/8',methods = ['POST', 'GET'])
@landing.route('/demo/9',methods = ['POST', 'GET'])
@landing.route('/demo/10',methods = ['POST', 'GET'])
@landing.route('/demo/11',methods = ['POST', 'GET'])
@landing.route('/demo/12',methods = ['POST', 'GET'])
@landing.route('/demo/13',methods = ['POST', 'GET'])
@landing.route('/demo/14',methods = ['POST', 'GET'])
@landing.route('/demo/15',methods = ['POST', 'GET'])
@landing.route('/demo/16',methods = ['POST', 'GET'])
@landing.route('/demo/17',methods = ['POST', 'GET'])
@landing.route('/demo/18',methods = ['POST', 'GET'])
@landing.route('/demo/19',methods = ['POST', 'GET'])
def p1():
    change_on_field(int(request.path.split('/')[-1]))
    parsed,t,team_name = load_data()
    #redirect(url_for(landing.demo))
    return redirect(url_for('.demo'))
    #return render_template('demo.html',players=parsed,team_name=team_name,time=t,page_title='Demo')
