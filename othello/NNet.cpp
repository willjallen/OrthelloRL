#include <torch/script.h> // One-stop header.
#include <iostream>
#include <algorithm>

#include "Othello.h"
#include "NNet.h"
#include "PredictionItem.h"

NNet::NNet(std::string modelPath){ 

  std::cout << "Loading model..\n";
  try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
    this->module_ = torch::jit::load(modelPath, at::kCUDA);
    
  }
  catch (const c10::Error& e) {
    std::cerr << "Error loading the model\n";
    std::cout << e.what() << std::endl;
  }

  std::cout << "Model loaded\n";
}
// void displayGPUMemory()
// {
// 	CUresult uRet;
// 	size_t free1;
// 	size_t total1;
// 	uRet = cuMemGetInfo(&free1, &total1);
// 	if (uRet == CUDA_SUCCESS)
// 		printf("FreeMemory = %d Mb in TotalMeory = %d Mb\n", free1 / 1024 / 1024, total1 / 1024 / 1024);
// }

void NNet::run(std::atomic<bool> &stopFlag, ThreadSafeQueue &inferenceQueue_){

 std::cout << "Starting inference thread\n"; 
 auto past_time = std::chrono::high_resolution_clock::now();
  while(!stopFlag){
    // std::cout << stopFlag << std::endl;
    // When the prediction queue size is >= 64 OR 200 ms has passed, run inference on all items
    auto now_time = std::chrono::high_resolution_clock::now();
    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - past_time).count();
    if (inferenceQueue_.size() >= 64 || time_diff >= 200) {
        // std::cout << "Processing inference queue\n";

        int inferenceCount = std::min(64, static_cast<int>(inferenceQueue_.size()));
        if(inferenceCount == 0){
          goto beach;
        }

        std::vector<PredictionItem> predictionItems;
        predictionItems.reserve(inferenceCount);
        for (int i = 0; i < inferenceCount; ++i) {
            predictionItems.emplace_back(std::move(inferenceQueue_.pop()));
        }

        // displayGPUMemory();
        auto resultTensor = this->predict(predictionItems);
        // std::cout << resultTensor << "\n";
        
        auto p_vals_tensor = resultTensor.first;
        auto value_tensor = resultTensor.second;


        // Copy the tensor to the CPU
        auto p_vals_cpu = p_vals_tensor.to(torch::kCPU);
        auto values_cpu = value_tensor.to(torch::kCPU);

        // Fulfill each promise 
        for (int i = 0; i < inferenceCount; ++i) {
            // std::cout << "Fulfilling promises\n";
            // Get pvals
            const float* p_vals_ptr = p_vals_cpu[i].data_ptr<float>(); 
            // std::cout << p_vals_ptr << "\n";
            int num_elements = 64;
            std::vector<float> p_vals(p_vals_ptr, p_vals_ptr + num_elements);
            // std::cout << p_vals << "\n";
            // std::cout << "Processed pvals\n";

            // Get value
            float value = values_cpu[i].item<float>();
            // std::cout << "Processed value\n";
            
            // Fulfill promise
            predictionItems[i].promise.set_value(std::pair<std::vector<float>, float>(p_vals, value));
            // std::cout << "Processed promise\n";

        }

        // std::cout << "Done processing\n";

      past_time = now_time;
      now_time = std::chrono::high_resolution_clock::now();
    }

    beach: continue;

  }

}



std::pair<torch::Tensor, torch::Tensor> NNet::predict(const std::vector<PredictionItem> &predictionItems){


  // std::cout << "Executing prediction\n";
  // c10::cuda::CUDACachingAllocator::emptyCache();
  // displayGPUMemory();
  // Set no_grad flag for faster inference
  torch::NoGradGuard no_grad;


  // Concatenate game states from all prediction items into a 64x8x8 tensor
  std::vector<torch::Tensor> gameStateTensorVec;
  for(auto& predictionItem : predictionItems){
    gameStateTensorVec.push_back(torch::from_blob(const_cast<float*>(predictionItem.contiguousGameState.data()), {1, 8, 8}, torch::kFloat32));
  }

  torch::Tensor gameStateTensor = torch::cat(gameStateTensorVec, 0);

  // std::cout << gameStateTensor << "\n";
  //https://stackoverflow.com/questions/53570334/documentation-for-pytorch-tocpu-or-tocuda
  torch::Tensor gameStateTensorCUDA = gameStateTensor.to(at::kCUDA);

  // exit(0);
  // displayGPUMemory();

  // Create a vector of inputs.
  std::vector<torch::jit::IValue> inputs;
  
  inputs.push_back(gameStateTensorCUDA);

  // Run inference
  // std::cout << "Forward pass\n";
  auto outputs = this->module_.forward(inputs).toTuple();

  torch::Tensor p_vals = outputs->elements()[0].toTensor();
  torch::Tensor v = outputs->elements()[1].toTensor();

  return std::make_pair(torch::exp(p_vals), v);
}

