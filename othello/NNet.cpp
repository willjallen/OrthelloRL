#include <torch/script.h> // One-stop header.
#include "Othello.h"
#include "NNet.h"
#include <iostream>
NNet::NNet(const char* modelPath){ 

  try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
    this->_module = torch::jit::load(modelPath, at::kCUDA);
    
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    // return -1;
  }

  std::cout << "Model loaded" << std::endl;
}

std::pair<torch::Tensor, torch::Tensor> NNet::predict(const Othello::GameState &gameState){

  torch::NoGradGuard no_grad;
  // auto start = std::chrono::high_resolution_clock::now();

  // Roll the game state into one contiguous 1d array of length 64
  torch::Tensor gameStateContiguous = torch::zeros(64);
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int boardValue = 0;
      if(gameState.currentPlayer == Othello::BLACK){
        if(gameState.board[i][j] == Othello::BLACK){
          boardValue = 1;
        }else if(gameState.board[i][j] == Othello::WHITE){
          boardValue = -1;
        }else{
          boardValue = 0;
        }
        
      }else{
        if(gameState.board[i][j] == Othello::WHITE){
          boardValue = 1;
        }else if(gameState.board[i][j] == Othello::BLACK){
          boardValue = -1;
        }else{
          boardValue = 0;
        }
        
      }

      gameStateContiguous[i*8 + j] = boardValue;
    }
  }

  // auto stop = std::chrono::high_resolution_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  // std::cout << "Construct board Runtime: " << duration.count() << "ms" << std::endl;
  
  // Create a vector of inputs.
  std::vector<torch::jit::IValue> inputs;
  
  // Transform into 1x8x8
  gameStateContiguous = gameStateContiguous.view({1, 8, 8});
  // start = std::chrono::high_resolution_clock::now();


  //https://stackoverflow.com/questions/53570334/documentation-for-pytorch-tocpu-or-tocuda
  torch::Tensor gameStateContiguousCUDA = gameStateContiguous.to(at::kCUDA);
  // stop = std::chrono::high_resolution_clock::now();
  // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  // std::cout << "Runtime copy: " << duration.count() << "ms" << std::endl;
  // Now we need 63 1x8x8 dummy channels
  // std::cout << gameStateContiguous << std::endl;
  // std::cout << "HERE10" << std::endl;
  // std::cout << &(this->_module) << std::endl;

 // // Create a vector of tensors
 //  std::vector<torch::Tensor> tensors;
 //  tensors.push_back(gameStateContiguous);
 //
 //  // Create 63 tensors of the same shape filled with zeros
 //  for (int i = 0; i < 63; i++) {
 //      tensors.push_back(torch::zeros({1, 8, 8}));
 //  }
 //
 //  // Concatenate the tensors along the first dimension
 //  auto input_64ch = torch::cat(tensors, 0);
 //  std::cout << input_64ch.sizes() << std::endl;
  inputs.push_back(gameStateContiguousCUDA);

  // Run inference
  // std::cout << "Inference" << std::endl;
  // start = std::chrono::high_resolution_clock::now();
  auto outputs = this->_module.forward(inputs).toTuple();
  // stop = std::chrono::high_resolution_clock::now();
  // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  // std::cout << "Runtime inference: " << duration.count() << "ms" << std::endl;
  // std::cout << outputs << std::endl;
  torch::Tensor p_vals = outputs->elements()[0].toTensor();
  torch::Tensor v = outputs->elements()[1].toTensor();

  // std::cout << p_vals[0] << std::endl;
  // std::cout << v[0] << std::endl;
  //
  // Recover probabilities
  // std::cout << "probs" << std::endl; 
  // std::cout << torch::exp(p_vals.index({0})) << std::endl;

  // We want the first channel of the output.
  // Additionally, we want to recover the log probabilities from p_vals

  return std::make_pair(torch::exp(p_vals[0]), v[0]);



}

