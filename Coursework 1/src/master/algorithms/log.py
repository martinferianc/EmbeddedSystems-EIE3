import json
import time

def log_event(data, label, file_path = "../../data/team/"):
    team_f = open(file_path+"team.json", 'r')
    team_data = json.load(team_f)
    team_f.close()

    condition = None
    message = None
    if label == 2:
        condition = "bad"
        message = "The player has been badly hit, there is moderate proababilty of concussion."
    elif label == 3:
        condition = "critical"
        message = "The player has been critically hit, there is high proababilty of concussion."

    log_f = open(file_path+"log.json", 'r')
    log_data = json.load(log_f)
    log_f.close()
    player_index = str(data["PLAYER"])

    name = team_data[player_index]["name"]
    number = team_data[player_index]["number"]
    player_condition = team_data[player_index]["condition"]

    # No Change
    if condition == player_condition or (condition=="bad" and player_condition="critical"):
        return

    new_log = {
                "name": name,
                "number": number,
                "condition": condition,
                "message": message,
                "time": str(time.ctime())
               }
    log_data[str(len(log_data))] = new_log
    log_f = open(file_path+"log.json", 'w')
    json.dump(log_data, log_f)
    log_f.close()

    team_data[player_index]["condition"] = condition
    team_f = open(file_path+"team.json", 'w')
    json.dump(team_data, team_f)
    team_f.close()


def check_on_field(file_path = "../../data/team/", size=20):
    team_f = open(file_path+"team.json", 'r')
    team_data = json.load(team_f)
    team_f.close()

    size = team_data["size"]
    players = [0 for i in range(size)]
    for i in range(1,int(size)+1):
        if team_data[str(i)]["on_field"] == "yes":
            players[i] = 1
    return players
