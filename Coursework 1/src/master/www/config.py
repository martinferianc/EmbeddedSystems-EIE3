import os
basedir = os.path.abspath(os.path.dirname(__file__))

class Config(object):
    DEBUG = False
    TESTING = False
    CSRF_ENABLED = True
    SECRET_KEY = 'this-is-a-secret'
    #SQLALCHEMY_DATABASE_URI = 'postgresql://webui:testpass@localhost:51001/webui'
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    CLIENT_API_KEYS = [
        ["foo", "bar"]
    ]

class DevelopmentConfig(Config):
    DEVELOPMENT = True
    DEBUG = True

class ProductionConfig(Config):
    DEBUG = False
    SECRET_KEY = 'this-is-a-secret'
    SQLALCHEMY_DATABASE_URI = 'postgresql:///webui'


config = {
    'dev': DevelopmentConfig,
    'production': ProductionConfig
}
