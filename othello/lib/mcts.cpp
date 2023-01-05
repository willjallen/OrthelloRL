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

class SearchTree
{

    SearchTree()
    {
        Node *root = std::make_unique<Node>();
    }
}
