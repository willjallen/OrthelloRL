// Build command cl /D_USRDLL /D_WINDLL test.cpp /MT /link /DLL /OUT:testDLL.dll

// If i were to **hypothetically** rewrite this, I would have multiple 64xint arrays and do the legal moves and flipping
// tiles all in one go


#ifndef OTHELLO_H
#define OTHELLO_H 


// #include <stdlib>
#include <math.h>
#include <random>
#include <ostream>
#include <sstream>

namespace Othello {
const unsigned int DEFAULT_BOARD[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 1, 2, 0, 0, 0},
                                              {0, 0, 0, 2, 1, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0,
                                              }};

const unsigned int EMPTY = 0;
const unsigned int BLACK = 1;
const unsigned int WHITE = 2;

const unsigned int LEGAL = 8;

const int NORTH = 0;
const int NORTHEAST = 1;
const int EAST = 2;
const int SOUTHEAST = 3;
const int SOUTH = 4;
const int SOUTHWEST = 5;
const int WEST = 6;
const int NORTHWEST = 7;

const int DIRECTIONS[8] = {NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST};
const int DIRECTIONS_COMPLEMENT[8] = {SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, EAST, SOUTHEAST};

const int DIRECTION_COMPONENTS[8][2] = {
  {0, 1},   // N = (x+0, y+1)
  {1, 1},   // NE = (x+1, y+1)
  {1, 0},   // E = (x+1, y+0)
  {1, -1},  // SE = (x+1, y-1)
  {0, -1},  // S = (x+0, y-1)
  {-1, -1}, // SW = (x-1, y-1)
  {-1, 0},  // W = (x-1, y+0)
  {-1, 1},  // NW = (x-1, y+1)
};

  /**
 * @brief Checks whether given coordinates are within the board
 *
 * @param x
 * @param y
 * @return true
 * @return false
 */

struct Coordinate
{
    bool occupied = false;
    unsigned int x;
    unsigned int y;

    Coordinate(bool occupied, unsigned int x, unsigned int y) : occupied(occupied), x(x), y(y) {}
    Coordinate() {}
};

/**
 * @brief A structure containing a single possible line of play for a given coordinate
 *
 */
struct Line
{
    unsigned int assignedTurnNumber;
    bool valid;
    unsigned int length;
};

/**
 * @brief A structure containing all possible lines of play for a given coordinate
 *
 */
struct MoveLines
{
    Line lines[8];
};

/**
 * @brief A structure containing all relevant game information for a particular state
 *
 */
// struct GameState
// {
//     bool noLegalMoveOnLastTurn;
//     bool gameOver;
//       
//     bool legalMovesCalulated;
//
//     unsigned int currentPlayer;
//     unsigned int winner;
//     unsigned int turnNumber;
//
//     int numBlackTiles;
//     int numWhiteTiles;
//
//     unsigned int numBlackLegalMoves;
//     unsigned int numWhiteLegalMoves;
//
//     unsigned int board[8][8];
//     unsigned int legalMoves[8][8];
//
//     MoveLines moveLines[8][8];
//
// };



class ComparableGameState
{

  public:
    ComparableGameState(uint64_t blackVec, uint64_t whiteVec, int currentPlayer) : blackVec(blackVec), whiteVec(whiteVec), currentPlayer(currentPlayer) {};
    ComparableGameState() {};

    bool operator==(const ComparableGameState &other);
    bool operator<(const ComparableGameState &other);
    bool operator>(const ComparableGameState &other);
    
    uint64_t blackVec;
    uint64_t whiteVec;
    int currentPlayer;

};

  class GameState 
  {
    public:
      GameState();
      GameState(const GameState &GameState);
      
      void switchPlayers();
      void findLegalMove(unsigned int startingX, unsigned int startingY, unsigned int player, unsigned int otherPlayer);
      void calculateLegalMoves();
      bool shouldPass();
      void calculateWinner();
      void playMove(unsigned int startingX, unsigned int startingY);
      void playRandomMove();
      
      void pass();

      // std::stringstream printBoard() const;

      std::vector<std::pair<unsigned int, unsigned int>> getLegalMoves();
      ComparableGameState getComparableGameState();

      
      GameState& operator=(const GameState &src);

      bool noLegalMoveOnLastTurn;
      bool gameOver;

      bool legalMovesCalulated;

      unsigned int currentPlayer;
      unsigned int winner;
      unsigned int turnNumber;

      int numBlackTiles;
      int numWhiteTiles;

      unsigned int numBlackLegalMoves;
      unsigned int numWhiteLegalMoves;

      unsigned int board[8][8];
      unsigned int legalMoves[8][8];

      MoveLines moveLines[8][8];
    
      
    private:

  };


std::ostream& operator<<(std::ostream &strm, const Othello::GameState &gameState);
std::ostream& operator<<(std::ostream &strm, const Othello::ComparableGameState &comparableGameState);

}




#endif




