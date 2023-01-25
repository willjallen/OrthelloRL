#include "MCTS.h"
#include "Othello.h"
#include <iostream>



void MCTSvsRandom(){ 
 
  float numGames = 100;

  int blackWins = 0;
  int whiteWins = 0;
  int ties = 0;

  for(int i = 0; i < numGames; i++){

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState = actualGameState;
    // Create MCTS
    MCTS mcts(searchGameState);
    while(true){


      actualGameState.calculateLegalMoves();
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        if(actualGameState.winner == Othello::BLACK){
          blackWins++;
        }else if(actualGameState.winner == Othello::WHITE){
          whiteWins++;
        }else{
          ties++;
        }
        break;
      }

      if(actualGameState.currentPlayer == Othello::BLACK){

      
      for(int j = 0; j < 100; j++){
        searchGameState = actualGameState;
        mcts.search(searchGameState);
      }
      // mcts.stateSearchTree->printTree(); 
      std::vector<std::pair<std::pair<unsigned int, unsigned int>, float>> improvedPolicy = mcts.getPI(actualGameState);

      std::pair<int,int> bestAction;
      float maxVal = -999999999999;

      for(auto& item : improvedPolicy){
        // std::cout << "(" << item.first.first << ", " << item.first.second << ", " << item.second << ") ";
        if(item.second > maxVal){
          maxVal = item.second;
          bestAction = item.first;
        }
      }
       
      // std::cout << std::endl;



      // std::cout << actualGameState;
      
      if(improvedPolicy.size() != 0){
        actualGameState.playMove(bestAction.first, bestAction.second);
      }else{
        actualGameState.pass();
      }
      }else{
        actualGameState.playRandomMove();
      }


    }
  }
  // mcts.stateSearchTree->printTree();
  //
  std::cout << "Black win %: " << (blackWins/numGames) << std::endl;
  std::cout << "White win %: " << (whiteWins/numGames) << std::endl;
  std::cout << "Tie %: " << (ties/numGames) << std::endl;

}

void RandomvsRandom(){

  float numGames = 100;

  int blackWins = 0;
  int whiteWins = 0;
  int ties = 0;
  // Set up the game
  
  for(int i = 0; i < 100; i++){
    Othello::GameState actualGameState;
    while(true){
      actualGameState.calculateLegalMoves();
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        if(actualGameState.winner == Othello::BLACK){
          blackWins++;
        }else if(actualGameState.winner == Othello::WHITE){
          whiteWins++;
        }else{
          ties++;
        }
        break;
      }
      actualGameState.playRandomMove();
    }
  }
  // mcts.stateSearchTree->printTree();
  std::cout << "Black win %: " << (blackWins/numGames) << std::endl;
  std::cout << "White win %: " << (whiteWins/numGames) << std::endl;
  std::cout << "Tie %: " << (ties/numGames) << std::endl;


}

int main(){
  MCTSvsRandom();
  std::cout << "random now" << std::endl;
  RandomvsRandom();
}
