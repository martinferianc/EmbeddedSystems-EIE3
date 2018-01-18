from . import landing

import os
import datetime
import flask
import logging

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
    return render_template('demo.html', page_title='Demo')
