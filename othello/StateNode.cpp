#include <future>
#include "StateNode.h"
#include "PredictionItem.h"

StateNode::StateNode(Othello::GameState &gameState, ThreadSafeQueue &inferenceQueue){
    this->comparableGameState = gameState.getComparableGameState();
    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
 
    // Save actions as x,y pairs 
    std::vector<std::pair<unsigned int, unsigned int>> legalMoves 
      = gameState.getLegalMoves();


    auto nnetResult = this->getPrediction(gameState.getContiguousGameState(), inferenceQueue);
    
    // Get state value
    auto value = nnetResult.second;
    this->value = value; 

    if(legalMoves.size() == 0){
      this->noLegalMoves = true;
    }else{
      auto pvals = nnetResult.first;
    
      for(auto& action : legalMoves){
        int actionX = action.first;
        int actionY = action.second;

        float p_val_at_location = pvals[8*actionX + actionY]; 
        this->actions.push_back(ActionValues(action, value, 1, p_val_at_location));
    }
  }
}

std::pair<std::vector<float>, float> StateNode::getPrediction(std::vector<float> contigousGameState, ThreadSafeQueue& inferenceQueue){

  // Create the promise and future
  std::promise<std::pair<std::vector<float> , float>> promise;
  std::future<std::pair<std::vector<float>, float>> future = promise.get_future();

  PredictionItem predictionItem(std::move(promise), contigousGameState);

  // Queue the inference task
  inferenceQueue.push(std::move(predictionItem));

  // Await the result
  return future.get();

}
