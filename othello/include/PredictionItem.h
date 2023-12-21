#ifndef PREDICTION_ITEM_H
#define PREDICTION_ITEM_H

#include <future>

struct PredictionItem {
  std::promise<std::pair<std::vector<float>, float>> promise;
  std::vector<float> contiguousGameState;

  PredictionItem(std::promise<std::pair<std::vector<float>, float>> promise, std::vector<float> contiguousGameState){
    this->promise = std::move(promise);
    this->contiguousGameState = contiguousGameState;
  }

  // Move constructor
  PredictionItem(PredictionItem&& predictionItem) noexcept
      : promise(std::move(predictionItem.promise)),
        contiguousGameState(std::move(predictionItem.contiguousGameState)) {}

};

#endif