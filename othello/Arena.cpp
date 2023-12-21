
void arena(int numGames, int numMCTSsims, NNet *nnetOne, NNet *nnetTwo, std::string outputPath){
  printf("Executing arena with %d games and %d MCTS simulations\n", numGames, numMCTSsims);
  
  float numP1Wins = 0;
  float numP2Wins = 0;
  float numTies = 0;

  for(int i = 0; i < numGames; i++){
    printf("Starting game %d\n", i);

    // Set up the game
    Othello::GameState actualGameState;

    Othello::GameState searchGameStateOne;
    Othello::GameState searchGameStateTwo;

    // Create a MCTS for each player 
    MCTS mctsOne(searchGameStateOne, nnetOne);
    MCTS mctsTwo(searchGameStateTwo, nnetTwo);


    // To encourage variety in the opening moves (instead of deterministic play), temperature is given as 
    // temperature = max(0.65^(x-1), 0.01), where temperature = 0 if < 0.01.
    float temperature = 1;
    while(true){
  
      std::vector<float> improvedPolicy;
     
      actualGameState.calculateLegalMoves();

      
      if(actualGameState.gameOver){
        actualGameState.calculateWinner();
        // std::cout << actualGameState << std::endl;
        if(actualGameState.winner == Othello::BLACK){
          numP1Wins++;
        }else if(actualGameState.winner == Othello::WHITE){
          numP2Wins++;
        }else{
          numTies++;
        }
        break;
      }  

      if(!actualGameState.legalMovePresent){
        actualGameState.pass();
        continue;
      }
      
      if(actualGameState.currentPlayer == Othello::BLACK){
        for(int j = 0; j < numMCTSsims; j++){
          searchGameStateOne = actualGameState;
          mctsOne.search(searchGameStateOne);
        }
    
        improvedPolicy = mctsOne.getPolicy(actualGameState, temperature);
      }else{
        for(int j = 0; j < numMCTSsims; j++){
          searchGameStateTwo = actualGameState;
          mctsTwo.search(searchGameStateTwo);
        }
    
        improvedPolicy = mctsTwo.getPolicy(actualGameState, temperature);
      }

      // Play move
      // std::cout << actualGameState << std::endl;

      // Uniformly sample according to improved policy
      // (pick the best action when temperature = 0)
      std::pair<int, int> loc = sampleDistribution(improvedPolicy);

      if(actualGameState.legalMovePresent){
        actualGameState.playMove(loc.first, loc.second);
      }else{
        actualGameState.pass();
      }

      // Decrease temperature
      if(temperature <= 0.01){
        temperature = 0;
      }else{
        temperature = std::max(pow(0.65, actualGameState.turnNumber + 1), 0.01);
      }

    }

  }


  saveOutcome(numGames, numP1Wins, numP2Wins, numTies, outputPath);

}
