import math
from distutils.log import info
from tkinter import *
from tkinter import ttk
from cgi import test
from ctypes import *
from tokenize import String
from turtle import width
import random
import json

def print_board(othello):
    print('Player: ', othello.currentPlayer)
    for row in range(0, 8):
        for col in range(0, 8):
            print(str(othello.board[row][col]) + ' ', end='')
        print()
    print()

class Othello():
    def __init__(self):
        # Load library
        self._lib = WinDLL("C:\\Users\\WilliamAllen\\Desktop\\School\\thesis\\Othello\\build\\Debug\\othello.dll") # Not fine
        
        # Create the game state
        self.board = ((c_int * 8) * 8)()
        
        self.currentPlayer = 1
        
    def player_at(self, row, col):
        return self.board[row][col]

    def set_current_player(self, player):
        if(player != 1 and player != 2):
            raise Exception('Player must have value 1 or 2')
        self.currentPlayer = player


    def set_board_from_string(self, boardStr):
        if len(boardStr) != 64:
            raise Exception('Length of string is not 64')

        for x in range(0, 8):
            for y in range(0, 8):
                self.board[x][y] = c_int(int(boardStr[(8*x)+y]))

    def get_board_as_string(self):
        boardStr = ''
        for x in range(0, 8):
            for y in range(0, 8):
                boardStr += str(self.board[x][y])
                
        return boardStr
        

    def calculate_legal_moves(self):
        # https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
        self._lib.calculateLegalMoves.argtypes = (((c_int * 8) * 8), c_int)
        self._lib.calculateLegalMoves(self.board, c_int(self.currentPlayer))
        
    def play_move(self, row, col):
        self._lib.playMove.argtypes = (((c_int * 8) * 8), c_int, c_int, c_int)
        self._lib.playMove(self.board, c_int(row), c_int(col), c_int(self.currentPlayer))
        
 

# a subclass of Canvas for dealing with resizing of windows
class ResizingCanvas(Canvas):
    def __init__(self,parent,**kwargs):
        Canvas.__init__(self,parent,**kwargs)
        self.bind("<Configure>", self.on_resize)
        self.height = self.winfo_reqheight()
        self.width = self.winfo_reqwidth()

    def on_resize(self,event):
        # determine the ratio of old width/height to new width/height
        wscale = float(event.width)/self.width
        hscale = float(event.height)/self.height
        self.width = event.width
        self.height = event.height
        # resize the canvas 
        
        # This \/ causes the window the expand upon launch, maybe an issue to not include idk 
        # self.config(width=self.width, height=self.height)
        
        # rescale all the objects tagged with the "all" tag
        self.scale("all",0,0,wscale,hscale)


class GUITile():
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
                outline=outline
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
    


class GUI():
    def __init__(self, root, othello):
        
        self.othello = othello
        
        root.title("Othello")
        
        frame = Frame(root)
        # frame.pack(fill=BOTH, expand=YES)
        frame.grid(column=0, row=0, sticky=(N, W, E, S))
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        
        
        
        
        self.gameCanvas = ResizingCanvas(frame, width=400, height=400, bg="green")
        self.gameCanvas.grid(column=0, row=0, sticky=(N,E,S,W))
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(0, weight=1)

        infoFrame = Frame(frame)
        infoFrame.grid(row=0,column=1)
        
        ttk.Label(infoFrame, text="Current Player: 1 (Black)").grid(column=0, row=0, sticky=N)
        ttk.Label(infoFrame, text="Black tiles: ?").grid(column=0, row=1, sticky=N)
        ttk.Label(infoFrame, text="White tiles: ?").grid(column=0, row=2, sticky=N)

        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(1, weight=1)
        

        self.GUIBoard = GUIBoard(self)
        
        self.GUIBoard.create_board()
    


class GUIBoard():
    def __init__(self, gui):
        self.gui = gui
        self.othello = gui.othello
        self.gameCanvas = gui.gameCanvas
        
        self.guiTiles = []
        self.canvasWidth = self.gameCanvas.width
        self.canvasHeight = self.gameCanvas.height
        
        self.cellLinesWidth = 4
                
        # interval between 8 lines
        self.widthInterval = ((self.canvasWidth - 2*self.cellLinesWidth) / 8)
        self.heightInterval = ((self.canvasHeight - 2*self.cellLinesWidth) / 8)
        
        self.guiTileVerticalPadding = self.heightInterval / 4 
        self.guiTileHorizontalPadding = self.widthInterval / 4 

        self.guiTileWidth = self.widthInterval / 2
        self.guiTileHeight = self.heightInterval / 2
        self.guiTileScaleFactor = 1
    
    
    # Done once 
    def create_board(self):
            
        

        drawX = drawY = self.cellLinesWidth
        
        # Draw 8x8 grid
        for row in range(0, 9):
            self.gameCanvas.create_line(0, drawY, self.canvasWidth, drawY, width=self.cellLinesWidth)
            drawY += self.heightInterval
        
        for col in range(0, 9):
            self.gameCanvas.create_line(drawX, 0, drawX, self.canvasHeight, width=self.cellLinesWidth)
            drawX += self.widthInterval
            


        # Draw tiles

      
        
        drawX = drawY = self.cellLinesWidth

        for row in range(0, 8):
            for col in range(0, 8):

                currOthelloTile = self.othello.board[row][col]
                guiTile = GUITile(gui=self, canvas=self.gameCanvas, row=row, col=col, player=self.othello.currentPlayer, val=currOthelloTile)
                self.guiTiles.append(guiTile)
                
                if(currOthelloTile == 0):
                    fill = 'black'
                    state = 'hidden'
                    outline = ''
                elif(currOthelloTile == 1):
                    fill = 'black'
                    state = 'normal'
                    outline = ''
                elif(currOthelloTile == 2):
                    fill = 'white'
                    state = 'normal'
                    outline = ''
                elif(currOthelloTile == 8):
                    state = 'normal'
                    fill = ''
                    if(self.othello.currentPlayer == 1):
                        outline = 'black'
                    else:
                        outline = 'white'
                        

                guiTile.draw(
                        drawX + self.guiTileHorizontalPadding,
                        drawY + self.guiTileVerticalPadding,
                        drawX + self.guiTileHorizontalPadding + self.guiTileWidth,
                        drawY + self.guiTileVerticalPadding + self.guiTileHeight,
                        fill,
                        state,
                        outline
                    )
                drawX += self.widthInterval
                
            drawX = self.cellLinesWidth
            drawY += self.heightInterval

        self.gameCanvas.addtag_all("all")
        self.gameCanvas.bind("<Button-1>", self.on_click)
    
    def on_click(self, event):
        row, col = self.findTile(event.x, event.y)
        print(row, col)
        self.play_tile(row, col)

       
    def update_board(self):
        for row in range(0, 8):
            for col in range(0, 8):
                    self.guiTiles[8*row + col].update(self.othello.board[row][col])

                        
    def play_tile(self, row, col):
        if(self.othello.board[row][col] == 8):
            self.othello.play_move(row, col)
            print('Played move')
            print_board(self.othello)
            self.next_turn()
   
    def next_turn(self):
        self.othello.currentPlayer = 1 if self.othello.currentPlayer == 2 else 2
        print('Next player')
        self.othello.calculate_legal_moves()
        print('Legal moves')
        print_board(self.othello)
        self.update_board() 

    def findTile(self, canvasX, canvasY):
        return math.floor(canvasY / self.heightInterval), math.floor(canvasX / self.widthInterval)

othello = Othello()

opening_board = "0000000000000000000010000001100000012222001221000101000000000000"
othello.set_board_from_string(opening_board)
othello.set_current_player(2)
print_board(othello)
othello.calculate_legal_moves()
print()
print_board(othello)

# for x in range(0, 8):
#     for y in range(0, 8):
#         print(othello._board[x][y])

root = Tk()
GUI(root, othello)
root.mainloop()



    
    