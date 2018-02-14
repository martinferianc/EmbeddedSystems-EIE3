# Master program

This code runs on the server side which is configured as the MQTT broker to receive the messages. It creates a web server which updates a website based on the data which is read from the accelerometer, clearly allowing a coach or other support personnel to identify when a player has suffered a trauma. The script contained within `main.py` handles the creation of the web sever and the MQTT server to listen to incoming messages from all of the connected players.

## Structure

```
.
├── algorithms
└── www
    └── web
        ├── landing
        ├── static
        │   ├── css
        │   ├── img
        │   ├── js
        │   ├── scss
        │   └── vendor
        │       ├── bootstrap
        │       ├── font-awesome
        │       └── jquery
        └── templates
```
The `algorithms` foldter contains all the other programs needed to filter out the data or enable connectivity with the web server. The main directory contains `main.py` which combines all the sub programs into the main loop that runs the server and all pooling to the server.

`www` contains all the necessary stuff for the web server to run. `landing` contains the main configuration file for the web server where all URL linking takes place and where the server responds to POST or GET requersts. `static` contains all the static data and libraries that we use to run the web server, the main css bootstrap enhanced template is under `static/css/`. `tempaltes` contains the HTML templates for the websites that we run.

## Building & Running

Make sure that you have all the dependencies:

`pip install -r requierments.txt`

Then just:

`python3 main.py "0.0.0.0" 8080 "192.168.0.183" 1883`

to setup the host address and the port on which the server is going to listen and you are good to go.
