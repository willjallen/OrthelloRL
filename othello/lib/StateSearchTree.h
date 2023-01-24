#ifndef STATE_SEARCH_TREE_H
#define STATE_SEARCH_TREE_H

#include "Othello.h"

// (s,a)
struct ActionValues {
  std::pair<int, int> coordinate;
  // The expected reward for taking action a from state s
  float Q;
  
  // The number of times we took action a from state s across simulations
  int N;
  
  // The initial estimate of taking an action a from state s according to policy theta
  float P;

  ActionValues() {}
  ActionValues(std::pair<int, int> coordinate, float Q, int N, float P) : coordinate(coordinate), Q(Q), N(N), P(P) {}

};


struct StateNode{
  
  Othello::GameState gameState;
  uint64_t hashedGameState;

  std::vector<ActionValues> actions;


  StateNode *left;
  StateNode *right;

  StateNode(Othello::GameState &gameState, uint64_t hashedGameState){
   
    // Make a copy
    this->gameState = Othello::GameState(gameState);
    this->hashedGameState = hashedGameState;

    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
 
    // Save actions as x,y pairs 
    std::vector<std::pair<int,int>> legalMoves = gameState.getLegalMoves();
    for(auto& action : legalMoves){ 
      // TODO: When NN comes in it will go here
      actions.push_back(ActionValues(action, 0, 0, 0));
    }

    this->left = nullptr;
    this->right = nullptr;
  }
    

};



class StateSearchTree {
  
  public:
    StateSearchTree();
    StateSearchTree(Othello::GameState &gameState);
    ~StateSearchTree();

    StateNode* add(Othello::GameState &gameState);
    StateNode* find(Othello::GameState &gameState);  

  private:
    StateNode *root;

    void deleteTree(StateNode *stateNode);

};

#endif
