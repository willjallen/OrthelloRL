/**
 * Implementation of a monte-carlo tree search for Othello
 */

#include "Othello.h"
#include "MCTS.h"
#include "StateSearchTree.h"

MCTS::MCTS(Othello::GameState *initialState){
  StateSearchTree stateSearchTree(initialState);
}

// gamestate, NN
float MCTS::search(Othello::GameState *gameState){

  if(gameState->gameOver){
    return getGameOverReward(gameState);
  }

  if(!stateSearchTree.find(gameState)){
    stateSearchTree.add(gameState);
  }

  // Proper big number later
  max_u = 9999999999999;


}

// 1 if BLACK won, -1 if WHITE won
float getGameOverReward(Othello::GameState *gameState){
  if(gameState->winner == Othello::BLACK){
    return 1;
  }else{
    return -1;
  }
}




