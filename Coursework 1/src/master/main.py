from team import Team
from GUI import GUI

JSON_FILE = "../../data/team/team.json"
IMG_FILES = "img/"
SCREEN_WIDTH = 1500
SCREEN_HEIGHT = 400

team = Team(JSON_FILE)

gui = GUI(team, IMG_FILES, SCREEN_WIDTH, SCREEN_HEIGHT)

gui.update()
gui.loop()
