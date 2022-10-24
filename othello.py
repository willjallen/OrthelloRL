from tkinter import *
from ctypes import *

class Othello():
    def __init__(self):
        # Load library
        self._lib = WinDLL("./build/Debug/othello.dll") # Not fine
        
        # Create the game state
        self.board = ((c_int * 8) * 8)()
        
        self.currentPlayer = 1
        
    def player_at(self, row, col):
        return self.board[row][col]

    def set_current_player(self, player):
        if(player != 1 and player != 2):
            raise Exception('Player must have value 1 or 2')
        self.currentPlayer = player

    def switch_player(self):
        self.currentPlayer = 1 if self.currentPlayer == 2 else 2

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
        