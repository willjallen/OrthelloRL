# import tkinter as tk

# class App(tk.Frame):
#     def __init__(self, master=None):
#         super().__init__(master)
#         self.pack()

# # create the application
# myapp = App()

# #
# # here are method calls to the window manager class
# #
# myapp.master.title("My Do-Nothing Application")
# myapp.master.maxsize(1000, 400)

# # start the program
# myapp.mainloop()



from cgi import test
from ctypes import *


lib = WinDLL("C:\\Users\\WilliamAllen\\Desktop\\School\\thesis\\Othello\\othello.dll") # Not fine
print(lib)

testBoard = ((c_int * 8) * 8)()

# print(testBoard[9][0])

testBoard[3][3] = 1;
testBoard[4][3] = 2;
testBoard[3][4] = 2;
testBoard[4][4] = 1;

print('array ptr: ', byref(testBoard))

for i in testBoard:
    for j in i:
        print(j, end='')
    print('')
    

player = c_int(2)

# https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
lib.calculateLegalMoves.argtypes = (((c_int * 8) * 8), c_int)
lib.calculateLegalMoves(testBoard, player);

print()

for i in testBoard:
    for j in i:
        print(j, end='')
    print('')
    
    
    