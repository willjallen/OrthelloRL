#ifndef STATENODE_H
#define STATENODE_H

#include "Othello.h"
#include "NNet.h"

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


class StateNode{
  
  public:
    Othello::ComparableGameState comparableGameState;
    StateNode(Othello::GameState &gameState, NNet *nnet);
    // v
    float value;

    // policy
    std::vector<ActionValues> actions;
    bool noLegalMoves = false;


    StateNode *left = nullptr;
    StateNode *right = nullptr;
};

#endif
