#include "Othello.h"

// (s,a)
struct ActionValues {
  Othello::Coordinate action;
  // The expected reward for taking action a from state s
  float Q;
  
  // The number of times we took action a from state s across simulations
  int N;
  
  // The initial estimate of taking an action a from state s according to policy theta
  float P;

  ActionValues(Othello::Coordinate action, float Q, int N, float P) : action(action), Q(Q), N(N), P(P) {}
}


struct StateNode{
  
  GameState *gameState;
  uint64_t hashedGameState;

  std::vector<ActionValues> actions;

  StateNode *left;
  StateNode *right;

  StateNode(GameState *gameState, uint64_t hashedGameState) : hashedGameState(hashedGameState){
    
    Othello::copyGameState(gameState, this->gameState);
  

    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
  
    std::vector<Othello::Coordinate> legalMoves = Othello::getLegalMoves(gameState);
    for(auto& action : legalMoves){ 
      actions.push_back(action, 0, 0, 0);
    }

    this->left = nullptr;
    this->right = nullptr;
  }
    

};



class StateSearchTree {
  
  public:
    StateSearchTree(GameState *gameState);
    ~StateSearchTree();

    int addNode();
    int findNode();  

  private:
    StateNode *root;

    void deleteTree(StateNode *stateNode);

}
