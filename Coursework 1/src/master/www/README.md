# HeadAid Website

## Requires
* python2, libpq-dev, python-dev, python-pip
* requirements.txt


## Virtualenv
* Run setup_env.sh to set up virtualenv
* pip install -U pip setuptools

## Organisation

The web applications are organised into blueprints:
1) __init__.py - In the web folder, used to create the app from main.py

The main entry point (which creates the Flask app) is in `main.py` (subject to change).


## Gunicorn & nginx

https://www.digitalocean.com/community/tutorials/how-to-serve-flask-applications-with-gunicorn-and-nginx-on-ubuntu-14-04

source pyenv/bin/activate

sudo gunicorn --bind 0.0.0.0:8081 main:app

1. Copy webui.super.conf to /etc/supervisord.conf
2. Copy webui.nginx.conf to /etc/nginx/sites-available/webui
3. sudo ln -s /etc/nginx/sites-available/webui /etc/nginx/sites-enabled
4. sudo nginx -t # to check for errors
5. sudo service nginx restart

``` bash
sudo cp frisch/website/server_config/webui.super.conf /etc/supervisord.conf
sudo cp frisch/website/server_config/webui.nginx.conf /etc/nginx/sites-available/webui
sudo ln -s /etc/nginx/sites-available/webui /etc/nginx/sites-enabled
sudo nginx -t
sudo service nginx restart
```


### supervisord

http://supervisord.org/introduction.html

1. Ensure supervisor is installed
2. Ensure supervisord.conf file is present at /etc/supervisord.conf
3. Make logs directories
```
mkdir logs
touch logs/stdout.log
touch logs/stderr.log
```

Run as root: (su - root)
`/home/frisch/website/pyenv/bin/supervisord`

supervisorctl reload
supervisorctl status all
supervisorctl stop all
supervisorctl start all
