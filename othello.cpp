// Build command cl /D_USRDLL /D_WINDLL test.cpp /MT /link /DLL /OUT:testDLL.dll



extern "C" {

    const int EMPTY = 0;
    const int PLAYER_1 = 1;
    const int PLAYER_2 = 2;
    // const int PLAYER_1_CHECKED = 3;
    // const int PLAYER_2_CHECKED = 4;
    const int LEGAL = 8;


    const int NORTH = 0;
    const int NORTHEAST = 1;
    const int EAST = 2;
    const int SOUTHEAST = 3;
    const int SOUTH = 4;
    const int SOUTHWEST = 5;
    const int WEST = 6;
    const int NORTHWEST = 7;

    const int DIRECTIONS[8] = {NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST};

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
     * @param i 
     * @param j 
     * @return true 
     * @return false 
     */
    __declspec(dllexport)
    bool inBounds(int x, int y){
        if(0 <= x && x < 8 && 0 <= y && y < 8 ){
            return true;
        }else{
            return false;
        }
    } 

    __declspec(dllexport)
    void setLegalMove(int board[8][8], int startingX, int startingY, int player, int otherPlayer){
        
        // For all 8 cardinal directions
        for(int direction = 0; direction < 8; direction++){

            int dx = DIRECTION_COMPONENTS[direction][0];
            int dy = DIRECTION_COMPONENTS[direction][1];

            // Check if the first cell is the other player
            if(inBounds(startingX + dx, startingY + dy)){
                if(board[startingX + dx][startingY + dy] != otherPlayer) continue;
            }

            int x = startingX;
            int y = startingY;
            int boardVal = board[x][y];

            // Continue until we reach the end of the board or an empty cell
            while(inBounds(x + dx, y + dy) && boardVal != EMPTY){
                
                x += dx;
                y += dy;
                boardVal = board[x][y];

                // In the case that a line is already flanked on both sides by a chip (BWWB), we can not place a B chip like (BWWBB)
                if(boardVal == player) goto beach;

            }

            // If the last cell we reached is empty, set it to legal
            if(board[x][y] == EMPTY){
                board[x][y] = LEGAL;
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
    void calculateLegalMoves(int board[8][8], int player){

        int otherPlayer = 0;
        if(player == 1){
            otherPlayer = 2;
        }else{
            otherPlayer = 1;
        }

        // Internal to function:
        // 3 = checked empty
        // 4 = checked player 1
        // 5 = checked player 2
        // 6 = legal move
        
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                int boardVal = board[x][y];

                if(boardVal == EMPTY) continue;

                if(boardVal == player){
                    // Check the 8 cardinal directions, do not wrap around the board
                    setLegalMove(board, x, y, player, otherPlayer);
                }

            }
        }

        // return board;
    }




}