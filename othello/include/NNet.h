#ifndef NNET_H
#define NNET_H

#include <torch/script.h> // One-stop header.

class NNet{

  public:
    NNet(std::string modelPath);

    // Get policy (8x8 array) and value
    std::pair<torch::Tensor, torch::Tensor> predict(const Othello::GameState &gameState);
    
    torch::jit::script::Module _module;
  private:


};

#endif

