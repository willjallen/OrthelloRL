#include "Othello.h"

// (s,a)
template <typename T>
struct StateValuePair {
  uint64_t hashedGameState;
  uint64_t hashedGameStateAfterAction;
  T value;
}


struct StateNode{
  
  GameState *gameState;
  uint64_t hashedGameState;

  // The expected reward for taking action a from state s
  std::vector<StateValuePair<float>> Qvals;
  
  // The number of times we took action a from state s across simulations
  std::vector<StateValuePair<int>> Nvals;

  // The initial estimate of taking an action a from state s according to policy theta
  std::vector<StateValuePair<float>> Pvals;
  

  StateNode *left;
  StateNode *right;

  StateNode(uint64_t hashedGameState) : hashedGameState(hashedGameState){
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
