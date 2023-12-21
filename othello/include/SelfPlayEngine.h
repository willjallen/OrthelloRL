#ifndef SELF_PLAY_ENGINE_H
#define SELF_PLAY_ENGINE_H

#include "MCTS.h"
#include "Othello.h"
#include "NNet.h"
#include "ThreadSafeQueue.h"

struct TrainingExample {
  std::vector<float> contiguousGameState;
  std::vector<float> improvedPolicy;

  std::vector<std::vector<float>> boardSymmetries;
  std::vector<std::vector<float>> policySymmetries;

  int reward;

  TrainingExample(std::vector<float> contiguousGameState, std::vector<float> improvedPolicy, int reward) :
    contiguousGameState(contiguousGameState), improvedPolicy(improvedPolicy), reward(reward) {}
};


class SelfPlayEngine {
  public:
    void run(ThreadSafeQueue &inferenceQueue, std::vector<TrainingExample> &examples);
    SelfPlayEngine(int threadId, int numMCTSSims);
  private:
    int threadId_;
    int numMCTSSims_;
};

#endif