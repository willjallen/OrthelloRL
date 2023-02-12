#ifndef STATE_SEARCH_TREE_H
#define STATE_SEARCH_TREE_H

#include "Othello.h"
#include "NNet.h"
#include "StateNode.h"
#include <iostream>
#include <random>
#include <torch/script.h>
// (s,a)
struct ActionValues {
  std::pair<unsigned int, unsigned int> coordinate = std::make_pair(0,0);
  // The expected reward for taking action a from state s
  float Q = 0;
  
  // The number of times we took action a from state s across simulations
  float N = 0;
  
  // The initial estimate of taking an action a from state s according to policy theta
  float P = 0;

  ActionValues() {}
  ActionValues(std::pair<unsigned int, unsigned int> coordinate, float Q, int N, float P) : coordinate(coordinate), Q(Q), N(N), P(P) {}

};


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


#endif
