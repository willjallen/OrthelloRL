#ifndef STATE_SEARCH_TREE_H
#define STATE_SEARCH_TREE_H

#include "Othello.h"
#include "NNet.h"
#include <iostream>
#include <random>
#include <torch/script.h>
// (s,a)
struct ActionValues {
  std::pair<unsigned int, unsigned int> coordinate = std::make_pair(0,0);
  // The expected reward for taking action a from state s
  float Q = 0;
  
  // The number of times we took action a from state s across simulations
  int N = 0;
  
  // The initial estimate of taking an action a from state s according to policy theta
  float P = 0;

  ActionValues() {}
  ActionValues(std::pair<unsigned int, unsigned int> coordinate, float Q, int N, float P) : coordinate(coordinate), Q(Q), N(N), P(P) {}

};


static std::random_device rd; 
static std::mt19937 rng{rd()};
static std::uniform_real_distribution<float> dist(0, 1);
struct StateNode;


class StateSearchTree {
  
  public:
    StateSearchTree();
    StateSearchTree(Othello::GameState &gameState, NNet *nnet);
    ~StateSearchTree();

    StateNode* add(Othello::GameState &gameState);
    StateNode* find(Othello::GameState &gameState);  

    void printTree();
    void printTree(const std::string &prefix, const StateNode *stateNode, bool isLeft);
  

    NNet *nnet;
    
  private:
    StateNode *root;

    void deleteTree(StateNode *stateNode);
};

struct StateNode{
  
  Othello::ComparableGameState comparableGameState;

  // v
  float value;

  // policy
  std::vector<ActionValues> actions;
  bool noLegalMoves = false;


  StateNode *left = nullptr;
  StateNode *right = nullptr;

  StateNode(Othello::GameState &gameState, NNet *nnet){
    this->comparableGameState = gameState.getComparableGameState(); 
    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
 
    // Save actions as x,y pairs 
    std::vector<std::pair<unsigned int, unsigned int>> legalMoves 
      = gameState.getLegalMoves();

    auto nnetResult = nnet->predict(gameState);
    auto value = nnetResult.second.item<float>();

    // Get state value
    this->value = value; 

    if(legalMoves.size() == 0){
      this->noLegalMoves = true;
    }else{
      auto pvals = nnetResult.first;
     
      for(auto& action : legalMoves){
        int actionX = action.first;
        int actionY = action.second;

        float p_val_at_location = pvals[8*actionX + actionY].item<float>(); 
        this->actions.push_back(ActionValues(action, 0, 0, p_val_at_location));
      }
    }


  }

    

};
#endif
