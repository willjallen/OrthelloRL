
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


void saveTrainingExamples(std::vector<TrainingExample> examples, std::string outputPath){

  std::cout << "Writing training examples to";
  std::cout << outputPath << "\n";
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
  std::ofstream file(outputPath);
  file << serializedData.dump(-1);
  file.close();
}

void selfPlay(int numGames, int numMCTSsims, NNet *nnet, std::string outputPath){ 
  
  printf("Executing self play with %d games and %d MCTS simulations\n", numGames, numMCTSsims);
   
  

  std::vector<TrainingExample> fullExamples;

  for(int i = 0; i < numGames; i++){
    std::vector<TrainingExample> examples;
    printf("Starting game %d\n", i);

    // Set up the game
    Othello::GameState actualGameState;
    Othello::GameState searchGameState;
    // Create MCTS
    MCTS mcts(searchGameState, nnet);
    int numSimsExecuted = 0;
    int temperature = 1;
    while(true){

      actualGameState.calculateLegalMoves();
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        writeRewards(examples, actualGameState.winner);
        fullExamples.insert(fullExamples.end(), examples.begin(), examples.end());
        break;
      }

      for(int j = 0; j < numMCTSsims; j++){
        searchGameState = actualGameState;
        mcts.search(searchGameState);
        numSimsExecuted++;
      }
  
      if(numSimsExecuted == 30) temperature = 0;

      std::vector<std::vector<float>> improvedPolicy = mcts.getPolicy(actualGameState, temperature);
      examples.push_back(TrainingExample(nnet->getContiguousGameState(actualGameState),
                                         improvedPolicy,
                                         0));

      // Play random move
      actualGameState.playRandomMove();


    }

  }


  saveTrainingExamples(fullExamples, outputPath);

}

void arena(int numGames, int numMCTSsims, NNet *nnetOne, NNet *nnetTwo, std::string outputPath){
  printf("Executing arena with %d games and %d MCTS simulations\n", numGames, numMCTSsims);
   

  for(int i = 0; i < numGames; i++){
    printf("Starting game %d\n", i);

    // Set up the game
    Othello::GameState actualGameState;

    Othello::GameState searchGameStateOne;
    Othello::GameState searchGameStateTwo;

    // Create a MCTS for each player 
    MCTS mctsOne(searchGameStateOne, nnetOne);
    MCTS mctsTwo(searchGameStateTwo, nnetTwo);


    // Temperature is set to 0 for evaluation
    int temperature = 0;
    std::vector<std::vector<float>> improvedPolicy;
    while(true){
      actualGameState.calculateLegalMoves();
      
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        break;
      }  
      
      if(actualGameState.currentPlayer == Othello::BLACK){
        for(int j = 0; j < numMCTSsims; j++){
          searchGameStateOne = actualGameState;
          mctsOne.search(searchGameStateOne);
        }
    
        improvedPolicy = mctsOne.getPolicy(actualGameState, temperature);
      }else{
        for(int j = 0; j < numMCTSsims; j++){
          searchGameStateTwo = actualGameState;
          mctsTwo.search(searchGameStateTwo);
        }
    
        improvedPolicy = mctsTwo.getPolicy(actualGameState, temperature);
      } 
      // Get action
      float maxVal = -9999;
      std::pair<unsigned int, unsigned int> chosenCoordinate;
      for(unsigned int i = 0; i < 8; i++){
        for(unsigned int j = 0; j < 8; j++){
          float val = improvedPolicy[i][j];
          if(val > maxVal){
            maxVal = val;
            chosenCoordinate.first = i;
            chosenCoordinate.second = j;
          }
        }
      }

      // Play move
      actualGameState.playMove(chosenCoordinate.first, chosenCoordinate.second);

    }

  }


  // saveOutcome(numP1Win, numP2Win, outputPath);

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

  std::string outputPath;



  if(argc < 2){
    std::cout << "usage: othello <command> [<args>]\n";
    std::cout << "commands:\n";
    std::cout << "  selfplay [numGames] [MCTSsims] [modelPath] [outputPath]\n";
    std::cout << "  arena [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputFilePath] [outputFilename]\n";
    return 0;
  }else if(strcmp(argv[1], "selfplay") == 0){
    if(argc < 6){
      std::cout << "Usage: selfplay [numGames] [MCTSsims] [modelPath] [outputFilePath] [outputFilename]\n";
    }else{
      numGames = std::stoi(argv[2]);
      MCTSsims = std::stoi(argv[3]);
      modelPathOne = argv[4];
      outputPath = argv[5];

      NNet *nnet = new NNet(modelPathOne);

      selfPlay(numGames, MCTSsims, nnet, outputPath);

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

      // NNet *nnet = new NNet(modelPathOne);

      // selfPlay(numGames, numMCTSItr, nnet, outputFilePath, outputFilename);
    }
  }


  

}


