import random
from tkinter import *
from cgi import test
from ctypes import *
from othello_wrapper import Othello
from ui.gui import GUI
import json
import cProfile
import pstats
import io
from pstats import SortKey


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

        newStr = ''

        for c in range(0, len(test_case['legal_moves_black'])):
            if test_case['legal_moves_black'][c] == '8':
                newStr += '8'
            else:
                newStr += '0'
        test_legal_moves_black = newStr

        if(len(test_legal_moves_black) > 0):
            othello.set_current_player(2)
            othello.calculate_legal_moves()

            engine_legal_moves_black = othello.get_legal_moves_as_string()
            # for i in range(0, 8):
            #     for j in range(0, 8):
            #         print(othello.legal_moves[i][j], ' ', end='')
            #     print()
            # print(engine_legal_moves_black)
            # exit()

            if(test_legal_moves_black != engine_legal_moves_black):
                print('Test case failed')
                print("Board state: " + test_case['board_state'])
                print("Black legal moves engine: ")
                print_board(othello)
                print("Black legal moves test: ")
                for row in range(0, 8):
                    for col in range(0, 8):
                        print(test_legal_moves_black[8 * row + col], end=' ')
                    print()
            else:
                print('Passed')


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
        random_player_container = RandomPlayContainer(gui, othello)
        root.after(0, random_player_container.random_play)
        root.mainloop()


def test_performance(num_games):
    othello = Othello()
    performance_test_container = PerformanceTestContainer(othello)

    pr = cProfile.Profile()
    pr.enable()
    performance_test_container.random_play(num_games)
    pr.disable()
    s = io.StringIO()
    sortby = SortKey.CUMULATIVE
    ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats()
    print(s.getvalue())


class RandomPlayContainer():
    def __init__(self, gui, othello):
        self.othello = othello
        self.gui = gui

        self.game_over = False

    def random_play(self):
        if(self.othello.gameState.numBlackTiles + self.othello.gameState.numWhiteTiles == 64):
            self.game_over = True

        print('--')
        print(self.othello.gameState.numBlackTiles)
        print(self.othello.gameState.numWhiteTiles)

        print(self.othello.gameState.numBlackLegalMoves)
        print(self.othello.gameState.numWhiteLegalMoves)
        print('--')

        if(self.game_over):
            self.othello.init()
            self.game_over = False
            self.gui.root.after(200, self.random_play)
            # print("RENINIT")
        else:
            self.othello.calculate_legal_moves()

            if(self.gui):
                self.gui.update()

            if(self.othello.gameState.currentPlayer == 1):
                if(self.othello.gameState.numBlackLegalMoves > 0):
                    self.othello.play_random_move()

                else:
                    self.othello.switch_players()
            else:
                if(self.othello.gameState.numWhiteLegalMoves > 0):
                    self.othello.play_random_move()
                else:
                    self.othello.switch_players()

        self.gui.root.after(20, self.random_play)


class PerformanceTestContainer():
    def __init__(self, othello):
        self.othello = othello

    def random_play(self, num_games):

        black_won = 0
        white_won = 0

        for i in range(0, num_games):

            self.game_over = False
            consecutive_no_play_turns = 0

            while(not self.game_over):
                # print('--')
                # print(self.othello.gameState.numBlackTiles)
                # print(self.othello.gameState.numWhiteTiles)

                # print(self.othello.gameState.numBlackLegalMoves)
                # print(self.othello.gameState.numWhiteLegalMoves)
                # print('--')

                self.othello.calculate_legal_moves()

                if(self.othello.gameState.currentPlayer == 1):
                    if(self.othello.gameState.numBlackLegalMoves > 0):
                        self.othello.play_random_move()
                        consecutive_no_play_turns = 0

                    else:
                        self.othello.switch_players()
                        consecutive_no_play_turns += 1
                else:
                    if(self.othello.gameState.numWhiteLegalMoves > 0):
                        self.othello.play_random_move()
                        consecutive_no_play_turns = 0
                    else:
                        self.othello.switch_players()
                        consecutive_no_play_turns += 1

                if(self.othello.gameState.numBlackTiles + self.othello.gameState.numWhiteTiles == 64
                   or consecutive_no_play_turns == 2):
                    self.game_over = True

            if(self.othello.gameState.numBlackTiles > self.othello.gameState.numWhiteTiles):
                black_won += 1
            else:
                white_won += 1

            self.othello.init()
            self.game_over = False

        print(black_won, white_won)


# test_cases()
# test_play_random()
test_performance(100000)
