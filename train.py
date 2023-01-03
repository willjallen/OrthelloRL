import torch
import torch.nn as nn
from networks.simple import simple 

def getOtherPlayer(currentPlayer):
    return 1 if currentPlayer == 2 else 2

def getEncodedState(othello):
    
    gameState = othello.gameState

    currentPlayer = othello.currentPlayer
    
    # 8x8 binary encoded player tiles
    # 8x8 binary encoded enemy tiles
    # 8x8 legal moves
    player_tiles = []
    enemy_tiles = []
    legal_moves = []

    for i in range(0, 8):
        player_tiles_tmp = []
        enemy_tiles_tmp = []
        legal_moves_tmp = []
        for j in range(0, 8):
            position = gameState.board[i][j]
            player_tiles_tmp.append(1 if position == currentPlayer else 0)     
            enemy_tiles_tmp.append(1 if position == getOtherPlayer(currentPlayer) else 0)     
            legal_tiles_tmp.append(1 if position == othello.legal else 0)     

        player_tiles.append(player_tiles_tmp)
        enemy_tiles.append(enemy_tiles_tmp)
        legal_moves.append(legal_moves_tmp)

    data = [player_tiles, enemy_tiles, legal_moves]
    return torch.tensor(data)


def trainSimple(othello):
   model =  





