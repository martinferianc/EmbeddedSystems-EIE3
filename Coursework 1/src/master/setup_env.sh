#! /bin/bash
## Execute setup_env from CRM folder
## python, postgresql, postgresql-server-dev-9.4, python-dev, python-pip, python-virtualenv
# check if venv exists, if it doesn't, create the venv
virtualenv pyenv
#activate the virtual environment
source pyenv/bin/activate
#install all python packages from the list requirements.txt
pip install -U pip setuptools
pip install -r requirements.txt
