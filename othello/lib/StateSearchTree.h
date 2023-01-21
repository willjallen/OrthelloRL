#include "Othello.h"

// (s,a)
template <typename T>
struct StateValuePair {
  unsigned int hashedGameState;
  unsigned int hashedGameStateAfterAction;
  T value;
}


struct StateNode{
  
  GameState *gameState;
  unsigned int hashedGameState;

  // The expected reward for taking action a from state s
  std::vector<StateValuePair<float>> Qvals;
  
  // The number of times we took action a from state s across simulations
  std::vector<StateValuePair<int>> Nvals;

  // The initial estimate of taking an action a from state s according to policy theta
  std::vector<StateValuePair<float>> Pvals;
  

  StateNode *left;
  StateNode *right;

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
