#ifndef NNET_H
#define NNET_H

#include <torch/script.h> // One-stop header.
#include "ThreadSafeQueue.h"
#include "PredictionItem.h"


class NNet{

  public:
    NNet(std::string modelPath);
    std::pair<torch::Tensor, torch::Tensor> predict(const std::vector<PredictionItem> &predictionItems);
    void run(std::atomic<bool> &stopFlag, ThreadSafeQueue &inferenceQueue_);
  private:
    torch::jit::script::Module module_;

};

#endif

