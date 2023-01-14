#include "Othello.h"

typedef struct StateNode{
  unsigned int hashedGameState;
  StateNode *left;
  StateNode *right;
} StateNode;



class StateSearchTree {
  
  public:
    StateSearchTree(GameState *gameState);
    ~StateSearchTree();

    int add();

    int find();

  private:
    StateNode *root;


}
