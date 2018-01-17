class Player:
    def __init__(self, name, condition, number):
        self.name = name
        self.condition = condition
        self.on_field = False
        self.number = number

    def get_condition(self):
        return self.condition
    def get_number(self):
        return self.number
    def get_name(self):
        return self.name
    def get_on_field(self):
        return self.on_field

    def update_player(self, on_field, condition):
        if on_field is not None:
            self.on_field = on_field
        if condition is not None:
            self.condition = condition
        return True

    def get_player(self):
        p = {}
        p["name"] = self.name
        p["condition"] = self.condition
        p["on_field"] = self.on_field
        p["number"] = self.number
        return p
