#include "SelfPlayEngine.h"
#include "Utils.h"
#include "NNet.h"


const int N = 8;

std::vector<std::vector<float>> getSymmetries(std::vector<float> &board) {
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

SelfPlayEngine::SelfPlayEngine(int threadId, int numMCTSSims){
  this->numMCTSSims_ = numMCTSSims;
  this->threadId_ = threadId;
}


void SelfPlayEngine::run(ThreadSafeQueue &inferenceQueue, std::vector<TrainingExample> &examples){ 
  
  std::cout << "Starting selfPlay thread: " + std::to_string(this->threadId_) + "\n";

  // Set up the game
  Othello::GameState actualGameState;
  Othello::GameState searchGameState;
  // Create MCTS
  MCTS mcts(searchGameState, inferenceQueue);
  int numSimsExecuted = 0;
  float temperature = 1;
  while(true){

    actualGameState.calculateLegalMoves();
    if(actualGameState.gameOver){
      actualGameState.calculateWinner();
      writeRewards(examples, actualGameState.winner);
      break;
    }

    for(int j = 0; j < this->numMCTSSims_; j++){
      searchGameState = actualGameState;
      mcts.search(searchGameState);
      numSimsExecuted++;
    }

    if(numSimsExecuted == 30) temperature = 0;

    std::vector<float> improvedPolicy = mcts.getPolicy(actualGameState, temperature);
    // std::cout << examples.size() << std::endl;
    // std::cout << examples.capacity() << std::endl;
    // std::cout << examples << std::endl;
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

  std::cout << "SelfPlay thread " << std::to_string(this->threadId_) << " finished executing." << std::endl;

  // Set board symmetries
  for (auto &example : examples) {
    example.boardSymmetries = getSymmetries(example.contiguousGameState);
    example.policySymmetries = getSymmetries(example.improvedPolicy);
  }
}
