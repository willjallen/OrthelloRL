/**
 * Implementation of a monte-carlo tree search for Othello
 */

#include "mcts.h"

typedef struct Node
{
    unsigned int playout;
    unsigned int wins;

    Node() : playout(0), wins(0)
    {
        auto gamestate = std::make_unique<GameState>();
        init(gamestate);
    }

} Node;


class StateSearchTree
{

    SearchTree()
    {
        StateNode *root = std::make_unique<StateNode>();
    }

    


    void find(int state){

    }
}
