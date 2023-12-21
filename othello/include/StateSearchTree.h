#ifndef STATE_SEARCH_TREE_H
#define STATE_SEARCH_TREE_H

#include "Othello.h"
#include "NNet.h"
#include "StateNode.h"
#include <iostream>
#include <random>
#include <torch/script.h>


class StateSearchTree {
  
  public:
    StateSearchTree();
    StateSearchTree(Othello::GameState &gameState, ThreadSafeQueue &inferenceQueue);
    ~StateSearchTree();

    StateNode* add(Othello::GameState &gameState);
    StateNode* find(Othello::GameState &gameState);  

    void printTree();
    void printTree(const std::string &prefix, const StateNode *stateNode, bool isLeft);
    
  private:
    StateNode *root;
    ThreadSafeQueue &inferenceQueue_;
    void deleteTree(StateNode *stateNode);
};


#endif
