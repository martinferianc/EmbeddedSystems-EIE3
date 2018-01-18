from flask import Blueprint

landing = Blueprint('landing', __name__)

from . import views
