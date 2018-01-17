from team import Team
from GUI import GUI
import random
import time

JSON_FILE = "../../data/team/team.json"
IMG_FILES = "img/"
SCREEN_WIDTH = 1500
SCREEN_HEIGHT = 400

team = Team(JSON_FILE)

gui = GUI(team, IMG_FILES, SCREEN_WIDTH, SCREEN_HEIGHT)

def main_loop():
    #In case of an injury
    i = random.randint(0,19)
    t = time.strftime('%H:%M:%S')
    team.update_player(gui_index = i, condition="critical", injury_time=t)
    gui.alert(gui_index=i)

    time.sleep(3)
    gui.update(f_handle=main_loop)

gui.update(f_handle=main_loop)
gui.loop()
