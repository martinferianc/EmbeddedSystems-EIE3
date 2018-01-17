from tkinter import *
from PIL import ImageTk
from PIL import Image
import time

class GUI():
    def __init__(self, team, img_path, x,y):
        self.root = Tk()
        self.x = x
        self.y = y
        self.team = team
        self.img_path = img_path
        self.canvas = Canvas(width = x, height = y, bg="#FFFFFF")
        self.current_time = None

        self.player_img = None
        self.red_cross_img = None
        self.yellow_cross_img = None
        self.green_cross_img = None

        self.player_box_x = 150
        self.player_box_x_offset = 10
        self.player_box_y = 170
        self.player_box_y_offset = 50

        self.canvas_conditions = []
        self.canvas_on_fields = []
        self.canvas.bind("<Button-1>", self.__on_field)

        self.__initialize_board()
        self.__draw_players()


    def __initialize_board(self):
        n = self.team.get_size()
        name = self.team.get_name()
        self.canvas.create_text(self.x/2 - 30, 15, text = name,font=("Helvetica", 18))
        self.clock = self.canvas.create_text(self.x - 100, 10, font=('Helvetica', 16))

    def __draw_player(self,x,y,h,w, condition, on_field, name, number):
        if self.player_img is None:
            self.player_img = Image.open(self.img_path+"player.gif")
            self.player_img = ImageTk.PhotoImage(self.player_img)

        self.canvas.create_image(x+60,y+60,image=self.player_img, anchor="center")
        self.canvas.create_text(x + 40, y+4, font=('Helvetica', 13), text=name)
        self.canvas.create_text(x + 10, y+h - 30, font=('Helvetica', 16), text=number)

        if self.red_cross_img is None:
            self.red_cross_img = Image.open(self.img_path+"red_cross.gif")
            self.red_cross_img = ImageTk.PhotoImage(self.red_cross_img)
        if self.yellow_cross_img is None:
            self.yellow_cross_img = Image.open(self.img_path+"yellow_cross.gif")
            self.yellow_cross_img = ImageTk.PhotoImage(self.yellow_cross_img)
        if self.green_cross_img is None:
            self.green_cross_img = Image.open(self.img_path+"green_cross.gif")
            self.green_cross_img = ImageTk.PhotoImage(self.green_cross_img)

        i = None
        if condition == "good":
            i = self.canvas.create_image(x+w-70,y+h-50,image=self.green_cross_img)
        elif condition == "neutral":
            i = self.canvas.create_image(x+w-70,y+h-50,image=self.yellow_cross_img)
        elif condition == "bad":
            i = self.canvas.create_image(x+w-70,y+h-50,image=self.red_cross_img)
        self.canvas_conditions.append(i)

        if on_field is True:
            i=self.canvas.create_oval(x+w-65-10,y+5-10,x+w-65+10,y+10+5,fill="#0000FF")
        else:
            i=self.canvas.create_oval(x+w-65-10,y+5-10,x+w-65+10,y+10+5,fill="#FF0000")
        self.canvas_on_fields.append(i)

    def __draw_players(self):
        n = self.team.get_size()
        x = self.player_box_x_offset
        y = self.player_box_y_offset
        players = []
        for i in range(n):
            players.append(self.team.get_player(gui_index=i))

        for player in players:
            self.__draw_player(x,y,self.player_box_x,self.player_box_y, player["condition"], player["on_field"], player["name"], player["number"])
            x+=140
            if x>=1400:
                x=self.player_box_x_offset
                y+=170

    def __on_field(self,event):
        i = (event.x-self.player_box_x_offset) // self.player_box_x
        if (event.y-self.player_box_y_offset) // self.player_box_y >=1:
            i+=10
        on_field = self.team.get_player(gui_index=i)["on_field"]
        if on_field:
            on_field = False
            fill = "#FF0000"
        else:
            on_field = True
            fill = "#00FF00"
        self.team.update_player(gui_index=i, on_field= on_field)
        self.canvas.itemconfig(self.canvas_on_fields[i],fill=fill)

    def __tick(self):
        t = time.strftime('%H:%M:%S')
        if self.current_time != t:
            self.current_time = t
            self.canvas.itemconfig(self.clock,text=self.current_time)
        self.canvas.after(200, self.__tick)

    def loop(self):
        self.__tick()
        self.root.mainloop()

    def update(self):
        self.canvas.pack()
