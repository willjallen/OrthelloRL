import math
from tkinter import *
from tkinter import ttk
from ctypes import *
from othello import Othello
from ui.gui import GUI

def print_board(othello):
    print('Player: ', othello.currentPlayer)
    for row in range(0, 8):
        for col in range(0, 8):
            print(str(othello.board[row][col]) + ' ', end='')
        print()
    print()


othello = Othello()

default_borad = "0000000000000000000000000001200000021000000000000000000000000000"
# opening_board = "0000000000000000000010000001100000012222001221000101000000000000"
othello.set_board_from_string(default_borad)
othello.set_current_player(1)
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



    
    