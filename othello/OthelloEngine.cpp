
#include "MCTS.h"
#include "Othello.h"
#include "NNet.h"

#include "SelfPlayEngine.h"


#include <iostream>
#include <torch/script.h> // One-stop header.
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <assert.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


void saveTrainingExamples(std::vector<TrainingExample> examples, std::string outputPath){

  std::cout << "Writing training examples to";
  std::cout << outputPath << "\n";
  // Create a JSON object to hold the serialized data
  json serializedData;
  serializedData["examples"] = json::array();
 
  for (const auto &example : examples) {
    json exampleData;

    assert(example.boardSymmetries.size() == example.policySymmetries.size());

   // Write symmetries 
    for(size_t i = 0; i < example.boardSymmetries.size(); i++){
      exampleData["contiguousGameState"] = example.boardSymmetries[i];    
      exampleData["pi"] = example.policySymmetries[i]; 
      exampleData["reward"] = example.reward;
      serializedData["examples"].push_back(exampleData);    
    }


  }
  
  // Write the serialized data to a file
  std::ofstream file(outputPath);
  file << serializedData.dump(-1);
  file.close();
}



void saveArenaOutcome(int numGames, int numP1Wins, int numP2Wins, int numTies, std::string outputPath){
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


void selfPlayWorkerThread(int threadId, int numMCTSSims, ThreadSafeQueue &inferenceQueue, std::vector<TrainingExample> &examples){
  SelfPlayEngine engine(threadId, numMCTSSims);
  engine.run(inferenceQueue, examples);
}

void nnetInferenceThread(NNet *nnet, std::atomic<bool> &stopFlag, ThreadSafeQueue &inferenceQueue){
  nnet->run(stopFlag, inferenceQueue);
}

void selfPlay(int numGames, int numThreads, int numMCTSSims, NNet *nnet, std::string outputPath){

  std::cout << "Starting selfPlay engine\n";

  // Construct shared inference queue
  ThreadSafeQueue sharedInferenceQueue;

  std::atomic<bool> stopFlag{false};

  // Create inference thread
  std::thread inferenceThread(nnetInferenceThread, nnet, std::ref(stopFlag), std::ref(sharedInferenceQueue));

  // Create a pool of threads and execute them
  std::vector<std::thread> threads;
  std::vector<std::vector<TrainingExample>> trainingExamples;

  trainingExamples.reserve(numThreads);

  for(int i = 0; i < numThreads; ++i){
    trainingExamples.emplace_back(std::vector<TrainingExample>());
    trainingExamples[i].reserve(64);
  }

  for(int i = 0; i < numThreads; ++i) {
      threads.push_back(std::thread(selfPlayWorkerThread, i, numMCTSSims, std::ref(sharedInferenceQueue), std::ref(trainingExamples[i])));
  }


  // Wait for threads to finish
  for(auto& thread : threads){
    thread.join();
  }

  std::cout << "Worker threads finished\n";

  // Stop inference thread
  stopFlag = true;
  inferenceThread.join();

  std::cout << "Inference thread finished\n";

  // Write training examples to file

  std::vector<TrainingExample> allExamples;

  for(auto& trainingExampleVec : trainingExamples){
    for(auto& trainingExample : trainingExampleVec){
      allExamples.push_back(trainingExample);
    }
  }

  std::cout << "Saving training example\n";
  saveTrainingExamples(allExamples, outputPath);

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

      selfPlay(numGames, 64, MCTSsims, nnet, outputPath);

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

      // arena(numGames, MCTSsims, nnetOne, nnetTwo, outputPath);
    }
  }


  

}


