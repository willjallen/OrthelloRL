#include "MCTS.h"
#include "Othello.h"
#include "StateSearchTree.h"




int main(){
  // Set up the game
  GameState gameState;
  Othello::init(gameState);

  // Create MCTS
  MCTS mcts();
}
