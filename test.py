from main import Othello
from tkinter import *
from cgi import test
from ctypes import *
import json

def print_board(othello):
    print('Player: ', othello.currentPlayer)
    for row in range(0, 8):
        for col in range(0, 8):
            print(str(othello.board[row][col]) + ' ', end='')
        print()
    print()

def test_cases():

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
            othello.set_current_player(2)
            othello.calculate_legal_moves()
            engine_legal_moves_black = othello.get_board_as_string()

            print("Black legal moves engine: ")
            print_board(othello)
            print("Black legal moves test: ")
            for row in range(0, 8):
                for col in range(0, 8):
                    print(legal_moves_black[8*row + col], end=' ')
                print()

            
            assert legal_moves_black == engine_legal_moves_black
            
        if(len(legal_moves_white) > 0):
            othello.set_current_player(2)
            othello.calculate_legal_moves()
            engine_legal_moves_white = othello.get_board_as_string()

            print("White legal moves engine: " + engine_legal_moves_white)
            print("White legal moves test: " + legal_moves_white)

            
            assert legal_moves_white == engine_legal_moves_white

def test_play_random():
    othello = Othello()
    
    default_board = "0000000000000000000000000001200000021000000000000000000000000000"
    othello.get_board_as_string(default_board)
    

test()