#include "MCTS.h"
#include "Othello.h"




int main(){
  // Set up the game
  Othello::GameState gameState;
  Othello::init(&gameState);

  // Create MCTS
  MCTS mcts(&gameState);
}
