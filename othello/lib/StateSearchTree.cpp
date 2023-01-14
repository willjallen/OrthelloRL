  
#include "StateSearchTree.h"
#include "Othello.h"








StateSearchTree::StateSearchTree(Othello::GameState *gameState) {
  this->root = new StateNode(Othello::getHashedGameState(gameState), nullptr, nullptr); 
}

int StateSearchTree::add(Othello:GameState *gameState){

  StateNode *cursor = this->root;
  unsigned int newHashedGameState = Othello::getHashedGameState(gameState)
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return -1;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        cursor->left = new StateNode(newHashedGameState, nullptr, nullptr);
        return 1;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        cursor->right = new StateNode(newHashedGameState, nullptr, nullptr);
        return 1;
      }else{
        cursor = newCursor;
        continue;
      }

    }
  }

}


int StateSearchTree::find(Othello:GameState *gameState){

  StateNode *cursor = this->root;
  unsigned int newHashedGameState = Othello::getHashedGameState(gameState)
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return 1;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        return -1;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        cursor->right = new StateNode(newHashedGameState, nullptr, nullptr);
        return 1;
      }else{
        cursor = newCursor;
        continue;
      }

    }
  }

}

