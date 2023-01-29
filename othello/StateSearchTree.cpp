  
#include "StateSearchTree.h"
#include "Othello.h"
#include <iostream>
StateSearchTree::StateSearchTree(Othello::GameState &gameState, NNet *nnet) {

  
  this->root = new StateNode(gameState, nnet);
  this->nnet = nnet;
}

StateSearchTree::StateSearchTree(){
  
}




StateSearchTree::~StateSearchTree() {
  this->deleteTree(this->root);
}


// Delete inorder
void StateSearchTree::deleteTree(StateNode *stateNode){
  if(stateNode == nullptr) return;
  this->deleteTree(stateNode->left);
  this->deleteTree(stateNode->right);

  delete stateNode;
}



StateNode* StateSearchTree::add(Othello::GameState &gameState){

  StateNode *cursor = this->root;
  Othello::ComparableGameState comparableGameState = gameState.getComparableGameState();  
  
  while(true){


    if(comparableGameState == cursor->comparableGameState){
      return nullptr;
    }


    if(comparableGameState < cursor->comparableGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        cursor->left = new StateNode(gameState, this->nnet);
        return cursor->left;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(comparableGameState > cursor->comparableGameState){
      
      StateNode *newCursor = cursor->right;
      
      if(newCursor == nullptr){
        cursor->right = new StateNode(gameState, this->nnet);
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
  Othello::ComparableGameState comparableGameState = gameState.getComparableGameState();  
  while(true){


    if(comparableGameState == cursor->comparableGameState){
      return cursor;
    }


    if(comparableGameState < cursor->comparableGameState){
      
      StateNode *newCursor = cursor->left;
      
      if(newCursor == nullptr){
        return nullptr;
      }else{
        cursor = newCursor;
        continue;
      }
    }

    if(comparableGameState > cursor->comparableGameState){
      
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
      std::cout << stateNode->comparableGameState; 
      // enter the next tree level - left and right branch
      this->printTree( prefix + (isLeft ? "│   " : "    "), stateNode->left, true);
      this->printTree( prefix + (isLeft ? "│   " : "    "), stateNode->right, false);
  }
}



