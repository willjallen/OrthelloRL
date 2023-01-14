

#include <limits.h>
#include "othello.h"


// y = e^(0.3x)
HASH_DISTRIBUTION = {20, 8418, 162863, 3082470, 651048268}


typedef struct Node
{
  unsigned int playout;
  unsigned int wins;

  Node() : playout(0),
           wins(0)
  {
    auto gamestate = std::make_unique<GameState>();
    init(gamestate);
  }


} Node;


// Current idea is to have two search trees, one for the main MCTS and another to 
// quickly find whether we have an existing state (saved as a hash probably)
class MCTS {
  
 MCTS();
  ~MCTS();

}


struct HashedGameState {
  unsigned long hash;
  unsigned int player;

  HashedGameState(unsigned long hash, unsigned int player) : hash(0), player(0);
  ~HashGameState();
}
  



}



