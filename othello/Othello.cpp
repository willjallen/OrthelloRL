#include "Othello.h"
#include <random>
#include <ostream>
#include <sstream>
#include <iostream> 
namespace Othello {


static std::random_device rd; 
static std::mt19937 rng{rd()};

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
  this->noLegalMoveOnLastTurn = src.noLegalMoveOnLastTurn;
  this->gameOver = src.gameOver;
  this->legalMovesCalulated = src.legalMovesCalulated;
  this->legalMovePresent = src.legalMovePresent;
  
  this->currentPlayer = src.currentPlayer;
  this->turnNumber = src.turnNumber;
  
  this->numBlackTiles = src.numBlackTiles;
  this->numWhiteTiles = src.numWhiteTiles;
  
  this->numBlackLegalMoves = src.numBlackLegalMoves;
  this->numWhiteLegalMoves = src.numWhiteLegalMoves;
  
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      this->board[x][y] = src.board[x][y];
      this->legalMoves[x][y] = src.legalMoves[x][y];
  
      for (int l = 0; l < 8; l++)
      {
          const Line *srcLine = &(src.moveLines[x][y].lines[l]);
          this->moveLines[x][y].lines[l].valid = srcLine->valid;
          this->moveLines[x][y].lines[l].length = srcLine->length;
          this->moveLines[x][y].lines[l].assignedTurnNumber = srcLine->assignedTurnNumber;
          // currLine->valid = false;
          // currLine->length = 0;
          // currLine->assignedTurnNumber = 0;
      }
    }
  }

  return *this;
}


void GameState::switchPlayers()
{
    this->currentPlayer = getOtherPlayer(this->currentPlayer);
}


GameState::GameState()
{


    this->noLegalMoveOnLastTurn = false;
    this->legalMovePresent = false;
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

  
GameState::GameState(const GameState &src){
  this->noLegalMoveOnLastTurn = src.noLegalMoveOnLastTurn;
  this->gameOver = src.gameOver; 
  this->legalMovesCalulated = src.legalMovesCalulated;
  this->legalMovePresent = src.legalMovePresent;
  
  this->currentPlayer = src.currentPlayer;
  this->turnNumber = src.turnNumber;
  
  this->numBlackTiles = src.numBlackTiles;
  this->numWhiteTiles = src.numWhiteTiles;
  
  this->numBlackLegalMoves = src.numBlackLegalMoves;
  this->numWhiteLegalMoves = src.numWhiteLegalMoves;
  
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      this->board[i][j] = src.board[i][j];
      this->legalMoves[i][j] = src.legalMoves[i][j];
      this->moveLines[i][j] = src.moveLines[i][j];
    }
  }

}


void GameState::findLegalMove(unsigned int startingX, unsigned int startingY, unsigned int player,
                                         unsigned int otherPlayer)
{

    // For all 8 cardinal directions
    for (unsigned int direction = 0; direction < 8; direction++)
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
    if(this->legalMovesCalulated) return;
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
                findLegalMove(x, y, this->currentPlayer, otherPlayer);
            }
        }
    }
  this->legalMovesCalulated = true;

  this->legalMovePresent = false;

  if(this->currentPlayer == BLACK){
    if(this->numBlackLegalMoves > 0){
      this->legalMovePresent = true;
    }
  }else{
    if(this->numWhiteLegalMoves > 0){
       this->legalMovePresent = true;
    }
  }

  if(this->noLegalMoveOnLastTurn && !this->legalMovePresent){
    this->gameOver = true;
    this->calculateWinner();
  }
  if(!this->legalMovePresent){
    this->noLegalMoveOnLastTurn = true;
  }else{
    this->noLegalMoveOnLastTurn = false;
  }
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

void GameState::playMove(unsigned int startingX, unsigned int startingY)
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
  
    // For each direction, check if a valid line exists
    for (unsigned int direction = 0; direction < 8; direction++)
    {

        // Get our particular line
        Line line = moveLines.lines[direction];

        if (line.valid && line.assignedTurnNumber == this->turnNumber)
        {
            
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
    

    this->turnNumber++;
    this->legalMovesCalulated = false;
    this->switchPlayers();
}

void GameState::pass(){
    this->turnNumber++;
    this->legalMovesCalulated = false;
    this->switchPlayers();
}


// Uniformly sample the legal action space

void GameState::playRandomMove()
{

    if(!this->legalMovesCalulated){
      this->calculateLegalMoves();
    }
    Coordinate potentialCoordinates[64];
    int idx = 0;
    bool legalMovePresent = false;
    // Collect all valid lines
    for (unsigned int x = 0; x < 8; x++)
    {
        for (unsigned int y = 0; y < 8; y++)
        {
            // Find at least one line on this tile
            if (this->legalMoves[x][y] == LEGAL)
            {
                legalMovePresent = true;
                // Save it
                potentialCoordinates[idx].occupied = true;
                potentialCoordinates[idx].x = x;
                potentialCoordinates[idx].y = y;

                idx++;

                break;
            }
        }
    }

    if(legalMovePresent){
      // We now have idx number of lines to choose from
      
      std::uniform_int_distribution<> distrib(0, idx-1);
      int choice = distrib(rng);
      
      // std::cout << choice << std::endl;
      unsigned int randomX = potentialCoordinates[choice].x;
      unsigned int randomY = potentialCoordinates[choice].y;
      
      this->playMove(randomX, randomY);
    }else{
      this->pass();
    }
}  


std::vector<std::pair<unsigned int, unsigned int>> GameState::getLegalMoves(){

  if(!this->legalMovesCalulated){
    this->calculateLegalMoves();
  }

  std::vector<std::pair<unsigned int, unsigned int>> legalMoves;

  for(unsigned int i = 0; i < 8; i++){
    for(unsigned int j = 0; j < 8; j++){
      if(this->legalMoves[i][j] == LEGAL){
        legalMoves.push_back(std::pair<unsigned int, unsigned int>(i, j));
      }
    }
  }

  return legalMoves;

}  


ComparableGameState GameState::getComparableGameState(){
  uint64_t whiteVec = 0;
  uint64_t blackVec = 0;
  
  for(unsigned int i = 0; i < 8; i++){
    for(unsigned int j = 0; j < 8; j++){
        if(this->board[i][j] == BLACK){
          blackVec = blackVec | 1ULL << (i*8 + j);
        }else if(this->board[i][j] == WHITE){
          whiteVec = whiteVec | 1ULL << (i*8 + j);
        }
    }
  }

  return ComparableGameState(blackVec, whiteVec, this->currentPlayer);

}



std::vector<float> GameState::getContiguousGameState() const {
  std::vector<float> contiguousGameState(64, 0);
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      int boardValue = 0;
      if(this->currentPlayer == BLACK){
        if(this->board[i][j] == BLACK){
          boardValue = 1;
        }else if(this->board[i][j] == WHITE){
          boardValue = -1;
        }else{
          boardValue = 0;
        }
        
      }else{
        if(this->board[i][j] == WHITE){
          boardValue = 1;
        }else if(this->board[i][j] == BLACK){
          boardValue = -1;
        }else{
          boardValue = 0;
        }
        
      }

      contiguousGameState[i*8 + j] = boardValue;
    }
  }

  return contiguousGameState;

}


// ComparableGameState::ComparableGameState(uint64_t whiteVec, uint64_t blackVec, int currentPlayer){
//   this->whiteVec = whiteVec;
//   this->blackVec = blackVec;
//   this->currentPlayer = currentPlayer;
// }


bool ComparableGameState::operator==(const ComparableGameState &other){
  if(this->whiteVec == other.whiteVec && this->blackVec == other.blackVec && this->currentPlayer == other.currentPlayer){
    return true;
  }
  return false;
}

bool ComparableGameState::operator<(const ComparableGameState &other){
  if(this->blackVec < other.blackVec){
    return true;
  }else if(this->blackVec > other.blackVec){
    return false;
  }
  
  if(this->whiteVec < other.whiteVec){
    return true;
  }else if(this->whiteVec > other.whiteVec){
    return false;
  }

  if(this->currentPlayer < other.currentPlayer){
    return true;
  }
  return false;
}


bool ComparableGameState::operator>(const ComparableGameState &other){

  if(this->blackVec > other.blackVec){
    return true;
  }else if(this->blackVec < other.blackVec){
    return false;
  }
  
  if(this->whiteVec > other.whiteVec){
    return true;
  }else if(this->whiteVec < other.whiteVec){
    return false;
  }

  if(this->currentPlayer > other.currentPlayer){
    return true;
  }
  return false;
}



std::ostream& operator<<(std::ostream &strm, const Othello::GameState &gameState){
  strm << "Turn number: " << gameState.turnNumber << "\n"
       << "Current player: " << gameState.currentPlayer << "\n"
       << "Board: " << "\n";

      

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      if(gameState.board[x][y] == Othello::BLACK){
        strm << "B  ";
      }else if(gameState.board[x][y] == Othello::WHITE){
        strm << "W  ";
      }else if(gameState.legalMoves[x][y] == Othello::LEGAL){
        strm << "X  ";
      }else{
        strm << ".  ";
      }
    }
    strm << "\n";
  }

  strm << std::endl;
  strm << "No legal move on last turn: " << gameState.noLegalMoveOnLastTurn << "\n" 
       << "Game Over: " << gameState.gameOver << "\n";

  return strm;
}


std::ostream& operator<<(std::ostream &strm, const Othello::ComparableGameState &comparableGameState){

  strm << "(" << comparableGameState.blackVec << ", " << comparableGameState.whiteVec << ", " << comparableGameState.currentPlayer << ")" << std::endl;

  return strm;
}


}
