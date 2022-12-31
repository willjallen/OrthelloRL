import math
from tkinter import *
from tkinter import ttk
from ctypes import *
from othello import Othello


class BoardTile():
    def __init__(self, gui, canvas, othello):
        self._gui = gui
        self._canvas = canvas
        self._id = None
        self.othello = othello

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

    def update(self, playerTile, legalTile):
        if(legalTile == 8):
            state = 'normal'
            fill = ''
            if(self.othello.gameState.currentPlayer == 1):
                outline = 'black'
            else:
                outline = 'white'
        elif(playerTile == 0):
            fill = 'black'
            state = 'hidden'
            outline = ''
        elif(playerTile == 1):
            fill = 'black'
            state = 'normal'
            outline = ''
        elif(playerTile == 2):
            fill = 'white'
            state = 'normal'
            outline = ''
        self._canvas.itemconfigure(
            self._id, fill=fill, state=state, outline=outline)
