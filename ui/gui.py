import math
from tkinter import *
from tkinter import ttk
from ctypes import *
from othello import Othello
from ui.resizing_canvas import ResizingCanvas
from ui.board import Board

class GUI():
    def __init__(self, root, othello):
        
        self.othello = othello
        self.root = root
        
        root.title("Othello")
        
        frame = Frame(root)
        # frame.pack(fill=BOTH, expand=YES)
        frame.grid(column=0, row=0, sticky=(N, W, E, S))
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        
        
        
        
        self.gameCanvas = ResizingCanvas(frame, width=800, height=800, bg="green")
        self.gameCanvas.grid(column=0, row=0, sticky=(N,E,S,W))
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(0, weight=1)

        infoFrame = Frame(frame)
        infoFrame.grid(row=0,column=1)
        
        self.player_label = ttk.Label(infoFrame, text="Current Player: 1 (Black)")
        self.player_label.grid(column=0, row=0, sticky=N)
        # ttk.Label(infoFrame, text="Black tiles: ?").grid(column=0, row=1, sticky=N)
        # ttk.Label(infoFrame, text="White tiles: ?").grid(column=0, row=2, sticky=N)
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(1, weight=1)
        

        self.board = Board(self)
        
        self.board.create_board()

    def update(self):
        self.board.update()

        self.player_label.configure(text= 
                                    "Current Player: 1 (Black)"
                                    if self.othello.gameState.currentPlayer == 1 else
                                    "Current Player: 2 (White)"                                    
                                    )
# itemconfigure(self._id, fill=fill, state=state, outline=