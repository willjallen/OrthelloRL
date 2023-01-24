  
#include "StateSearchTree.h"
#include "Othello.h"

StateSearchTree::StateSearchTree(Othello::GameState *gameState) {
  this->root = new StateNode(gameState, Othello::getHashedGameState(gameState)); 
}

StateSearchTree::~StateSearchTree() {
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



StateNode* StateSearchTree::add(Othello::GameState *gameState){

  StateNode *cursor = this->root;
  uint64_t newHashedGameState = Othello::getHashedGameState(gameState);
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return nullptr;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        cursor->left = new StateNode(gameState, newHashedGameState);
        return cursor->left;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        cursor->right = new StateNode(gameState, newHashedGameState);
        return cursor->right;
      }else{
        cursor = newCursor;
        continue;
      }

    }

  }

}


StateNode* StateSearchTree::find(Othello::GameState *gameState){

  StateNode *cursor = this->root;
  unsigned int newHashedGameState = Othello::getHashedGameState(gameState);
  while(true){


    if(newHashedGameState == cursor->hashedGameState){
      return cursor;
    }


    if(newHashedGameState < cursor->hashedGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        return nullptr;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(newHashedGameState > cursor->hashedGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        return nullptr;
      }else{
        cursor = newCursor;
        continue;
      }

    }
  }

}

