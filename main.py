from tkinter import *
from tkinter import ttk
from cgi import test
from ctypes import *
from turtle import width


# lib = WinDLL("C:\\Users\\WilliamAllen\\Desktop\\School\\thesis\\Othello\\othello.dll") # Not fine
# print(lib)

# testBoard = ((c_int * 8) * 8)()

# # print(testBoard[9][0])

# testBoard[3][3] = 1;
# testBoard[4][3] = 2;
# testBoard[3][4] = 2;
# testBoard[4][4] = 1;

# print('array ptr: ', byref(testBoard))

# for i in testBoard:
#     for j in i:
#         print(j, end='')
#     print('')
    

# player = c_int(1)

# # https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
# lib.calculateLegalMoves.argtypes = (((c_int * 8) * 8), c_int)
# lib.calculateLegalMoves(testBoard, player);

# print()

# for i in testBoard:
#     for j in i:
#         print(j, end='')
#     print('')
    

class Othello():
    def __init__(self):
        # Create the game state
        self._board = ((c_int * 8) * 8)()
        
        self._current_player = c_int(1)
        
    def getTile(self, x, y):
        return self._board[x][y]



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


class GUI():
    def __init__(self, root, othello):
        
        self._othello = othello
        
        root.title("Othello")
        
        frame = Frame(root)
        # frame.pack(fill=BOTH, expand=YES)
        frame.grid(column=0, row=0, sticky=(N, W, E, S))
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        
        
        
        
        gameCanvas = ResizingCanvas(frame, width=400, height=400, bg="green")
        gameCanvas.grid(column=0, row=0, sticky=(N,E,S,W))
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(0, weight=1)

        ttk.Label(frame, text="feet").grid(column=1, row=0, sticky=N)
        frame.rowconfigure(0, weight=1)
        frame.columnconfigure(1, weight=1)
        


        # gameCanvas.pack(fill=BOTH, expand=YES)

        
        # gameCanvas = Canvas(root, width=400,height=400)

        # gameCanvas.grid(row=0, column=0, sticky=N+S+E+W)
        # gameCanvas.pack(fill="both", expand=True)

        # gameCanvas.create_line(0, 0, 200, 100)
        self.createBoard(gameCanvas)

        
    
    # Done once 
    def createBoard(self, gameCanvas):
            
        canvasWidth = gameCanvas.width
        canvasHeight = gameCanvas.height
        
        cellLinesWidth = 4
                
        # interval between 8 lines
        widthInterval = ((canvasWidth - 2*cellLinesWidth) / 8)
        heightInterval = ((canvasHeight - 2*cellLinesWidth) / 8)

        drawX = drawY = cellLinesWidth
        
        # Draw 8x8 grid
        for row in range(0, 9):
            gameCanvas.create_line(0, drawY, canvasWidth, drawY, width=cellLinesWidth)
            drawY += heightInterval
        
        for col in range(0, 9):
            gameCanvas.create_line(drawX, 0, drawX, canvasHeight, width=cellLinesWidth)
            drawX += widthInterval
            
        gameCanvas.addtag_all("all")

        # Draw tiles

        tileVerticalPadding = heightInterval / 4 
        tileHorizontalPadding = widthInterval / 4 

        tileWidth = widthInterval / 2
        tileHeight = heightInterval / 2
        tileScaleFactor = 1
        
        drawX = drawY = cellLinesWidth
        
        for row in range(0, 8):
            for col in range(0, 8):
                gameCanvas.create_oval(drawX + tileHorizontalPadding, drawY + tileVerticalPadding, drawX + tileHorizontalPadding + tileWidth, drawY + tileVerticalPadding + tileHeight, fill="black")
                drawX += widthInterval
            drawX = cellLinesWidth
            drawY += heightInterval

                
    
    def drawTiles(self):
        pass


       
        # self.feet = StringVar()
        # feet_entry = ttk.Entry(mainframe, width=7, textvariable=self.feet)
        # feet_entry.grid(column=2, row=1, sticky=(W, E))
        # self.meters = StringVar()

        # ttk.Label(mainframe, textvariable=self.meters).grid(column=2, row=2, sticky=(W, E))
        # ttk.Button(mainframe, text="Calculate", command=self.calculate).grid(column=3, row=3, sticky=W)

        # ttk.Label(mainframe, text="feet").grid(column=3, row=1, sticky=W)
        # ttk.Label(mainframe, text="is equivalent to").grid(column=1, row=2, sticky=E)
        # ttk.Label(mainframe, text="meters").grid(column=3, row=2, sticky=W)

        # for child in mainframe.winfo_children(): 
        #     child.grid_configure(padx=5, pady=5)

        # feet_entry.focus()
        # root.bind("<Return>", self.calculate)

    def calculate(self, *args):
        try:
            value = float(self.feet.get())
            self.meters.set(int(0.3048 * value * 10000.0 + 0.5)/10000.0)
        except ValueError:
            pass




root = Tk()
GUI(root, Othello())
root.mainloop()



    
    