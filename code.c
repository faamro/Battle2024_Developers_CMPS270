#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #define SIZE 10
    #define NUM_SHIPS 4
    struct ship { // just a struct for ship; name , size , hits, and placed to check if ship is on the grid 
        char name[20];   
        int size;       
        int hits;       
        int placed;     
    };
    typedef struct ship Ship;

    void initializeGrid(char grid[SIZE][SIZE]) { // empty array 
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j] = '~';
            }
        }
    }


    void displayGrid(char grid[SIZE][SIZE]) { // print the grid as shown bl pdf 
        printf("  A B C D E F G H I J\n");
        for (int i = 0; i < SIZE; i++) {
            printf("%d ", i + 1);
            for (int j = 0; j < SIZE; j++) {
                printf("%c ", grid[i][j]);
            }
            printf("\n");
        }
    }

    void getPlayerNames(char player1[], char player2[]) { // ask for names 
        printf("Enter name for Player 1: ");
        scanf("%s", player1);  

        printf("Enter name for Player 2: ");
        scanf("%s", player2);  
    }

    int selectFirstPlayer() {
        return rand() % 2; // Randomly return 0 or 1 

    }
void clearScreen() {
    printf("Press Enter to continue...\n");
    getchar(); // Waits for Enter
    getchar(); // Second getchar() to account for any remaining newline
    system("cls || clear"); // Clears screen on Windows (cls) or Unix-based (clear)
}



    void placeShip(char grid[SIZE][SIZE], Ship ship) {
        int row, col;
        char direction;
        int placed = 0;  // to check if the ship was successfully placed

        printf("Placing ship: %s (Size: %d)\n", ship.name, ship.size);

        
        while (!placed) {
            printf("Enter the starting coordinate (row col) and direction (h for horizontal, v for vertical): ");
            scanf("%d %d %c", &row, &col, &direction);
            row--; col--;  // we start 0 0 cord, but game 1 1 

            int valid = 1;  

            // Checking the direction 
            if (direction == 'h') {
                if (col + ship.size > SIZE) {
                    valid = 0;  // outside the 10 by 10 grid
                } else {
                    for (int i = 0; i < ship.size; i++) {
                        if (grid[row][col + i] != '~') {
                            valid = 0;  // checking if another ship placed here 
                            break;
                        }
                    }
                }
            } else if (direction == 'v') {
                if (row + ship.size > SIZE) {
                    valid = 0;  // outside the 10 by 10 grid
                } else {
                    for (int i = 0; i < ship.size; i++) {
                        if (grid[row + i][col] != '~') {
                            valid = 0;  // checking if another ship placed here 
                            break;
                        }
                    }
                }
            } else {
                valid = 0;  // not v or h
            }

            
            if (valid) {
                if (direction == 'h') {
                    for (int i = 0; i < ship.size; i++) {
                        grid[row][col + i] = ship.name[0];  // Place horizontally c, b ,d , or s ( carrier, battleship, desroyer , or submarine )
                    }
                } else {
                    for (int i = 0; i < ship.size; i++) {
                        grid[row + i][col] = ship.name[0];  // Place vertically c, b ,d , or s ( carrier, battleship, desroyer , or submarine )
                    }
                }
                placed = 1;  // to exit the loop
            } else {
                printf("Invalid placement. Try again.\n"); // an error 
            }
        }
    }

    void fireAtOpponent(char opponentGrid[SIZE][SIZE], char trackingGrid[SIZE][SIZE], Ship opponentShips[NUM_SHIPS], int hardMode) {
    int row, col;
    printf("Enter coordinates to fire at (row col): ");
    scanf("%d %d", &row, &col);
    row--; col--; // Convert to 0-based index

    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        printf("Invalid coordinates. Turn lost.\n");
        return;
    }

    if (opponentGrid[row][col] == '~') {
        printf("Miss!\n");
        if (!hardMode) {
            trackingGrid[row][col] = 'o'; // Mark miss in tracking grid for easy mode
        }
    } else if (opponentGrid[row][col] == 'X') {
        printf("Already hit!\n");
    } else {
        printf("Hit!\n");
        trackingGrid[row][col] = '*';
        opponentGrid[row][col] = 'X';

        // Increment hits for the ship that was hit
        for (int i = 0; i < NUM_SHIPS; i++) {
            if (opponentShips[i].name[0] == opponentGrid[row][col]) {
                opponentShips[i].hits++;
                break;
            }
        }
    }
}


    int allShipsSunk(char grid[SIZE][SIZE]) {
        for (int i = 0; i < SIZE; i++) {  
            for (int j = 0; j < SIZE; j++) { 
                if (grid[i][j] != '~' && grid[i][j] != 'X') {
                    return 0;  // checks all spots till it finds c,, b ,d , or s if it didnt it returns 0 (there are ships left)
                }
            }
        }
        return 1;  //  all cells contain either water ~ or  X return 1 ( no ships left)
    }

    // get top left cord
    void smokeScreen(char grid[SIZE][SIZE], int row, int col) {
        for (int i = 0; i < 2 ; i++) {
            for (int j = 0; j < 2 ; j++) {
                grid[row + i][col + j] = 'S'; // S for Smoke Screen
            }
        }
        clearScreen();
    }

    void artilleryAttack(char opponentGrid[SIZE][SIZE], char trackingGrid[SIZE][SIZE], int row, int col, int hardMode) {
        int hit = 0;  

        for (int i = 0; i < 2 ; i++) {
            for (int j = 0; j < 2 ; j++) {
                if (opponentGrid[row + i][col + j] == '~') {
                    if (hardMode == 0) {
                        trackingGrid[row + i][col + j] = 'o';  // Mark as miss
                    }
                } else if (opponentGrid[row + i][col + j] != 'X') {
                    trackingGrid[row + i][col + j] = '*'; 
                    opponentGrid[row + i][col + j] = 'X'; 
                    hit = 1; 
                }
            }
        }

        if (hit) {
            printf("Hit!\n");
        } else {
            printf("Miss!\n");
        }
    }


    void torpedoAttack(char opponentGrid[SIZE][SIZE], char trackingGrid[SIZE][SIZE], int isRow, int index, int hardMode) {
        int hit = 0;  

        if (isRow) {
            
            for (int j = 0; j < SIZE; j++) {
                if (opponentGrid[index][j] == '~') {
                    if (hardMode == 0) {
                        trackingGrid[index][j] = 'o';  //  in Easy Mode
                    }
                } else if (opponentGrid[index][j] != 'X') {
                    trackingGrid[index][j] = '*';  
                    opponentGrid[index][j] = 'X';  
                    hit = 1; 
                }
            }
        } else {
            // Column attack
            for (int i = 0; i < SIZE; i++) {
                if (opponentGrid[i][index] == '~') {
                    if (hardMode == 0) {
                        trackingGrid[i][index] = 'o';  //Easy Mode
                    }
                } else if (opponentGrid[i][index] != 'X') {
                    trackingGrid[i][index] = '*';  
                    opponentGrid[i][index] = 'X';  
                    hit = 1;  
                }
            }
        }

        if (hit) {
            printf("Hit!\n");
        } else {
            printf("Miss!\n");
        }
    }
    void radarSweep(char grid[SIZE][SIZE], int row, int col) {
        int foundShip = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                if (row + i < SIZE  && grid[row + i][col + j] != '~' && grid[row + i][col + j] != 'S') {
                    foundShip = 1;
                }
            }
        }
        if (foundShip) {
            printf("Enemy ships found!\n");
        } else {
            printf("No enemy ships found.\n");
        }
    }


    int checkIfShipSunk(char grid[SIZE][SIZE], Ship *ship) {
    int hitCount = 0; // Count of how many parts of the ship are hit

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == ship->name[0] && grid[i][j] == 'X') {
                hitCount++; // Count it as a hit
            }
        }
    }

    // If all parts are hit, mark the ship as sunk
    if (hitCount == ship->size) {
        ship->placed = 0; // Set placed to 0 to mark as sunk
        return 1;         // Ship is sunk
    }

    return 0; // Ship is not sunk yet
}

int countSunkShips(Ship ships[NUM_SHIPS]) {
    int sunkCount = 0; // Number of sunk ships

    for (int i = 0; i < NUM_SHIPS; i++) {
        if (ships[i].placed == 0) { // If ship is sunk
            sunkCount++;           // Increase the count
        }
    }
    return sunkCount; // Return total sunk ships
}


    Ship createShip(int index) {
        Ship ships[NUM_SHIPS] = {
            {"Carrier", 5, 0, 1},       // (a) Carrier - 5 cells
            {"Battleship", 4, 0, 1},    // (b) Battleship - 4 cells
            {"Destroyer", 3, 0, 1},     // (c) Destroyer - 3 cells
            {"Submarine", 2, 0, 1}      // (d) Submarine - 2 cells
        };
        return ships[index];
    }

    void updateSpecialAbilities(Ship opponentShips[NUM_SHIPS], int *artilleryReady, int *torpedoReady) {
    int sunkCount = countSunkShips(opponentShips);

    if (sunkCount >= 1) *artilleryReady = 1;  // Unlock artillery after sinking 1 ship
    if (sunkCount >= 3) *torpedoReady = 1;    // Unlock torpedo after sinking 3 ships
}

void updateSunkShips(Ship ships[NUM_SHIPS]) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        if (ships[i].hits == ships[i].size && ships[i].placed == 1) {
            printf("%s has been sunk!\n", ships[i].name);
            ships[i].placed = 0; // Mark the ship as sunk
        }
    }
}





    void gameplay(char grid1[SIZE][SIZE], char grid2[SIZE][SIZE], char trackingGrid1[SIZE][SIZE], char trackingGrid2[SIZE][SIZE], char player1[], char player2[], int hardMode, Ship player1Ships[NUM_SHIPS], Ship player2Ships[NUM_SHIPS]) {
        int radarUses1 = 3, radarUses2 = 3;
        int smokeUses1 = 0, smokeUses2 = 0;
        int artilleryReady1 = 0, artilleryReady2 = 0;
        int torpedoReady1 = 0, torpedoReady2 = 0;
        int turn = selectFirstPlayer();

        printf("%s will start first!\n", (turn % 2 == 0) ? player1 : player2);

        while (1) {
            char command[20];
            int row, col;
            char (*opponentGrid)[SIZE];
            char (*trackingGrid)[SIZE];
            int *radarUses, *smokeUses, *artilleryReady, *torpedoReady;
            char *currentPlayer;
            Ship *opponentShips;

            if (turn % 2 == 0) {
                opponentGrid = grid2;
                trackingGrid = trackingGrid1;
                radarUses = &radarUses1;
                smokeUses = &smokeUses1;
                artilleryReady = &artilleryReady1;
                torpedoReady = &torpedoReady1;
                currentPlayer = player1;
                opponentShips = player2Ships;
            } else {
                opponentGrid = grid1;
                trackingGrid = trackingGrid2;
                radarUses = &radarUses2;
                smokeUses = &smokeUses2;
                artilleryReady = &artilleryReady2;
                torpedoReady = &torpedoReady2;
                currentPlayer = player2;
                opponentShips = player1Ships;
            }
            updateSpecialAbilities(opponentShips, artilleryReady, torpedoReady);  // Update special abilities based on sunk ships
            printf("Debug - Artillery Ready: %d, Torpedo Ready: %d\n", *artilleryReady, *torpedoReady); // Debug
            
            printf("\n%s's turn:\n", currentPlayer);
            displayGrid(trackingGrid);
            printf("Available Commands: Fire, Radar, Smoke, Artillery, Torpedo\n");
            printf("Enter command: ");
            
          fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character
        
        printf("You entered command: %s\n", command);

            if (strcmp(command, "fire") == 0) {
                printf("Firing at opponent...\n");
                fireAtOpponent(opponentGrid, trackingGrid, opponentShips, hardMode);
                printf("Fire function executed.\n");
                printf("Press Enter to continue to the next turn.\n");
                getchar(); getchar();  

                printf("Tracking Grid after firing:\n");
                displayGrid(trackingGrid);

                printf("Opponent Grid after firing (for debug purposes):\n");
                displayGrid(opponentGrid);
                 updateSunkShips(opponentShips);

            } else if (strcmp(command, "Radar") == 0) {
                if (*radarUses > 0) {
                    printf("Enter top-left coordinate for Radar (row col): ");
                    scanf("%d %d", &row, &col);
                    row--; col--;
                    radarSweep(opponentGrid, row, col);
                    (*radarUses)--;
                } else {
                    printf("No Radar uses left!\n");
                }

            } else if (strcmp(command, "Smoke") == 0) {
                int allowedSmokeScreens = countSunkShips((turn % 2 == 0) ? player2Ships : player1Ships);
                if (*smokeUses < allowedSmokeScreens) {
                    printf("Enter top-left coordinate for Smoke Screen (row col): ");
                    scanf("%d %d", &row, &col);
                    row--; col--;
                    smokeScreen(trackingGrid, row, col);
                    (*smokeUses)++;
                    clearScreen();
                } else {
                    printf("No Smoke Screens left! Move lost.\n");
                }

            } else if (strcmp(command, "Artillery") == 0 && *artilleryReady) {
                printf("Enter top-left coordinate for Artillery (row col): ");
                scanf("%d %d", &row, &col);
                row--; col--;
                artilleryAttack(opponentGrid, trackingGrid, row, col, hardMode);
                *artilleryReady = 0;

            } else if (strcmp(command, "Torpedo") == 0 && *torpedoReady) {
                printf("Enter row (1-10) or column (A-J) for Torpedo: ");
                char target[5];
                scanf("%s", target);
                if (target[0] >= '1' && target[0] <= '9') {
                    row = target[0] - '1';
                    torpedoAttack(opponentGrid, trackingGrid, 1, row, hardMode);
                } else {
                    col = target[0] - 'A';
                    torpedoAttack(opponentGrid, trackingGrid, 0, col, hardMode);
                }
                *torpedoReady = 0;

            } else {
                printf("Invalid command or not available.\n");
            }

            turn++;
        }
    }



    int main() {
        char player1[50], player2[50];
        char grid1[SIZE][SIZE], grid2[SIZE][SIZE];
        char trackingGrid1[SIZE][SIZE], trackingGrid2[SIZE][SIZE];
        int hardMode = 0;
        Ship player1Ships[NUM_SHIPS];
        Ship player2Ships[NUM_SHIPS];

        initializeGrid(grid1);
        initializeGrid(grid2);
        initializeGrid(trackingGrid1);
        initializeGrid(trackingGrid2);

        getPlayerNames(player1, player2);

        // Ask for tracking difficulty level
        printf("Select tracking difficulty level (0 for Easy, 1 for Hard): ");
        scanf("%d", &hardMode);

        // Players place their ships
        printf("\n%s, place your ships:\n", player1);
        for (int i = 0; i < NUM_SHIPS; i++) {
            player1Ships[i] = createShip(i);  // Store each ship in player1Ships
            placeShip(grid1, player1Ships[i]);
        }
        clearScreen();

        printf("\n%s, place your ships:\n", player2);
        for (int i = 0; i < NUM_SHIPS; i++) {
            player2Ships[i] = createShip(i);  // Store each ship in player2Ships
            placeShip(grid2, player2Ships[i]);
        }
        clearScreen();

        // Start the game
        gameplay(grid1, grid2, trackingGrid1, trackingGrid2, player1, player2, hardMode, player1Ships, player2Ships);

        return 0;
    }