/**
 * Implementation of a monte-carlo tree search for Othello
 */

#include <algorithm>
#include <vector>

#include "Othello.h"
#include "MCTS.h"
#include "StateSearchTree.h"
#include <iostream>

// 1 if BLACK won, -1 if WHITE won
float getGameOverReward(const Othello::GameState &gameState){
  if(gameState.winner == Othello::BLACK){
    return 1;
  }else{
    return -1;
  }
}

MCTS::MCTS(Othello::GameState &initialState){
  this->stateSearchTree = new StateSearchTree(initialState);
}

MCTS::~MCTS(){
  delete this->stateSearchTree;
}



// gamestate, NN
float MCTS::search(Othello::GameState &gameState){

 
  gameState.calculateLegalMoves();

  std::cout << gameState << std::endl;
  // If we've reached a terminal state, propogate the reward up
  if(gameState.gameOver){
    return -getGameOverReward(gameState);
  }

  // If the state does not exist, add and initialize it
  // This will make a copy of gameState inside stateNode
  StateNode *stateNode = this->stateSearchTree->find(gameState);
  if(this->stateSearchTree->find(gameState) == nullptr){
    stateNode = stateSearchTree->add(gameState);
  }

  // Proper small number later
  float max_u = -9999999999999;

  // For each legal move
  // std::vector<Othello::Coordinate> legalMoves = Othello::getLegalMoves(gameState);

  // Find sum(state.actions.N)
  int sum_N;
  for(auto& action : stateNode->actions){
    sum_N += action.N;
  }
  float sqrt_sum_N = sqrt(sum_N);

  // Find best action
  ActionValues chosenAction;
  for(auto& action : stateNode->actions){
    // ActionValues actionValues = stateNode->actions.find(std::pair<int, int>(action.x, action.y));
    float u = action.Q + action.P * (sqrt_sum_N)/(1 + action.N);
    if(u > max_u){
      max_u = u;
      chosenAction = action; 
    }
  }

  gameState.playMove(chosenAction.coordinate.first, chosenAction.coordinate.second);
  // gameState is now s'

  float v = this->search(gameState);
  
  chosenAction.Q = (chosenAction.N * chosenAction.Q + v)/(chosenAction.N + 1);
  chosenAction.N += 1;

  return -v;
}

