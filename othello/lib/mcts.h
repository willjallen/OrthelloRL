

#include <limits.h>
#include "Othello.h"
#include "StateSearchTree.h"


class MCTS {
  
  public:
    MCTS(Othello::GameState *initialState);
    ~MCTS();

    float search(Othello::GameState *gameState);

  private:
    StateSearchTree stateSearchTree;

};






