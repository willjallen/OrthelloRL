#include <torch/script.h> // One-stop header.
#include "Othello.h"
#include "NNet.h"

NNet::NNet(const char* modelPath){ 

  try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
    this->_module = torch::jit::load(modelPath);
    
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    // return -1;
  }

  std::cout << "Model loaded" << std::endl;

}

std::pair<torch::Tensor, torch::Tensor> NNet::predict(const Othello::GameState &gameState){
  

  // std::vector<std::vector<float>> policy;
  // float value;
 
  // Roll the game state into one contiguous 1d array of length 64
  torch::Tensor gameStateContiguous = torch::zeros(64);
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(gameState.currentPlayer == Othello::BLACK){
        gameStateContiguous.index({i*8 + j}) = 1 ? (gameState.board[i][j] == Othello::BLACK) : -1;
      }else{
        gameStateContiguous.index({i*8 + j}) = 1 ? (gameState.board[i][j] == Othello::WHITE) : -1;

      }
    }
  }
  
  // Create a vector of inputs.
  std::vector<torch::jit::IValue> inputs;
  
  // Transform into 1 channel x 8 x 8
  gameStateContiguous = gameStateContiguous.view({1, 8, 8});

  inputs.push_back(gameStateContiguous);

  // Run inference
  auto outputs = this->_module.forward(inputs).toTuple();
  torch::Tensor p_vals = outputs->elements()[0].toTensor();
  torch::Tensor v = outputs->elements()[1].toTensor();

  // std::cout << p_vals.index({0}) << std::endl;
  // std::cout << v.index({0}) << std::endl;

  // Recover probabilities
  // std::cout << "probs" << std::endl; 
  // std::cout << torch::exp(p_vals.index({0})) << std::endl;

  // We want the first channel of the output.
  // Additionally, we want to recover the log probabilities from p_vals
  return std::make_pair(torch::exp(p_vals.index({0})), v.index({0}));
  
}

// torch::Tensor NNet::getMaskedPvals(const Othello::GameState &gameState, torch:;Tensor p_vals){
//   
// }
