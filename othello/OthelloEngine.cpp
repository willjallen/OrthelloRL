
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
  std::vector<float> contiguousGameState;
  std::vector<float> improvedPolicy;
  int reward;

  TrainingExample(std::vector<float> contiguousGameState, std::vector<float> improvedPolicy, int reward) : contiguousGameState(contiguousGameState), improvedPolicy(improvedPolicy), reward(reward) {
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






const int N = 8;

std::vector<std::vector<float>> findSymmetries(const std::vector<float> &board) {
    std::vector<std::vector<float>> symmetries;

    // Identity
    symmetries.push_back(board);

    // 90 degree rotation
    std::vector<float> rotated(N * N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            rotated[i + j * N] = board[(N - 1 - j) + i * N];
        }
    }
    symmetries.push_back(rotated);

    // 270 degree rotation
    std::reverse(rotated.begin(), rotated.end());
    symmetries.push_back(rotated);

    // 180 degree rotation
    rotated.clear();
    rotated.resize(N * N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            rotated[i + j * N] = board[(N - 1 - j) + i * N];
        }
    }
    std::vector<float> rotated_copy(rotated);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            rotated[i + j * N] = rotated_copy[(N - 1 - j) + i * N];
        }
    }
    symmetries.push_back(rotated);

    // Vertical reflection
    std::vector<float> reflected(N * N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            reflected[i + (N - 1 - j) * N] = board[i + j * N];
        }
    }
    symmetries.push_back(reflected);

    // Horizontal reflection
    std::reverse(reflected.begin(), reflected.end());
    symmetries.push_back(reflected);

    // Diagonal reflection
    std::vector<float> diag_reflected(N * N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            diag_reflected[j + i * N] = board[i + j * N];
        }
    }
    symmetries.push_back(diag_reflected);

    return symmetries;
}


void saveTrainingExamples(std::vector<TrainingExample> examples, std::string outputPath){

  std::cout << "Writing training examples to";
  std::cout << outputPath << "\n";
  // Create a JSON object to hold the serialized data
  json serializedData;
  serializedData["examples"] = json::array();
  
  for (const auto &example : examples) {
   // Identity
    json exampleData;

    auto boardSymmetries = findSymmetries(example.contiguousGameState);
    auto policySymmetries = findSymmetries(example.improvedPolicy);

    for(size_t i = 0; i < boardSymmetries.size(); i++){
      exampleData["contiguousGameState"] = boardSymmetries[i];    
      exampleData["pi"] = policySymmetries[i]; 
      exampleData["reward"] = example.reward;
      serializedData["examples"].push_back(exampleData);    
    }


  }
  
  // Write the serialized data to a file
  std::ofstream file(outputPath);
  file << serializedData.dump(-1);
  file.close();
}


std::pair<int, int> sampleDistribution(const std::vector<float> &dist) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> d(dist.begin(), dist.end());
    int index = d(gen);
    return {index / 8, index % 8};
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
    float temperature = 1;
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

      std::vector<float> improvedPolicy = mcts.getPolicy(actualGameState, temperature);
      examples.push_back(TrainingExample(actualGameState.getContiguousGameState(),
                                         improvedPolicy,
                                         0));

      // Uniformly sample according to improved policy
      std::pair<int, int> loc = sampleDistribution(improvedPolicy);

      // Play random move
      if(actualGameState.legalMovePresent){
        actualGameState.playMove(loc.first, loc.second);
      }else{
        actualGameState.pass();
      }


    }

  }


  saveTrainingExamples(fullExamples, outputPath);

}


void saveOutcome(int numGames, int numP1Wins, int numP2Wins, int numTies, std::string outputPath){
  std::cout << "Writing arena outcome to";
  std::cout << outputPath << "\n";
  // Create a JSON object to hold the serialized data
  json serializedData;
  
  serializedData["numGames"] = numGames;
  serializedData["numP1Wins"] = numP1Wins;
  serializedData["numP2Wins"] = numP2Wins;
  serializedData["numTies"] = numTies;


  // Write the serialized data to a file
  std::ofstream file(outputPath);
  file << serializedData.dump(-1);
  file.close();

}


void arena(int numGames, int numMCTSsims, NNet *nnetOne, NNet *nnetTwo, std::string outputPath){
  printf("Executing arena with %d games and %d MCTS simulations\n", numGames, numMCTSsims);
  
  float numP1Wins = 0;
  float numP2Wins = 0;
  float numTies = 0;

  for(int i = 0; i < numGames; i++){
    printf("Starting game %d\n", i);

    // Set up the game
    Othello::GameState actualGameState;

    Othello::GameState searchGameStateOne;
    Othello::GameState searchGameStateTwo;

    // Create a MCTS for each player 
    MCTS mctsOne(searchGameStateOne, nnetOne);
    MCTS mctsTwo(searchGameStateTwo, nnetTwo);


    // To encourage variety in the opening moves (instead of deterministic play), temperature is given as 
    // temperature = max(0.65^(x-1), 0.01), where temperature = 0 if < 0.01.
    float temperature = 1;
    while(true){
  
      std::vector<float> improvedPolicy;
     
      actualGameState.calculateLegalMoves();

      
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        // std::cout << actualGameState << std::endl;
        if(actualGameState.winner == Othello::BLACK){
          numP1Wins++;
        }else if(actualGameState.winner == Othello::WHITE){
          numP2Wins++;
        }else{
          numTies++;
        }
        break;
      }  

      if(!actualGameState.legalMovePresent){
        actualGameState.pass();
        continue;
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

      // Play move
      // std::cout << actualGameState << std::endl;

      // Uniformly sample according to improved policy
      // (pick the best action when temperature = 0)
      std::pair<int, int> loc = sampleDistribution(improvedPolicy);

      if(actualGameState.legalMovePresent){
        actualGameState.playMove(loc.first, loc.second);
      }else{
        actualGameState.pass();
      }

      // Decrease temperature
      if(temperature <= 0.01){
        temperature = 0;
      }else{
        temperature = std::max(pow(0.65, actualGameState.turnNumber + 1), 0.01);
      }

    }

  }


  saveOutcome(numGames, numP1Wins, numP2Wins, numTies, outputPath);

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
    std::cout << "  arena [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputPath]\n";
    return 0;
  }else if(strcmp(argv[1], "selfplay") == 0){
    if(argc < 6){
      std::cout << "Usage: selfplay [numGames] [MCTSsims] [modelPath] [outputPath]\n";
    }else{
      numGames = std::stoi(argv[2]);
      MCTSsims = std::stoi(argv[3]);
      modelPathOne = argv[4];
      outputPath = argv[5];

      NNet *nnet = new NNet(modelPathOne);

      selfPlay(numGames, MCTSsims, nnet, outputPath);

      delete nnet;
    }
    
  }else if(strcmp(argv[1], "arena") == 0){
    if(argc < 5){
      std::cout << "Usage: arena [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputPath]\n";
    }else{
      numGames = std::stoi(argv[2]);
      MCTSsims = std::stoi(argv[3]);
      modelPathOne = argv[4];
      modelPathTwo = argv[5];
      outputPath = argv[6];

      NNet *nnetOne = new NNet(modelPathOne);
      NNet *nnetTwo = new NNet(modelPathTwo);

      arena(numGames, MCTSsims, nnetOne, nnetTwo, outputPath);
    }
  }


  

}


