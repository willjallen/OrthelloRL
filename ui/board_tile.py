import math
from tkinter import *
from tkinter import ttk
from ctypes import *
from othello import Othello


class BoardTile():
    def __init__(self, gui, canvas, row, col, player, val):
        self._gui = gui
        self._canvas = canvas
        self._id = None
        self._row = row
        self._col = col
        self._player = player
        self._val = val
    
    def draw(self, drawX1, drawY1, drawX2, drawY2, fill, state, outline):
        id = self._canvas.create_oval(
                drawX1,
                drawY1,
                drawX2,
                drawY2, 
                fill=fill,
                state=state,
                outline=outline,
                width=4
            )
        self._id = id

        
    def update(self, val):
        self._val = val
        if(val == 0):
            fill = 'black'
            state = 'hidden'
            outline = ''
        elif(val == 1):
            fill = 'black'
            state = 'normal'
            outline = ''
        elif(val == 2):
            fill = 'white'
            state = 'normal'
            outline = ''
        elif(val == 8):
            state = 'normal'
            fill = ''
            
            if(self._gui.othello.currentPlayer == 1):
                outline = 'black'
            else:
                outline = 'white'
        self._canvas.itemconfigure(self._id, fill=fill, state=state, outline=outline)
