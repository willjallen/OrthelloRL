import math
from tkinter import *
from tkinter import ttk
from ctypes import *

from othello import Othello
from ui.board_tile import BoardTile

class Board():
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
                guiTile = BoardTile(gui=self, canvas=self.gameCanvas, row=row, col=col, player=self.othello.currentPlayer, val=currOthelloTile)
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

       
    def update(self):
        for row in range(0, 8):
            for col in range(0, 8):
                    self.guiTiles[8*row + col].update(self.othello.board[row][col])

                        
    def play_tile(self, row, col):
        if(self.othello.board[row][col] == 8):
            self.othello.play_move(row, col)
            print('Played move')
            # print_board(self.othello)
            self.next_turn()
   
    def next_turn(self):
        self.othello.currentPlayer = 1 if self.othello.currentPlayer == 2 else 2
        print('Next player')
        self.othello.calculate_legal_moves()
        print('Legal moves')
        # print_board(self.othello)
        self.update() 

    def findTile(self, canvasX, canvasY):
        return math.floor(canvasY / self.heightInterval), math.floor(canvasX / self.widthInterval)
