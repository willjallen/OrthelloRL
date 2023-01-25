  
#include "StateSearchTree.h"
#include "Othello.h"
#include <iostream>
StateSearchTree::StateSearchTree(Othello::GameState &gameState) {
  this->root = new StateNode(gameState, gameState.getHashedGameState()); 
}

StateSearchTree::StateSearchTree(){
  
}




StateSearchTree::~StateSearchTree() {
  this->deleteTree(this->root);
}


// Delete inorder
void StateSearchTree::deleteTree(StateNode *stateNode){
  if(stateNode == nullptr) return;
  std::cout << "ere" << std::endl;
  this->deleteTree(stateNode->left);
  this->deleteTree(stateNode->right);

  delete stateNode;
}



StateNode* StateSearchTree::add(Othello::GameState &gameState){

  StateNode *cursor = this->root;
  uint64_t newHashedGameState = gameState.getHashedGameState();
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


StateNode* StateSearchTree::find(Othello::GameState &gameState){

  StateNode *cursor = this->root;
  uint64_t newHashedGameState = gameState.getHashedGameState();  
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
 
void StateSearchTree::printTree(){
  this->printTree("", this->root, false); 
}

void StateSearchTree::printTree(const std::string& prefix, const StateNode *stateNode, bool isLeft){
  if( stateNode != nullptr )
  {
      std::cout << prefix;

      std::cout << (isLeft ? "├──" : "└──" );

      // print the value of the node
      std::cout << stateNode->hashedGameState << std::endl;

      // enter the next tree level - left and right branch
      this->printTree( prefix + (isLeft ? "│   " : "    "), stateNode->left, true);
      this->printTree( prefix + (isLeft ? "│   " : "    "), stateNode->right, false);
  }
}



