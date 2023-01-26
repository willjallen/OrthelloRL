#ifndef STATE_SEARCH_TREE_H
#define STATE_SEARCH_TREE_H

#include "Othello.h"
#include <iostream>
#include <random>
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
    StateSearchTree(Othello::GameState &gameState);
    ~StateSearchTree();

    StateNode* add(Othello::GameState &gameState);
    StateNode* find(Othello::GameState &gameState);  

    void printTree();
    void printTree(const std::string &prefix, const StateNode *stateNode, bool isLeft);
  

  private:
    StateNode *root;

    void deleteTree(StateNode *stateNode);
};

struct StateNode{
  
  // Othello::GameState gameState;
  Othello::ComparableGameState comparableGameState;


  std::vector<ActionValues> actions;
  bool noLegalMoves = false;

  StateSearchTree *tree;

  StateNode *left = nullptr;
  StateNode *right = nullptr;

  StateNode(Othello::GameState &gameState, StateSearchTree *tree){
    this->tree = tree;
    this->comparableGameState = gameState.getComparableGameState(); 
    // std::cout << "Constructing state node" << std::endl;
    // std::cout << "comparableGameState: " << this->comparableGameState; 
    // std::cout << "pointer: " << this << std::endl;
    // Make a copy
    // this->gameState = Othello::GameState(gameState);

    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
 
    // Save actions as x,y pairs 
    std::vector<std::pair<unsigned int, unsigned int>> legalMoves = gameState.getLegalMoves();
    if(legalMoves.size() == 0) this->noLegalMoves = true;
    for(auto& action : legalMoves){
      // std::cout << "Adding action: " << "(" << action.first << ", " << action.second << ")" << "\n";
      // TODO: When NN comes in it will go here
      float r = dist(rng);
      this->actions.push_back(ActionValues(action, 0, 0, r));
    }
  }

    

};
#endif
