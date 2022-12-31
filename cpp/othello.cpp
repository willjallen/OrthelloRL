// Build command cl /D_USRDLL /D_WINDLL test.cpp /MT /link /DLL /OUT:testDLL.dll

// If i were to **hypothetically** rewrite this, I would have multiple 64xint arrays and do the legal moves and flipping tiles all in one go

#include <stdlib.h>

// Linux fix
#ifdef __linux__
#define __declspec(v)
#endif


extern "C" {


    const unsigned int DEFAULT_BOARD[8][8] = {
                                                    {0,0,0,0,0,0,0,0},
                                                    {0,0,0,0,0,0,0,0}, 
                                                    {0,0,0,0,0,0,0,0}, 
                                                    {0,0,0,1,2,0,0,0}, 
                                                    {0,0,0,2,1,0,0,0}, 
                                                    {0,0,0,0,0,0,0,0}, 
                                                    {0,0,0,0,0,0,0,0}, 
                                                    {0,0,0,0,0,0,0,0,}
                                                };

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
            {0, 1}, // N = (x+0, y+1)
            {1, 1}, // NE = (x+1, y+1)
            {1, 0}, // E = (x+1, y+0)
            {1, -1}, // SE = (x+1, y-1)
            {0, -1}, // S = (x+0, y-1)
            {-1, -1}, // SW = (x-1, y-1)
            {-1, 0}, // W = (x-1, y+0)
            {-1, 1}, // NW = (x-1, y+1)
        };

    /**
     * @brief Checks whether given coordinates are within the board
     * 
     * @param x 
     * @param y 
     * @return true 
     * @return false 
     */
    bool inBounds(int x, int y){
        if(0 <= x && x < 8 && 0 <= y && y < 8 ){
            return true;
        }else{
            return false;
        }
    } 

    unsigned int getOtherPlayer(unsigned int player){
        return player == 1 ? 2 : 1;
    }



    /**
     * @brief A structure containing a single possible line of play for a given coordinate
     * 
     */
    struct Line {
        unsigned int assignedTurnNumber;
        bool valid;
        unsigned int length;
    };

    /**
     * @brief A structure containing all possible lines of play for a given coordinate
     * 
     */
    struct MoveLines {
        Line lines[8];
    };
    

    /**
     * @brief A structure containing all relevant game information for a particular state
     * 
     */
    // __declspec(dllexport)    
    struct GameState {

        unsigned int currentPlayer;

        unsigned int turnNumber;

        int numBlackTiles;
        int numWhiteTiles;

        unsigned int numBlackLegalMoves;
        unsigned int numWhiteLegalMoves;

        unsigned int board[8][8];
        unsigned int legalMoves[8][8];

        MoveLines moveLines[8][8];

    };
    

    __declspec(dllexport)
    void switchPlayers(GameState* gameState){
        gameState->currentPlayer = getOtherPlayer(gameState->currentPlayer);
    }

    __declspec(dllexport)
    void init(GameState* gameState) {

        gameState->currentPlayer = 1;
        gameState->turnNumber = 0;

        gameState->numBlackLegalMoves = 2;
        gameState->numWhiteLegalMoves = 2;

        gameState->numBlackTiles = 0;
        gameState->numWhiteTiles = 0;

        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                gameState->board[x][y] = DEFAULT_BOARD[x][y];
            }
        }

        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                
                gameState->legalMoves[x][y] = 0;
                
                for(int l = 0; l < 8; l++){
                    Line* currLine = &gameState->moveLines[x][y].lines[l];
                    currLine->valid = false;
                    currLine->length = 0;
                    currLine->assignedTurnNumber = 0;
                }
            }
        }
    }

    __declspec(dllexport)
    void findLegalMove(GameState* gameState, int startingX, int startingY, unsigned int player, unsigned int otherPlayer){

        
        // For all 8 cardinal directions
        for(int direction = 0; direction < 8; direction++){

            int dx = DIRECTION_COMPONENTS[direction][0];
            int dy = DIRECTION_COMPONENTS[direction][1];

            // Check if the first adjacent cell is in bounds, and if it is whether it is the other player
            if(inBounds(startingX + dx, startingY + dy)){
                if(gameState->board[startingX + dx][startingY + dy] != otherPlayer) continue;
            }else{
                continue;
            }

            // Start at the adjacent cell
            int x = startingX + dx;
            int y = startingY + dy;
            

            // Track distance traveled
            unsigned int distance = 1;


            // Continue until we reach the end of the board
            while(inBounds(x + dx, y + dy)){


                // In the case that a line is already flanked on both sides by a chip (BWWB), we can not place a B chip like (BWWBB)
                if(gameState->board[x][y] == player) goto beach;

                // Found legal move
                if(gameState->board[x][y] == EMPTY || gameState->board[x][y] == LEGAL){

                    if(gameState->board[x][y] != LEGAL){
                        if(player == 1){
                            gameState->numBlackLegalMoves++;
                        }else{
                            gameState->numWhiteLegalMoves++;
                        }
                    }
                    
                    gameState->legalMoves[x][y] = LEGAL;

                    // Get Line at coordinate and complement direction (we are moving towards the legal move, and if the legal move is played we flip tiles from the reverse direction)
                    Line* line = &gameState->moveLines[x][y].lines[DIRECTIONS_COMPLEMENT[direction]];
                    line->valid = true;
                    line->length = distance;
                    line->assignedTurnNumber = gameState->turnNumber;

                    goto beach;
                }

                // Keep searching
                if(gameState->board[x][y] == otherPlayer){
                    x += dx;
                    y += dy;
                    distance++;
                }
            }

            // If we reached the end of the board, and the cell is empty, set it to legal
            if(gameState->board[x][y] == EMPTY || gameState->board[x][y] == LEGAL){
                
                if(gameState->board[x][y] != LEGAL){
                    if(player == 1){
                        gameState->numBlackLegalMoves++;
                    }else{
                        gameState->numWhiteLegalMoves++;
                    }
                }
                
                gameState->legalMoves[x][y] = LEGAL;
                
                // Get Line at coordinate and complement direction (we are moving towards the legal move, and if the legal move is played we flip tiles from the reverse direction)
                Line* line = &gameState->moveLines[x][y].lines[DIRECTIONS_COMPLEMENT[direction]];
                line->valid = true;
                line->length = distance;
                line->assignedTurnNumber = gameState->turnNumber;
            }
        
            beach: continue;

        }

    }

    /**
     * @brief Calculates and returns and 8x8 array of legal positions   
     * 
     * @param board 8x8 board where 0 = empty, 1 = player 1, 2 = player 2
     * @param player 1 = player 1, 2 = player 2
     * @return int** 
     */
    __declspec(dllexport)
    void calculateLegalMoves(GameState* gameState){
        unsigned int otherPlayer = getOtherPlayer(gameState->currentPlayer);
        
        // Clear previous legal moves
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                if(gameState->legalMoves[x][y] == LEGAL) gameState->legalMoves[x][y] = EMPTY;
            }
        }

        gameState->numBlackLegalMoves = 0;
        gameState->numWhiteLegalMoves = 0;

        gameState->numBlackTiles = 0;
        gameState->numWhiteTiles = 0;

        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){

                if(gameState->board[x][y] == EMPTY) continue;

                if(gameState->board[x][y] == BLACK) gameState->numBlackTiles +=1;
                if(gameState->board[x][y] == WHITE) gameState->numWhiteTiles +=1;

                if(gameState->board[x][y] == gameState->currentPlayer){
                    // Check the 8 cardinal directions, do not wrap around the board
                    findLegalMove(gameState, x, y, gameState->currentPlayer, otherPlayer);
                }

            }
        }
    }

   


    __declspec(dllexport)
    void playMove(GameState* gameState, int startingX, int startingY){
       

       
        if(gameState->currentPlayer == 1){
            gameState->numBlackTiles++;
        }else{
            gameState->numWhiteTiles++;
        }

        // Get all lines at the coordinate
        MoveLines moveLines = gameState->moveLines[startingX][startingY];

        // For each direction, check if a valid line exists
        for(int direction = 0; direction < 8; direction++){
            
            // Get our particular line
            Line line = moveLines.lines[direction];

            if(line.valid && line.assignedTurnNumber == gameState->turnNumber){

                // Flip the tiles on the line

                int x = startingX;
                int y = startingY;

                int dx = DIRECTION_COMPONENTS[direction][0];
                int dy = DIRECTION_COMPONENTS[direction][1];

                for(int i = 0; i < line.length; i++){
                    gameState->board[x][y] = gameState->currentPlayer;

                    if(gameState->currentPlayer == 1){
                        gameState->numBlackTiles++;
                        gameState->numWhiteTiles--;
                    }else{
                        gameState->numWhiteTiles++;
                        gameState->numBlackTiles--;
                    }

                    if(gameState->numBlackTiles < 0){
                        gameState->numBlackTiles = 0;
                    }

                    if(gameState->numWhiteTiles < 0){
                        gameState->numWhiteTiles = 0;
                    }

                
                    x += dx;
                    y += dy;
                }
            }
        }

        gameState->turnNumber++;

        switchPlayers(gameState);
        
    }


    struct Coordinate {
        bool occupied = false;
        unsigned int x;
        unsigned int y;
    };

    __declspec(dllexport)
    void playRandomMove(GameState* gameState){
        Coordinate potentialCoordinates[64];
        int idx = 0;
        // Collect all valid lines
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                // Find at least one line on this tile
                if(gameState->legalMoves[x][y] == LEGAL){
                    
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

        playMove(gameState, randomX, randomY);

    }

  // 


}
