#include "MCTS.h"
#include "Othello.h"
#include <iostream>



int main(){
  // Set up the game
  Othello::GameState actualGameState;
  Othello::GameState searchGameState = actualGameState;
  // Create MCTS
  MCTS mcts(searchGameState);

  while(true){

    actualGameState.calculateLegalMoves();
    if(actualGameState.gameOver){
      break;
    }
    
    for(int i = 0; i < 100; i++){
      searchGameState = actualGameState;
      mcts.search(searchGameState);
    }
    // mcts.stateSearchTree->printTree(); 
    std::vector<std::pair<std::pair<unsigned int, unsigned int>, float>> improvedPolicy = mcts.getPI(actualGameState);

    std::pair<int,int> bestAction;
    float maxVal = -999999999999;

    for(auto& item : improvedPolicy){
      std::cout << "(" << item.first.first << ", " << item.first.second << ", " << item.second << ") ";
      if(item.second > maxVal){
        maxVal = item.second;
        bestAction = item.first;
      }
    }
     
    std::cout << std::endl;



    std::cout << actualGameState;
    
    if(improvedPolicy.size() != 0){
      actualGameState.playMove(bestAction.first, bestAction.second);
    }else{
      actualGameState.pass();
    }


  }
  // mcts.stateSearchTree->printTree();
}
