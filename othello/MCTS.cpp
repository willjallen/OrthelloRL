/**
 * Implementation of a monte-carlo tree search for Othello
 */

#include <algorithm>
#include <vector>

#include "Othello.h"
#include "MCTS.h"
#include "NNet.h"
#include "StateNode.h"
#include "StateSearchTree.h"
#include <iostream>
#include <random>
#include <cstddef>
#include <cmath>

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


  // Find sum(state.actions.N)
  int sum_N = 0;
  for(auto& action : stateNode->actions){
    sum_N += action.N;
  }

  float sqrt_sum_N = sqrt(sum_N);


  // Find best action
  ActionValues *chosenAction;
  for(auto& action : stateNode->actions){

   // Exploration parameter
   float cpuct = 1;
   float u = action.Q + cpuct * action.P * (sqrt_sum_N)/(1 + action.N);
   // float u = rand(); 

    if(u > max_u){
      max_u = u;
      chosenAction = &action; 
    }
  }

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


const float TEMP_THRESHOLD = 0.01;

std::vector<float> MCTS::getPolicy(Othello::GameState &gameState, float temperature){
  // If temperature < 0.01 (=0)
  // Set the prob of best action to 1
  // Otherwise explore based on temp
  std::vector<float> policy;
  for(int i = 0; i < 64; i++){
    policy.push_back(0);
  }

  StateNode *stateNode = this->stateSearchTree->find(gameState);
  if(this->stateSearchTree->find(gameState) == nullptr){
    std::cout << "should not happen" << std::endl;
  } 


  if(temperature < TEMP_THRESHOLD){
    // Pick the best action
    float maxN = -9999999;
    ActionValues bestAction;
    for(auto& action : stateNode->actions){
      if(action.N > maxN){
        bestAction = action;
        maxN = action.N;
      }
    }
   
   policy[8*bestAction.coordinate.first + bestAction.coordinate.second] = 1;
    
  }else{
    // Explore
    float sum = 0;
    for(auto& action : stateNode->actions){
      sum += pow(action.N, 1/temperature);
    }

    for(auto& action : stateNode->actions){
       policy[8*action.coordinate.first + action.coordinate.second] 
         = pow(action.N, 1/temperature)/sum;
    }
  }
  return policy;
}



