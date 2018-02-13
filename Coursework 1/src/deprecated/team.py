import json

class Player:
    def __init__(self, name, condition, number):
        self.name = name
        self.condition = condition
        self.on_field = False
        self.number = number
        self.injury_time = None

    def get_condition(self):
        return self.condition
    def get_number(self):
        return self.number
    def get_name(self):
        return self.name
    def get_on_field(self):
        return self.on_field
    def get_injury_time(self):
        return self.injury_time


    def update_player(self, on_field=None, condition=None, injury_time=None):
        if on_field is not None:
            self.on_field = on_field
        if condition is not None:
            self.condition = condition
        if injury_time is not None:
            self.injury_time = injury_time
        return True

    def get_player(self):
        p = {}
        p["name"] = self.name
        p["condition"] = self.condition
        p["on_field"] = self.on_field
        p["number"] = self.number
        p["injury_time"] = self.injury_time
        return p


class Team:
    def __init__(self, json_file):
        self.json_file = json_file
        self.players = []
        self.size = -1
        self.name = None
        self.__load_players()

    def __load_players(self):
        data = json.load(open(self.json_file))
        self.size = int(data["size"])
        self.name = data["team"]
        for i in range(1,self.size+1):
            player = Player(data[str(i)]["name"], data[str(i)]["condition"], data[str(i)]["number"])
            self.players.append(player)

    def update_player(self, gui_index=None ,name=None, number=None, on_field=None, condition=None, injury_time=None):
        if name is None and number is None and gui_index is None:
            raise Exception("No player selected!")

        if name is not None:
            for i in range(self.size):
                if self.players[i].get_name() == name:
                    return self.players[i].update_player(on_field, condition,injury_time)

        if gui_index is not None:
            return self.players[gui_index].update_player(on_field, condition,injury_time)

        if number is not None:
            for i in range(self.size):
                if self.players[i].get_number() == number:
                    return self.players[i].update_player(on_field, condition,injury_time)

    def get_name(self):
        return self.name
    def get_size(self):
        return self.size

    def get_player(self,gui_index=None ,name=None, number=None):
        if name is None and number is None and gui_index is None:
            raise Exception("No player selected!")

        if name is not None:
            for i in range(self.size):
                if self.players[i].get_name() == name:
                    return self.players[i].get_player()

        if gui_index is not None:
            return self.players[gui_index].get_player()

        if number is not None:
            for i in range(self.size):
                if self.players[i].get_number() == number:
                    return self.players[i].get_player()
