
#include "MCTS.h"
#include "Othello.h"
#include "NNet.h"
#include <iostream>
#include <torch/script.h> // One-stop header.
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>


struct TrainingExample {
  torch::Tensor contiguousGameState;
  std::vector<std::vector<float>> pi;
  int reward;

  TrainingExample(torch::Tensor contiguousGameState, std::vector<std::vector<float>> pi, int reward) : contiguousGameState(contiguousGameState), pi(pi), reward(reward) {
    // for(int i = 0; i < 8; i++){
    //   for(int j = 0; j < 8; j++){
    //     this->pi[i][j] = pi[i][j];
    //   }
    // }
  }
};

void writeRewards(std::vector<TrainingExample> &examples, int winner){
  
  // Tie
  if(winner == 0){
    return;
  }

  int player = Othello::BLACK;
  for(auto &example : examples){
    if(winner == player){
    example.reward = 1;
    }else{
      example.reward = -1;
    }
    
    if(player == Othello::BLACK){
      player = Othello::WHITE;
    }else{
      player = Othello::BLACK;
    }
  }
}


void selfPlay(int numGames, int numMCTSsims, NNet *nnet){ 
  
  printf("Executing self play with %d games and %d MCTS simulations", numGames, numMCTSsims);
   
  std::vector<TrainingExample> examples;


  for(int i = 0; i < numGames; i++){
    printf("Starting game %d", i);

  auto gen = std::mt19937{std::random_device{}()};
    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;
    // Create MCTS
    MCTS mcts(searchGameState, nnet);
    while(true){

      actualGameState.calculateLegalMoves();
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        writeRewards(examples, actualGameState.winner);
        break;
      }

      for(int j = 0; j < numMCTSsims; j++){
        searchGameState = actualGameState;
        mcts.search(searchGameState);
      }
      Policy improvedPolicy = mcts.getPI(actualGameState, 1);
      examples.push_back(TrainingExample(nnet->getContiguousGameState(actualGameState),
                                         improvedPolicy.pi,
                                         0));

      // Play random move
      actualGameState.playRandomMove();


    }
  }

  for(auto &example : examples){
    printf("=============== \n");
    printf("Training example: \n");
    // std::cout << example.contiguousGameState << std::endl;
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        printf("%f, ", example.pi[i][j]);
      }
      printf("\n");
    }
    printf("reward: %d \n", example.reward);
    printf("===============\n");
  }

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

int main(int argc, const char* argv[]){

  NNet *nnet = new NNet("../networks/output_model.pt");
 

  // auto start = std::chrono::high_resolution_clock::now();
  selfPlay(10, 25, nnet);
  // auto stop = std::chrono::high_resolution_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  // std::cout << "Runtime: " << duration.count() << "seconds" << std::endl;

  delete nnet;

  

}


