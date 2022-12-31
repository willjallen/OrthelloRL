import platform
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
    _fields_ = [  ("currentPlayer", c_uint),
                  ("turnNumber", c_uint),
                  ("numBlackTiles", c_uint),
                  ("numWhiteTiles", c_uint),
                  ("numBlackLegalMoves", c_uint),
                  ("numWhiteLegalMoves", c_uint),
                  ("board", (c_uint * 8) * 8),
                  ("legalMoves", (c_uint * 8) * 8),
                  ("moveLines", (MOVELINES * 8) * 8)]

DEFAULT_BOARD = "0000000000000000000000000001200000021000000000000000000000000000"


class Othello():
    def __init__(self):
        
        # Detect OS
        print(platform.system())
        # Load library
        if(platform.system() == "Linux"):
            self._lib = CDLL("./build/libothello_lib.so")
        elif(platform.system() == "Windows"):
            self._lib = WinDLL("./build/Release/othello_lib.dll")

        self.gameState = GAMESTATE()
        self.gameState_ptr = pointer(self.gameState)
        

        self._lib.init.argtypes = [POINTER(GAMESTATE)]
        self._lib.calculateLegalMoves.argtypes = [POINTER(GAMESTATE)]
        self._lib.playMove.argtypes = (POINTER(GAMESTATE), c_int, c_int)
        self._lib.playRandomMove.argtypes = [POINTER(GAMESTATE)]
        self._lib.switchPlayers.argtypes = [POINTER(GAMESTATE)]

        
        self._lib.init(self.gameState)
        
        self.board = self.gameState.board
        self.legal_moves = self.gameState.legalMoves
        

    def set_board_from_string(self, boardStr):
        if len(boardStr) != 64:
            raise Exception('Length of string is not 64')

        for x in range(0, 8):
            for y in range(0, 8):
                self.board[x][y] = c_uint(int(boardStr[(8*x)+y]))


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
        self._lib.calculateLegalMoves(self.gameState_ptr)
        
    def play_move(self, row, col):
        self._lib.playMove(self.gameState_ptr, c_int(row), c_int(col))
        
    def play_random_move(self):
        self._lib.playRandomMove(self.gameState_ptr)

    def switch_players(self):
        self._lib.switchPlayers(self.gameState_ptr)

    def init(self):
        self._lib.init(self.gameState_ptr)
