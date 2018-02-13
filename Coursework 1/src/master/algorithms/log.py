import json

def log_event(data, label, file_path = "../../data/team/"):
    team_f = open(file_path+"team.json", 'r')
    team_data = json.load(team_f)
    team_f.close()

    condition = None
    message = None
    if label == 1:
        condition = "bad"
        message = "The player has been badly hit, there is moderate proababilty of concussion."
    elif label == 2:
        condition = "critical"
        message = "The player has been critically hit, there is high proababilty of concussion."

    log_f = open(file_path+"log.json", 'r')
    log_data = json.load(log_f)
    log_f.close()

    name = team_data[str(data["PLAYER"])]["name"]
    number = team_data[str(data["PLAYER"])]["number"]

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

    team_data[str(data["PLAYER"])]["condition"] = condition
    team_f = open(file_path+"team.json", 'w')
    json.dump(team_data, team_f)
    team_f.close()
