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
