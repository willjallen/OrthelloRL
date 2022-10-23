from distutils.log import info
from tkinter import *
from tkinter import ttk
from cgi import test
from ctypes import *
from tokenize import String
from turtle import width
import random
import json

def printBoard(othello):
    print('Player: ', othello._currentPlayer)
    for row in range(0, 8):
        for col in range(0, 8):
            print(str(othello._board[row][col]) + ' ', end='')
        print()
    print()

class Othello():
    def __init__(self):
        # Load library
        self._lib = WinDLL("C:\\Users\\WilliamAllen\\Desktop\\School\\thesis\\Othello\\build\\Debug\\othello.dll") # Not fine
        
        # Create the game state
        self._board = ((c_int * 8) * 8)()
        
        self._currentPlayer = 1
        
    def player_at(self, row, col):
        return self._board[row][col]

    def set_currentPlayer(self, player):
        if(player != 1 and player != 2):
            raise Exception('Player must have value 1 or 2')
        self._currentPlayer = c_int(player)


    def set_board_from_string(self, boardStr):
        if len(boardStr) != 64:
            raise Exception('Length of string is not 64')

        for x in range(0, 8):
            for y in range(0, 8):
                self._board[x][y] = c_int(int(boardStr[(8*x)+y]))

    def get_board_as_string(self):
        boardStr = ''
        for x in range(0, 8):
            for y in range(0, 8):
                boardStr += str(self._board[x][y])
                
        return boardStr
        

    def calculate_legal_moves(self):
        # https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
        self._lib.calculateLegalMoves.argtypes = (((c_int * 8) * 8), c_int)
        self._lib.calculateLegalMoves(self._board, c_int(self._currentPlayer))
        
    def play_move(self, row, col):
        self._lib.playMove.argtypes = (((c_int * 8) * 8), c_int, c_int, c_int)
        self._lib.playMove(self._board, c_int(row), c_int(col), c_int(self._currentPlayer))
        
            
def test():

    othello = Othello()

    with open('test_cases.json') as file:
        contents = file.read()    
        test_cases_json = json.loads(contents)
    
    print("Running test cases...")
    
    for test_case in test_cases_json['test_cases']:
        print("Test Case: " + str(test_case['num']))
        
        othello.set_board_from_string(test_case['board_state'])
        print("Board state: " + test_case['board_state'])
        
        legal_moves_black = test_case['legal_moves_black']
        legal_moves_white = test_case['legal_moves_white']
        
        if(len(legal_moves_black) > 0):
            othello.set_currentPlayer(1)
            othello.calculateLegalMoves()
            engine_legal_moves_black = othello.get_board_as_string()

            print("Black legal moves engine: " + engine_legal_moves_black)
            print("Black legal moves test: " + legal_moves_black)

            
            assert legal_moves_black == engine_legal_moves_black
            
        if(len(legal_moves_white) > 0):
            othello.set_currentPlayer(2)
            othello.calculateLegalMoves()
            engine_legal_moves_white = othello.get_board_as_string()

            print("White legal moves engine: " + engine_legal_moves_white)
            print("White legal moves test: " + legal_moves_white)

            
            assert legal_moves_white == engine_legal_moves_white



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
        self._canvas.tag_bind(id, "<Button-1>", self.on_click)
    
    def on_click(self, event):
        tile = event.widget.find_closest(event.x, event.y)
        print(tile)
        self._gui.play_tile(self._row, self._col)

        
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
            
            if(self._gui._othello._currentPlayer == 1):
                outline = 'black'
            else:
                outline = 'white'
        self._canvas.itemconfigure(self._id, fill=fill, state=state, outline=outline)
    


class GUI():
    def __init__(self, root, othello):
        
        self._othello = othello
        
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
        


        self.guiTiles = []
        self.createBoard()
    
    
    
    def play_tile(self, row, col):
        if(self._othello._board[row][col] == 8):
            self._othello.play_move(row, col)
            print('Played move')
            printBoard(self._othello)
            self.next_turn()
   
    def next_turn(self):
        self._othello._currentPlayer = 1 if self._othello._currentPlayer == 2 else 2
        print('Next player')
        self._othello.calculate_legal_moves()
        print('Legal moves')
        printBoard(self._othello)
        self.updateBoard()
        
            
    def updateBoard(self):
        for row in range(0, 8):
            for col in range(0, 8):
                    self.guiTiles[8*row + col].update(self._othello._board[row][col])
            
    # Done once 
    def createBoard(self, ):
            
        canvasWidth = self.gameCanvas.width
        canvasHeight = self.gameCanvas.height
        
        cellLinesWidth = 4
                
        # interval between 8 lines
        widthInterval = ((canvasWidth - 2*cellLinesWidth) / 8)
        heightInterval = ((canvasHeight - 2*cellLinesWidth) / 8)

        drawX = drawY = cellLinesWidth
        
        # Draw 8x8 grid
        for row in range(0, 9):
            self.gameCanvas.create_line(0, drawY, canvasWidth, drawY, width=cellLinesWidth)
            drawY += heightInterval
        
        for col in range(0, 9):
            self.gameCanvas.create_line(drawX, 0, drawX, canvasHeight, width=cellLinesWidth)
            drawX += widthInterval
            


        # Draw tiles

        guiTileVerticalPadding = heightInterval / 4 
        guiTileHorizontalPadding = widthInterval / 4 

        guiTileWidth = widthInterval / 2
        guiTileHeight = heightInterval / 2
        guiTileScaleFactor = 1
        
        drawX = drawY = cellLinesWidth

        for row in range(0, 8):
            for col in range(0, 8):

                currOthelloTile = self._othello._board[row][col]
                guiTile = GUITile(gui=self, canvas=self.gameCanvas, row=row, col=col, player=self._othello._currentPlayer, val=currOthelloTile)
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
                    if(self._othello._currentPlayer == 1):
                        outline = 'black'
                    else:
                        outline = 'white'
                        

                guiTile.draw(
                        drawX + guiTileHorizontalPadding,
                        drawY + guiTileVerticalPadding,
                        drawX + guiTileHorizontalPadding + guiTileWidth,
                        drawY + guiTileVerticalPadding + guiTileHeight,
                        fill,
                        state,
                        outline
                    )
                drawX += widthInterval
            drawX = cellLinesWidth
            drawY += heightInterval

        self.gameCanvas.addtag_all("all")

                

othello = Othello()

opening_board = "0000000000000000000000000001200000021000000000000000000000000000"
othello.set_board_from_string(opening_board)
printBoard(othello)
othello.calculate_legal_moves()
print()
printBoard(othello)

# for x in range(0, 8):
#     for y in range(0, 8):
#         print(othello._board[x][y])

root = Tk()
GUI(root, othello)
root.mainloop()



    
    