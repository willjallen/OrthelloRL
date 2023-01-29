#include "MCTS.h"
#include "Othello.h"
#include "NNet.h"
#include <iostream>
#include <torch/script.h> // One-stop header.


void testMCTSvsMCTS(int numGames, int numSimsOne, int numSimsTwo, NNet *nnet){ 



  float blackWins = 0;
  float whiteWins = 0;
  float ties = 0;

  for(int i = 0; i < numGames; i++){

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;
    // Create MCTS
    MCTS mcts(searchGameState, nnet);
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


void testMCTSvsRandom(int numGames, int numSims, NNet *nnet){ 



  float blackWins = 0;
  float whiteWins = 0;
  float ties = 0;

  for(int i = 0; i < numGames; i++){

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;


  //  std::cout << "HERE5" << std::endl;
  // std::cout << &(nnet->_module) << std::endl;
    // Create MCTS
    MCTS mcts(searchGameState, nnet);
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

void testNN(){

  Othello::GameState actualGameState;

  NNet *nnet = new NNet("../networks/output_model.pt");
  
  auto out = nnet->predict(actualGameState);
  std::cout << out << std::endl;
  delete nnet;
}
// int argc, const char* argv[]
int main(){
  
  // testNN();

  // Othello::GameState actualGameState;

  NNet *nnet = new NNet("../networks/output_model.pt");
  
  std::cout << "MCTS vs Random" << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  testMCTSvsRandom(10, 25, nnet);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  std::cout << "Runtime: " << duration.count() << "seconds" << std::endl;

  delete nnet;
  // std::cout << "Random self-play" << std::endl;
  // testRandomvsRandom(100);
 // if (argc != 2) {
 //    std::cerr << "usage: example-app <path-to-exported-script-module>\n";
 //    return -1;
 //  }

  // torch::jit::script::Module module;
  // try {
  //   // Deserialize the ScriptModule from a file using torch::jit::load().
  //   module = torch::jit::load(argv[1]);
  // }
  // catch (const c10::Error& e) {
  //   std::cerr << "error loading the model\n";
  //   return -1;
  // }

  // std::cout << "ok\n";
  // NNet nnet = std::make_shared<NNet>(new NNet("../networks/output_model.pt"));
  // testNN();
}

