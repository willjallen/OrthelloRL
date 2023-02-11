#include <torch/script.h> // One-stop header.
#include "Othello.h"
#include "NNet.h"
#include <iostream>
NNet::NNet(std::string modelPath){ 

    
  std::cout << "Loading model..\n";
  try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
    this->_module = torch::jit::load(modelPath, at::kCUDA);
    
  }
  catch (const c10::Error& e) {
    std::cerr << "Error loading the model\n";
  }

  std::cout << "Model loaded\n";
}



std::pair<torch::Tensor, torch::Tensor> NNet::predict(const Othello::GameState &gameState){

  torch::NoGradGuard no_grad;
  // auto start = std::chrono::high_resolution_clock::now();

  // Roll the game state into one contiguous 1d array of length 64
   // auto stop = std::chrono::high_resolution_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  // std::cout << "Construct board Runtime: " << duration.count() << "ms" << std::endl;
  std::vector<float> gameStateContiguousVec = gameState.getContiguousGameState();
  size_t vecSize = gameStateContiguousVec.size();
  torch::Tensor gameStateContiguousTensor= torch::from_blob(gameStateContiguousVec.data(), {vecSize}, torch::dtype(torch::kFloat32));
  // size_t vec_size = gameStateContiguousVec.size();
  //
  // std::vector<double> floatVec(vec_size);
  // std::transform(gameStateContiguousVec.begin(), gameStateContiguousVec.end(), floatVec.begin(), 
  //     [](int i){ return static_cast<float>(i); });
  //

  // torch::Tensor gameStateContiguous = torch::from_blob(floatVec.data(), vec_size, torch::kFloat64);
  // Create a vector of inputs.
  std::vector<torch::jit::IValue> inputs;
  
  // Transform into 1x8x8
  gameStateContiguousTensor = gameStateContiguousTensor.view({1, 8, 8});
  // std::cout << gameStateContiguousTensor << std::endl;
  // start = std::chrono::high_resolution_clock::now();


  //https://stackoverflow.com/questions/53570334/documentation-for-pytorch-tocpu-or-tocuda
  torch::Tensor gameStateContiguousCUDA = gameStateContiguousTensor.to(at::kCUDA);
  // std::cout << gameStateContiguousCUDA << std::endl;
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

  return std::make_pair(torch::exp(p_vals[0]), v[0]);



}

