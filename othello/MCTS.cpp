/**
 * Implementation of a monte-carlo tree search for Othello
 */

#include <algorithm>
#include <vector>

#include "Othello.h"
#include "MCTS.h"
#include "NNet.h"

#include "StateSearchTree.h"
#include <iostream>
#include <random>
#include <cstddef>

// 1 if BLACK won, -1 if WHITE won
float getGameOverReward(const Othello::GameState &gameState){
  if(gameState.winner == Othello::BLACK){
    return 1;
  }else{
    return -1;
  }
}

MCTS::MCTS(Othello::GameState &initialState, NNet *nnet){

  this->stateSearchTree = new StateSearchTree(initialState, nnet);
}

MCTS::~MCTS(){
  delete this->stateSearchTree;
}



// gamestate, NN
float MCTS::search(Othello::GameState &gameState){

  // if(gameState.turnNumber == 40) return 0;

  gameState.calculateLegalMoves();
  // If we've reached a terminal state, propogate the reward up
  if(gameState.gameOver){
    return -getGameOverReward(gameState);
  }

    
  StateNode *stateNode = this->stateSearchTree->find(gameState);
  if(this->stateSearchTree->find(gameState) == nullptr){
    stateNode = stateSearchTree->add(gameState);
    return -stateNode->value;
    
  }

  // Proper small number later
  float max_u = -99999999;
// std::numeric_limits<float>::min();

  // For each legal move
  // std::vector<Othello::Coordinate> legalMoves = Othello::getLegalMoves(gameState);

  // Find sum(state.actions.N)
  int sum_N = 0;
  for(auto& action : stateNode->actions){
    sum_N += action.N;
  }

  float sqrt_sum_N = sqrt(sum_N);


  // Find best action
  ActionValues *chosenAction;
  for(auto& action : stateNode->actions){

   // Exploration para
   float c = 0.12;
   float u = action.Q + c * action.P * (sqrt_sum_N)/(1 + action.N);
   // float u = rand(); 

    if(u > max_u){
      max_u = u;
      chosenAction = &action; 
    }
  }
  // std::cout << "action ptr: " << chosenAction << "\n";
  // std::cout << "Chosen action: " << "(" << chosenAction->coordinate.first << ", " << chosenAction->coordinate.second << ")" << "\n";
  // std::cout << "ca " << chosenAction << std::endl;
  // std::cout << chosenAction->coordinate.first << " " << chosenAction->coordinate.second << std::endl;

  // TODO: ??????
  // If there is no action, pass
  if(!stateNode->noLegalMoves){
    gameState.playMove(chosenAction->coordinate.first, chosenAction->coordinate.second);
  }else{
    gameState.pass();
  }
  // gameState is now s'

  float v = this->search(gameState);
 
  if(!stateNode->noLegalMoves){
    chosenAction->Q = (chosenAction->N * chosenAction->Q + v)/(chosenAction->N + 1);
    chosenAction->N += 1;
  }

  return -v;
}

std::vector<std::pair<std::pair<unsigned int, unsigned int>, float>> MCTS::getPI(Othello::GameState &gameState){
  
  std::vector<std::pair<std::pair<unsigned int, unsigned int>, float>> pi;

  StateNode *stateNode = this->stateSearchTree->find(gameState);
  if(this->stateSearchTree->find(gameState) == nullptr){
    std::cout << "should not happen" << std::endl;
  } 

  // float sum_N = 0;
  // for(auto& action : stateNode->actions){
  //   sum_N += action.N;
  // }

  for(auto& action : stateNode->actions){
    pi.push_back(std::pair<std::pair<unsigned int, unsigned int>, float>(action.coordinate, action.Q));
  }

  return pi;
}



