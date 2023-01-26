#include "MCTS.h"
#include "Othello.h"
#include <iostream>




void testMCTSvsMCTS(int numGames, int numSimsOne, int numSimsTwo){ 



  float blackWins = 0;
  float whiteWins = 0;
  float ties = 0;

  for(int i = 0; i < numGames; i++){

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;
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

        // std::cout << actualGameState;
        break;
      }

      int numSims = 0;
      if(actualGameState.currentPlayer == Othello::BLACK){
        numSims = numSimsOne;
      }else{
        numSims = numSimsTwo;
      }
      for(int j = 0; j < numSims; j++){
         searchGameState = actualGameState;
        mcts.search(searchGameState);
      }
      std::vector<std::pair<std::pair<unsigned int, unsigned int>, float>> improvedPolicy = mcts.getPI(actualGameState);

      std::pair<int,int> bestAction;
      float maxVal = -999999999999;

      for(auto& item : improvedPolicy){
        if(item.second > maxVal){
          maxVal = item.second;
          bestAction = item.first;
        }
      }

        if(improvedPolicy.size() != 0){
          actualGameState.playMove(bestAction.first, bestAction.second);
        }else{
          actualGameState.pass();
        }
     
    }
  }

  std::cout << "Black win %: " << (blackWins/numGames) << std::endl;
  std::cout << "White win %: " << (whiteWins/numGames) << std::endl;
  std::cout << "Tie %: " << (ties/numGames) << std::endl;

}


void testMCTSvsRandom(int numGames, int numSims){ 



  float blackWins = 0;
  float whiteWins = 0;
  float ties = 0;

  for(int i = 0; i < numGames; i++){

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;
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

      
        for(int j = 0; j < numSims; j++){
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
      }else if(actualGameState.currentPlayer == Othello::WHITE){
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

void testRandomvsRandom(int numGames){


  float blackWins = 0;
  float whiteWins = 0;
  float ties = 0;
  // Set up the game
  
  for(int i = 0; i < numGames; i++){
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
  // std::cout << "MCTS self-play" << std::endl;
  // testMCTSvsMCTS(500, 25, 25);
  std::cout << "MCTS vs Random" << std::endl;
  testMCTSvsRandom(500, 25);
  std::cout << "Random self-play" << std::endl;
  testRandomvsRandom(500);
}
