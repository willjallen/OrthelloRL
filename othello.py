from os import curdir
from tkinter import *
from ctypes import *


class LINE(Structure):
    _fields_ = [("assignedTurnNumber", c_uint),
                ("valid", c_bool),
                ("langth", c_uint)]

class MOVELINES(Structure):
    _fields_ = [("lines", LINE*8)] 

class GAMESTATE(Structure):
    _fields_ = [("turnNumber", c_uint),
                  ("numBlackTiles", c_uint),
                  ("numWhiteTiles", c_uint),
                  ("numBlackLegalMoves", c_uint),
                  ("numWhiteLegalMoves", c_uint),
                  ("board", (c_uint * 8) * 8),
                  ("legalMoves", (c_uint * 8) * 8),
                  ("moveLines", (MOVELINES * 8) * 8)]


class Othello():
    def __init__(self):
        # Load library
        self._lib = WinDLL("./build/Debug/othello_lib.dll")
        
        # Create the game state
        # self.board = ((c_int * 8) * 8)()

        
        self.gameState = GAMESTATE()
        
        
        self._lib.init.argtypes = [POINTER(GAMESTATE)]
        self._lib.init(self.gameState)
        
        self.board = self.gameState.board
        self.legal_moves = self.gameState.legalMoves
        
        
        self.currentPlayer = 1

        self.playerOneTiles = 2
        self.playerTwoTiles = 2
        
    def increment_tile_count(self):
        if self.currentPlayer == 1:
            self.playerOneTiles += 1
        else:
            self.playerTwoTiles += 1
        
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
                self.board[x][y] = c_uint(int(boardStr[(8*x)+y]))

    def legal_move_present(self):
        for x in range(0, 8):
            for y in range(0, 8):
                if(self.legal_moves[x][y] == 8): return True
        return False

    def get_board_as_string(self):
        boardStr = ''
        for x in range(0, 8):
            for y in range(0, 8):
                boardStr += str(self.board[x][y])
                
        return boardStr
        
    def get_legal_moves_as_string(self):
        boardStr = ''
        for x in range(0, 8):
            for y in range(0, 8):
                boardStr += str(self.legal_moves[x][y])
                
        return boardStr
        
        
    def calculate_legal_moves(self):
        # https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
        self._lib.calculateLegalMoves.argtypes = (POINTER(GAMESTATE), c_uint)
        self._lib.calculateLegalMoves(pointer(self.gameState), c_uint(self.currentPlayer))
        
    def play_move(self, row, col):
        self._lib.playMove.argtypes = (POINTER(GAMESTATE), c_int, c_int, c_uint)
        self._lib.playMove(pointer(self.gameState), c_int(row), c_int(col), c_uint(self.currentPlayer))
        self.increment_tile_count()
        