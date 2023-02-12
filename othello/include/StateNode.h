#include "Othello.h"

class StateNode{
  
  public:
    Othello::ComparableGameState comparableGameState;
    StateNode::StateNode(Othello::GameState &gameState, NNet *nnet)
    // v
    float value;

    // policy
    std::vector<ActionValues> actions;
    bool noLegalMoves = false;


    StateNode *left = nullptr;
    StateNode *right = nullptr;



    

}
