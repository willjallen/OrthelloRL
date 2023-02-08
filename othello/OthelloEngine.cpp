
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

#include <nlohmann/json.hpp>
using json = nlohmann::json;


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


void saveTrainingExamples(std::vector<TrainingExample> examples, std::string outputFilePath, std::string outputFilename){

  std::cout << "Writing training examples to";
  std::cout << outputFilePath + outputFilename;
  // Create a JSON object to hold the serialized data
  json serializedData;
  serializedData["examples"] = json::array();
  
  for (const auto &example : examples) {
    json exampleData;
    auto contiguousGameStateData = example.contiguousGameState.data<float>();
    exampleData["contiguousGameState"] = std::vector<float>(contiguousGameStateData, contiguousGameStateData + example.contiguousGameState.numel());
    
    
  exampleData["pi"] = json::array();
  int idx = 0;
  for (const auto &piRow : example.pi) {
    if(idx >= 8) break;
    for (const auto &piVal : piRow) {
      exampleData["pi"].push_back(piVal);
    }
    idx++;
  }
    
    exampleData["reward"] = example.reward;
    serializedData["examples"].push_back(exampleData);
  }
  
  // Write the serialized data to a file
  std::string fullFilePath = outputFilePath + outputFilename;
  std::ofstream file(fullFilePath);
  file << serializedData.dump(-1);
  file.close();
}

void selfPlay(int numGames, int numMCTSsims, NNet *nnet, std::string outputFilePath, std::string outputFilename){ 
  
  printf("Executing self play with %d games and %d MCTS simulations\n", numGames, numMCTSsims);
   
  std::vector<TrainingExample> examples;


  for(int i = 0; i < numGames; i++){
    printf("Starting game %d\n", i);

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
        saveTrainingExamples(examples, outputFilePath, outputFilename);
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

  // for(auto &example : examples){
  //   printf("=============== \n");
  //   printf("Training example: \n");
  //   // std::cout << example.contiguousGameState << std::endl;
  //   for(int i = 0; i < 8; i++){
  //     for(int j = 0; j < 8; j++){
  //       printf("%f, ", example.pi[i][j]);
  //     }
  //     printf("\n");
  //   }
  //   printf("reward: %d \n", example.reward);
  //   printf("===============\n");
  // }
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

  // arg0 othello
  // arg1 command

  int numGames;
  int MCTSsims;
  
  std::string modelPathOne;
  std::string modelPathTwo;

  std::string outputFilePath;
  std::string outputFilename;


  NNet *nnet = nullptr;

  if(argc < 2){
    std::cout << "usage: othello <command> [<args>]\n";
    std::cout << "commands:\n";
    std::cout << "  selfplay [numGames] [MCTSsims] [modelPath] [outputFilePath] [outputFilename]\n";
    std::cout << "  pit [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputFilePath] [outputFilename]\n";
    return 0;
  }else if(strcmp(argv[1], "selfplay") == 0){
    if(argc < 6){
      std::cout << "Usage: selfplay [numGames] [MCTSsims] [modelPath] [outputFilePath] [outputFilename]\n";
    }else{
      numGames = std::stoi(argv[2]);
      MCTSsims = std::stoi(argv[3]);
      modelPathOne = argv[4];
      outputFilePath = argv[5];
      outputFilename = argv[6];

      NNet *nnet = new NNet(modelPathOne);

      selfPlay(numGames, MCTSsims, nnet, outputFilePath, outputFilename);

      delete nnet;
    }
    
  }else if(strcmp(argv[1], "pit") == 0){
    if(argc < 5){
      std::cout << "Usage: selfplay [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputFilePath] [outputFilename]\n";
    }else{
      numGames = std::stoi(argv[2]);
      MCTSsims = std::stoi(argv[3]);
      modelPathOne = argv[4];
      modelPathTwo = argv[5];
      outputFilePath = argv[6];
      outputFilename = argv[7];

      // NNet *nnet = new NNet(modelPathOne);

      // selfPlay(numGames, numMCTSItr, nnet, outputFilePath, outputFilename);
    }
  }


  

}


