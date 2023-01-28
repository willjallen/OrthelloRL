#ifndef NNET_H
#define NNET_H

#include <torch/script.h> // One-stop header.

class NNet{

  public:
    NNet(const char* modelPath);

    // Get policy (8x8 array) and value
    std::pair<torch::Tensor, torch::Tensor> predict(const Othello::GameState &gameState);

    // torch::Tensor getMaskedPvals(const Othello::GameState &gameState, torch::Tensor p_vals);

  private:
    torch::jit::script::Module _module;


};

#endif

