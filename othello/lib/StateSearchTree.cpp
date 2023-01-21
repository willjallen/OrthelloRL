  
#include "StateSearchTree.h"
#include "Othello.h"

StateSearchTree::StateSearchTree(Othello::GameState *e) {
  this->root = new StateNode(Othello::getHashedGameState(gameState), nullptr, nullptr); 
}

StateSearchTree::~StateSearchTree(Othello::GameState *gameState) {
  this->deleteTree(this->root);
}


// Delete inorder
void StateSearchTree::deleteTree(StateNode *stateNode){
  if(stateNode->left != nullptr){
    this->deleteTree(stateNode->left);
  }

  delete stateNode;

  if(stateNode->right != nullptr){
    this->deleteTree(stateNode->right);
  }

}



int StateSearchTree::add(Othello:GameState *gameState){

  StateNode *cursor = this->root;
  uint64_t newHashedGameState = Othello::getHashedGameState(gameState)
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return -1;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        cursor->left = new StateNode(newHashedGameState);
        return 1;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        cursor->right = new StateNode(newHashedGameState);
        return 1;
      }else{
        cursor = newCursor;
        continue;
      }

    }

  }

}


int StateSearchTree::find(Othello::GameState *gameState){

  StateNode *cursor = this->root;
  unsigned int newHashedGameState = Othello::getHashedGameState(gameState)
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return 1;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        return 0;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        return 0;
      }else{
        cursor = newCursor;
        continue;
      }

    }
  }

}

