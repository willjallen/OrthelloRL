#ifndef MCTS_H
#define MCTS_H

#include <limits.h>
#include <random>
#include "Othello.h"
#include "StateSearchTree.h"
#include "NNet.h"


struct Policy {

  std::vector<std::vector<float>> pi; 

  Policy(){
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        pi.push_back({0,0,0,0,0,0,0,0}); 
      }
    }
  }
};


class MCTS {
  
  public:
    MCTS(Othello::GameState &initialState, NNet *nnet);
   ~MCTS(); 

    float search(Othello::GameState &gameState);
    Policy getPI(Othello::GameState &gameState);

    
    StateSearchTree *stateSearchTree;
  private:

};


#endif
