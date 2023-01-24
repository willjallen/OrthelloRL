
bool inBounds(int x, int y)
{
    if (0 <= x && x < 8 && 0 <= y && y < 8)
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int getOtherPlayer(unsigned int player)
{
    return player == 1 ? 2 : 1;
}


GameState& GameState::operator=(const GameState &src){
  this.noLegalMoveOnLastTurn = src->noLegalMoveOnLastTurn;
  this.gameOver = src->gameOver;
  
  this.legalMovesCalulated = src->legalMovesCalulated;
  
  this.currentPlayer = src->currentPlayer;
 
  this.turnNumber = src->turnNumber;
  
  this.numBlackTiles = src->numBlackTiles;
  this.numWhiteTiles = src->numWhiteTiles;
  
  this.numBlackLegalMoves = src->numBlackLegalMoves;
  this.numWhiteLegalMoves = src->numWhiteLegalMoves;
  
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      this.board[i][j] = src->board[i][j];
      this.legalMoves[i][j] = src->legalMoves[i][j];
      this.moveLines[i][j] = src->moveLines[i][j];
    }
  }
}


void GameState::switchPlayers()
{
    this->currentPlayer = getOtherPlayer(this->currentPlayer);
}


void GameState::GameState()
{

    this->noLegalMoveOnLastTurn = false;
    this->gameOver = false;
    this->legalMovesCalulated = false;

    this->currentPlayer = 1;
    this->turnNumber = 0;

    this->numBlackLegalMoves = 2;
    this->numWhiteLegalMoves = 2;

    this->numBlackTiles = 0;
    this->numWhiteTiles = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            this->board[x][y] = DEFAULT_BOARD[x][y];
        }
    }

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {

            this->legalMoves[x][y] = 0;

            for (int l = 0; l < 8; l++)
            {
                Line *currLine = &this->moveLines[x][y].lines[l];
                currLine->valid = false;
                currLine->length = 0;
                currLine->assignedTurnNumber = 0;
            }
        }
    }
}

void GameState::~GameState(){

}

void GameState::findLegalMove(int startingX, int startingY, unsigned int player,
                                         unsigned int otherPlayer)
{

    // For all 8 cardinal directions
    for (int direction = 0; direction < 8; direction++)
    {

        int dx = DIRECTION_COMPONENTS[direction][0];
        int dy = DIRECTION_COMPONENTS[direction][1];

        // Check if the first adjacent cell is in bounds, and if it is whether it is the other player
        if (inBounds(startingX + dx, startingY + dy))
        {
            if (this->board[startingX + dx][startingY + dy] != otherPlayer)
                continue;
        }
        else
        {
            continue;
        }

        // Start at the adjacent cell
        int x = startingX + dx;
        int y = startingY + dy;

        // Track distance traveled
        unsigned int distance = 1;

        // Continue until we reach the end of the board
        while (inBounds(x + dx, y + dy))
        {

            // In the case that a line is already flanked on both sides by a chip (BWWB), we can not place a B chip
            // like (BWWBB)
            if (this->board[x][y] == player)
                goto beach;

            // Found legal move
            if (this->board[x][y] == EMPTY || this->board[x][y] == LEGAL)
            {

                if (this->board[x][y] != LEGAL)
                {
                    if (player == 1)
                    {
                        this->numBlackLegalMoves++;
                    }
                    else
                    {
                        this->numWhiteLegalMoves++;
                    }
                }

                this->legalMoves[x][y] = LEGAL;

                // Get Line at coordinate and complement direction (we are moving towards the legal move, and if the
                // legal move is played we flip tiles from the reverse direction)
                Line *line = &this->moveLines[x][y].lines[DIRECTIONS_COMPLEMENT[direction]];
                line->valid = true;
                line->length = distance;
                line->assignedTurnNumber = this->turnNumber;

                goto beach;
            }

            // Keep searching
            if (this->board[x][y] == otherPlayer)
            {
                x += dx;
                y += dy;
                distance++;
            }
        }

        // If we reached the end of the board, and the cell is empty, set it to legal
        if (this->board[x][y] == EMPTY || this->board[x][y] == LEGAL)
        {

            if (this->board[x][y] != LEGAL)
            {
                if (player == 1)
                {
                    this->numBlackLegalMoves++;
                }
                else
                {
                    this->numWhiteLegalMoves++;
                }
            }

            this->legalMoves[x][y] = LEGAL;

            // Get Line at coordinate and complement direction (we are moving towards the legal move, and if the
            // legal move is played we flip tiles from the reverse direction)
            Line *line = &this->moveLines[x][y].lines[DIRECTIONS_COMPLEMENT[direction]];
            line->valid = true;
            line->length = distance;
            line->assignedTurnNumber = this->turnNumber;
        }

    beach:
        continue;
    }
}

/**
 * @brief Calculates and returns and 8x8 array of legal positions
 *
 * @param board 8x8 board where 0 = empty, 1 = player 1, 2 = player 2
 * @param player 1 = player 1, 2 = player 2
 * @return int**
 */

void GameState::calculateLegalMoves()
{
    unsigned int otherPlayer = getOtherPlayer(this->currentPlayer);

    // Clear previous legal moves
    for (unsigned int x = 0; x < 8; x++)
    {
        for (unsigned int y = 0; y < 8; y++)
        {
            if (this->legalMoves[x][y] == LEGAL)
                this->legalMoves[x][y] = EMPTY;
        }
    }

    this->numBlackLegalMoves = 0;
    this->numWhiteLegalMoves = 0;

    this->numBlackTiles = 0;
    this->numWhiteTiles = 0;

    for (unsigned int x = 0; x < 8; x++)
    {
        for (unsigned int y = 0; y < 8; y++)
        {

            if (this->board[x][y] == EMPTY)
                continue;

            if (this->board[x][y] == BLACK)
                this->numBlackTiles += 1;
            if (this->board[x][y] == WHITE)
                this->numWhiteTiles += 1;

            if (this->board[x][y] == this->currentPlayer)
            {
                // Check the 8 cardinal directions, do not wrap around the board
                findLegalMove(this, x, y, this->currentPlayer, otherPlayer);
            }
        }
    }
  this->legalMovesCalulated = true;
}

/**
 * @brief Determines whether the current player has no legal moves
 *
 * @paramthis 
 * @return boolean
 */
bool GameState::shouldPass()
{
  if((this->currentPlayer == BLACK && this->numBlackLegalMoves == 0)
      || (this->currentPlayer == WHITE && this->numWhiteLegalMoves == 0)) { return true; }
  return false;
}


void GameState::calculateWinner(){
  if(this->numBlackTiles > this->numWhiteTiles){
    this->winner = BLACK;
  }else if(this->numBlackTiles < this->numWhiteTiles){
     this->winner = WHITE;
  }else{
    this->winner = 0;
  }
}




void GameState::playMove(, unsigned int startingX, unsigned int startingY)
{

    if (this->currentPlayer == 1)
    {
        this->numBlackTiles++;
    }
    else
    {
        this->numWhiteTiles++;
    }

    // Get all lines at the coordinate
    MoveLines moveLines = this->moveLines[startingX][startingY];
  
    bool noLegalMoves = true;

    // For each direction, check if a valid line exists
    for (unsigned int direction = 0; direction < 8; direction++)
    {

        // Get our particular line
        Line line = moveLines.lines[direction];

        if (line.valid && line.assignedTurnNumber == this->turnNumber)
        {
            
          noLegalMoves = false;
            // Flip the tiles on the line

            unsigned int x = startingX;
            unsigned int y = startingY;

            unsigned int dx = DIRECTION_COMPONENTS[direction][0];
            unsigned int dy = DIRECTION_COMPONENTS[direction][1];

            for (unsigned int i = 0; i < line.length; i++)
            {
                this->board[x][y] = this->currentPlayer;

                if (this->currentPlayer == 1)
                {
                    this->numBlackTiles++;
                    this->numWhiteTiles--;
                }
                else
                {
                    this->numWhiteTiles++;
                    this->numBlackTiles--;
                }

                if (this->numBlackTiles < 0)
                {
                    this->numBlackTiles = 0;
                }

                if (this->numWhiteTiles < 0)
                {
                    this->numWhiteTiles = 0;
                }

                x += dx;
                y += dy;
            }
        }
    }
    
    if(this->noLegalMoveOnLastTurn && noLegalMoves){
      this->gameOver = true;
      calculateWinner(this);
      return; 
    }else if(noLegalMoves){
      this->noLegalMoveOnLastTurn = true;
    }else{
      this->noLegalMoveOnLastTurn = false;
    }

    this->turnNumber++;
    this->legalMovesCalulated = false;
    switchPlayers(this);
}


// Uniformly sample the legal action space

void GameState::playRandomMove()
{
    Coordinate potentialCoordinates[64];
    int idx = 0;
    // Collect all valid lines
    for (unsigned int x = 0; x < 8; x++)
    {
        for (unsigned int y = 0; y < 8; y++)
        {
            // Find at least one line on this tile
            if (this->legalMoves[x][y] == LEGAL)
            {

                // Save it
                potentialCoordinates[idx].occupied = true;
                potentialCoordinates[idx].x = x;
                potentialCoordinates[idx].y = y;

                idx++;

                break;
            }
        }
    }

    // We now have idx number of lines to choose from
    unsigned int choice = rand() % idx;
    unsigned int randomX = potentialCoordinates[choice].x;
    unsigned int randomY = potentialCoordinates[choice].y;

    playMove(this, randomX, randomY);
}  


std::vector<std::pair<int,int>> getLegalMoves(){

  if(!this->legalMovesCalulated){
    calculateLegalMoves(this);
  }

  std::vector<std::pair<int, int>> legalMoves;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(this->board[i][j] == LEGAL){
        legalMoves.push_back(std::pair<int,int>(i, j));
      }
    }
  }

  return legalMoves;

}  

// 64 digit base 3 number
uint64_t getHashedGameState(){
  uint64_t whiteVec = 0;
  uint64_t blackVec = 0;
  /* Seed */
  std::random_device rd;

  /* Random number generator */
  std::default_random_engine generator(rd());

  /* Distribution on which to apply the generator */
  std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);

for(int i = 0; i < 8; i++){
  for(int j = 0; j < 8; j++){
      if(this->board[i][j] == BLACK){
        blackVec = blackVec | 1ULL << (i*j);
      }else if(this->board[i][j] == WHITE){
        whiteVec = whiteVec | 1ULL << (i*j);
      }
  }
}

// Should probably check for collisions
uint64_t hash = (whiteVec ^ distribution(generator)) ^ (blackVec ^ distribution(generator));


return hash; 
}



