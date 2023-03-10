#include "StateNode.h"
#include "NNet.h"
StateNode::StateNode(Othello::GameState &gameState, NNet *nnet){
    this->comparableGameState = gameState.getComparableGameState(); 
    // Initialize Q and N for all potential actions to 0
    // Initialize P using NN
 
    // Save actions as x,y pairs 
    std::vector<std::pair<unsigned int, unsigned int>> legalMoves 
      = gameState.getLegalMoves();


    auto nnetResult = nnet->predict(gameState);
    auto value = nnetResult.second.item<float>();
    // Get state value
    this->value = value; 

    if(legalMoves.size() == 0){
      this->noLegalMoves = true;
    }else{
      auto pvals = nnetResult.first;
    
      for(auto& action : legalMoves){
        int actionX = action.first;
        int actionY = action.second;

        float p_val_at_location = pvals[8*actionX + actionY].item<float>(); 
        this->actions.push_back(ActionValues(action, value, 1, p_val_at_location));
    }
  }
}
