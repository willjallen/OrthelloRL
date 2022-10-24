import random
from tkinter import *
from cgi import test
from ctypes import *
from othello import Othello
from ui.gui import GUI
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

        
        legal_moves_black = test_case['legal_moves_black']
        legal_moves_white = test_case['legal_moves_white']
        
        if(len(legal_moves_black) > 0):
            othello.set_current_player(2)
            othello.calculate_legal_moves()
            engine_legal_moves_black = othello.get_board_as_string()



            
            if(legal_moves_black != engine_legal_moves_black):
                print('Test case failed')
                print("Board state: " + test_case['board_state'])
                print("Black legal moves engine: ")
                print_board(othello)
                print("Black legal moves test: ")
                for row in range(0, 8):
                    for col in range(0, 8):
                        print(legal_moves_black[8*row + col], end=' ')
                    print()
            else:
                print('Passed')
                
        # if(len(legal_moves_white) > 0):
        #     othello.set_current_player(2)
        #     othello.calculate_legal_moves()
        #     engine_legal_moves_white = othello.get_board_as_string()

        #     print("White legal moves engine: " + engine_legal_moves_white)
        #     print("White legal moves test: " + legal_moves_white)

            
        #     assert legal_moves_white == engine_legal_moves_white


def get_random_move(board):
    potential_moves = []
    for row in range(0, 8):
        for col in range(0, 8):
            if(board[row][col] == 8):
                potential_moves.append((row, col))
    
    return random.choice(potential_moves)
    


def test_play_random(GUI_ENABLE=True):
    othello = Othello()
    
    gui = None
    if(GUI_ENABLE):
        root = Tk()
        gui = GUI(root, othello)
        default_board = "0000000000000000000000000001200000021000000000000000000000000000"
        othello.set_board_from_string(default_board)
        othello.set_current_player(1)
        randomPlayerContainer = RandomPlayContainer(gui, othello)
        root.after(0, randomPlayerContainer.random_play)
        root.mainloop()



    
    
class RandomPlayContainer():
    def __init__(self, gui, othello):
        self.gui = gui
        self.othello = othello    

    def random_play(self):
        self.othello.calculate_legal_moves()

        if(self.gui):
            self.gui.update()

        if(self.othello.legal_move_present()):
            move_choice = get_random_move(self.othello.board)
            self.othello.play_move(move_choice[0], move_choice[1])    
        self.othello.switch_player()

        self.gui.root.after(200, self.random_play)

test_cases()

# test_play_random()