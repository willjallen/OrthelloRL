


#include "othello.h"

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


class SearchTree{
  
  SearchTree();
  ~SearchTree();

  

}


