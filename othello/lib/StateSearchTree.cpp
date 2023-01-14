  
#include "StateSearchTree.h"



StateSearchTree() {

}




unsigned long StateSearchTree::getHashedGameState(GameState *gameState){

    unsigned long hash = 0;

    unsigned long iterator = 0;

     for(int i = 0; i < 8; i++){
       for(int j = 0; j < 8; j++){
         if(gameState->board[i][j] == gameState->currentPlayer){
          hash = hash || 1 << (i*j);
         }
       }
     }

  return hash; 


}


