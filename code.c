#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 10


typedef struct {
    char name[20];
    int size;
    int positions[5][2];
    int hitCount;
} Ship;


typedef struct {
    char name[20];
    char board[BOARD_SIZE][BOARD_SIZE];
    char tracking[BOARD_SIZE][BOARD_SIZE];
    Ship ships[4];
    int radarCount;  
    int smokeCount;  
    int shipsRemaining;  
    int sunkShips;  
    int difficulty;
    int artilleryUnlocked;
    int torpedoUnlocked;
    int isBot;
} Player;


void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '~';
        }
    }
}


void displayGrid(Player *p) {
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i<9) {
            printf(" %d ", i + 1);
        } else {
            printf("%d ", i + 1);
        }
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (p->board[i][j] == 'X' || (p->board[i][j] == 'O' && p->difficulty == 0)) {
                printf("%c ", p->board[i][j]);
            } else {
                printf("~ ");
            }
        }
        printf("\n");
    }
}


int isValidPosition(int x, int y) {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}


void clearScreen() {
    printf("\e[1;1H\e[2J");
}


void placeShips(Player *p) {
    int shipSizes[] = {5, 4, 3, 2};
    char *shipNames[] = {"Carrier", "Battleship", "Destroyer", "Submarine"};
   
    for (int i = 0; i < 4; i++) {
        int x, y;
        char orientation;
        int placed = 0;

        while (!placed) {
            printf("%s, place your %s (%d cells).\n", p->name, shipNames[i], shipSizes[i]);
            printf("Enter starting coordinates (e.g., B3) and orientation (H or V): ");
            char input[5];
            scanf("%s %c", input, &orientation);

            if (input[2] == '0') {
                x = 9;
            } else {
                x = input[1] - '1';
            }
            y = input[0] - 'A';

            if (!isValidPosition(x, y) || (orientation != 'H' && orientation != 'V')) {
                printf("Invalid input. Try again.\n");
                continue;
            }

            int fits = 1;
            if (orientation == 'H') {
                for (int j = 0; j < shipSizes[i]; j++) {
                    if (y + j >= BOARD_SIZE || p->board[x][y + j] != '~') {
                        fits = 0;
                        break;
                    }
                }
            } else {
                for (int j = 0; j < shipSizes[i]; j++) {
                    if (x + j >= BOARD_SIZE || p->board[x + j][y] != '~') {
                        fits = 0;
                        break;
                    }
                }
            }

            if (fits) {
                p->ships[i] = (Ship){.size = shipSizes[i], .hitCount = 0};
                strcpy(p->ships[i].name, shipNames[i]);

                if (orientation == 'H') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        p->board[x][y + j] = 'S';
                        p->ships[i].positions[j][0] = x;
                        p->ships[i].positions[j][1] = y + j;
                    }
                } else {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        p->board[x + j][y] = 'S';
                        p->ships[i].positions[j][0] = x + j;
                        p->ships[i].positions[j][1] = y;
                    }
                }
                placed = 1;
            } else {
                printf("Invalid position. Try again.\n");
            }
        }
    }
}


void placeShipsBot(Player *p) {
    int shipSizes[] = {5, 4, 3, 2};
    char *shipNames[] = {"Carrier", "Battleship", "Destroyer", "Submarine"};
    
    for (int i = 0; i < 4; i++) {
        int placed = 0;
        while (!placed) {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            char orientation = (rand() % 2 == 0) ? 'H' : 'V';

            int fits = 1;
            if (orientation == 'H') {
                for (int j = 0; j < shipSizes[i]; j++) {
                    if (y + j >= BOARD_SIZE || p->board[x][y + j] != '~') {
                        fits = 0;
                        break;
                    }
                }
            } else {
                for (int j = 0; j < shipSizes[i]; j++) {
                    if (x + j >= BOARD_SIZE || p->board[x + j][y] != '~') {
                        fits = 0;
                        break;
                    }
                }
            }

            if (fits) {
                p->ships[i] = (Ship){.size = shipSizes[i], .hitCount = 0};
                strcpy(p->ships[i].name, shipNames[i]);

                if (orientation == 'H') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        p->board[x][y + j] = 'S';
                        p->ships[i].positions[j][0] = x;
                        p->ships[i].positions[j][1] = y + j;
                    }
                } else {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        p->board[x + j][y] = 'S';
                        p->ships[i].positions[j][0] = x + j;
                        p->ships[i].positions[j][1] = y;
                    }
                }
                placed = 1;
            }
        }
    }
}


void setDifficulty(Player *p, int difficulty) {
    p->difficulty = difficulty;  
    p->radarCount = 3;  
    p->smokeCount = 0;
    p->sunkShips = 0;  
    p->artilleryUnlocked = 0;
    p->torpedoUnlocked = 0;
}


void fire(Player *attacker, Player *defender, int x, int y, int *shipSunk) {
    if (attacker->tracking[x][y] == 'X' || attacker->tracking[x][y] == 'O') {
        printf("You have already targeted that location. You lose your turn.\n");
        return;
    }

    if (!isValidPosition(x, y)) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    if (defender->board[x][y] == 'S' || defender->board[x][y] == 'W') {
        printf("Hit!\n");
        defender->board[x][y] = 'X';
        attacker->tracking[x][y] = 'X';

        for (int i = 0; i < 4; i++) {
            Ship *ship = &defender->ships[i];
            for (int j = 0; j < ship->size; j++) {
                if (ship->positions[j][0] == x && ship->positions[j][1] == y) {
                    ship->hitCount++;
                    break;
                }
            }

            if (ship->hitCount == ship->size) {
                attacker->sunkShips++;
                defender->shipsRemaining--;
                printf("%s sank the %s!\n", attacker->name, ship->name);
                ship->hitCount = ship->size + 1;

                *shipSunk = 1;

                attacker->smokeCount++;
                printf("%s's smoke count increased to %d!\n", attacker->name, attacker->smokeCount);

                attacker->artilleryUnlocked = 1;
                printf("%s's artillery action unlocked for the next turn only!\n", attacker->name);

                if (defender->shipsRemaining == 1) {
                    attacker->torpedoUnlocked = 1;
                    printf("%s's torpedo action unlocked for the next turn!\n", attacker->name);
                }
            }
        }
    } else {
        printf("Miss!\n");
        defender->board[x][y] = 'O';
        attacker->tracking[x][y] = 'O';  
    }
}


void useRadar(Player *p, Player *opponent, int x, int y) {
    if (p->radarCount <= 0) {
        printf("No radar sweeps left! You lose your turn.\n");
        return;
    }

    if (!isValidPosition(x, y)) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    printf("Radar activated at %c%d:\n", 'A' + y, x + 1);
    int found = 0;
    for (int i = x; i < x + 2 && i < BOARD_SIZE; i++) {
        for (int j = y; j < y + 2 && j < BOARD_SIZE; j++) {
            if (opponent->board[i][j] == 'S') {
                found = 1;
            }
        }
    }
    if (found) {
        printf("Enemy ships found!\n");
    } else {
        printf("No enemy ships found.\n");
    }
    p->radarCount--;
}


void useSmoke(Player *p, int x, int y) {
    if (p->smokeCount == 0) {
        printf("No smoke screens left! You lose your turn.\n");
        return;  
    }

    if (!isValidPosition(x, y)) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    printf("Smoke screen deployed at %c%d.\n", 'A' + y, x + 1);
    for (int i = x; i < x + 2 && i < BOARD_SIZE; i++) {
        for (int j = y; j < y + 2 && j < BOARD_SIZE; j++) {
            if (p->board[i][j] == 'S') {
                p->board[i][j] = 'W';
            }
        }
    }
    p->smokeCount--;
    clearScreen();
}


void useArtillery(Player *p, Player *opponent, int x, int y, int *shipSunk) {

    if (!isValidPosition(x, y)) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    printf("Artillery strike at %c%d:\n", 'A' + y, x + 1);

    for (int i = x; i < x + 2 && i < BOARD_SIZE; i++) {
        for (int j = y; j < y + 2 && j < BOARD_SIZE; j++) {
            if (opponent->board[i][j] == 'S' || opponent->board[i][j] == 'W') {
                printf("Hit at %c%d!\n", 'A' + j, i + 1);
                opponent->board[i][j] = 'X';  
                p->tracking[i][j] = 'X';
                for (int k = 0; k < 4; k++) {
                    Ship *ship = &opponent->ships[k];
                    for (int l = 0; l < ship->size; l++) {
                        if (ship->positions[l][0] == i && ship->positions[l][1] == j) {
                            ship->hitCount++;
                            break;
                        }
                    }
                    if (ship->hitCount == ship->size) {
                        p->sunkShips++;
                        opponent->shipsRemaining--;
                        printf("%s sank the %s!\n", p->name, ship->name);
                        ship->hitCount = ship->size + 1;

                        *shipSunk = 1;

                        p->smokeCount++;
                        printf("%s's smoke count increased to %d!\n", p->name, p->smokeCount);

                        p->artilleryUnlocked = 1;
                        printf("%s's artillery action unlocked for the next turn only!\n", p->name);

                        if (opponent->shipsRemaining == 1) {
                            p->torpedoUnlocked = 1;
                            printf("%s's torpedo action unlocked for the next turn only!\n", p->name);
                        }
                    }
                }
            } else if (opponent->board[i][j] == 'X') {
                printf("Old Hit at %c%d\n", 'A' + j, i + 1); 
            } else {
                printf("Miss at %c%d\n", 'A' + j, i + 1);
                opponent->board[i][j] = 'O';
                p->tracking[i][j] = 'O'; 
            }
        }
    }
}


void useTorpedo(Player *p, Player *opponent, int target, char type) {

    if (target<0 || target>9) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    printf("Torpedo attack along %s %d:\n", type == 'R' ? "row" : "column", target + 1);

    if (type == 'R') {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (opponent->board[target][j] == 'S' || opponent->board[target][j] == 'W') {
                printf("Hit at %c%d!\n", 'A' + j, target + 1);
                opponent->board[target][j] = 'X';
                p->tracking[target][j] = 'X';
                for (int k = 0; k < 4; k++) {
                    Ship *ship = &opponent->ships[k];
                    for (int l = 0; l < ship->size; l++) {
                        if (ship->positions[l][0] == target && ship->positions[l][1] == j) {
                            ship->hitCount++;
                            if (ship->hitCount == ship->size) {
                                p->sunkShips++;
                                opponent->shipsRemaining--;
                                printf("%s sank the %s!\n", p->name, ship->name);
                            }
                            break;
                        }
                    }
                }
            } else if (opponent->board[target][j] == 'X') {
                printf("Old Hit at %c%d\n", 'A' + j, target + 1);
            } else {
                printf("Miss at %c%d\n", 'A' + j, target + 1);
                opponent->board[target][j] = 'O';
                p->tracking[target][j] = 'O'; 
            }
        }
    } else {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (opponent->board[i][target] == 'S' || opponent->board[i][target] == 'W') {
                printf("Hit at %c%d!\n", 'A' + target, i + 1);
                opponent->board[i][target] = 'X';
                p->tracking[i][target] = 'X'; 
                for (int k = 0; k < 4; k++) {
                    Ship *ship = &opponent->ships[k];
                    for (int l = 0; l < ship->size; l++) {
                        if (ship->positions[l][0] == i && ship->positions[l][1] == target) {
                            ship->hitCount++;
                            if (ship->hitCount == ship->size) {
                                p->sunkShips++;
                                opponent->shipsRemaining--;
                                printf("%s sank the %s!\n", p->name, ship->name);
                            }
                            break;
                        }
                    }
                }
            } else if (opponent->board[i][target] == 'X') {
                printf("Old Hit at %c%d\n", 'A' + target, i + 1);
            } else {
                printf("Miss at %c%d\n", 'A' + target, i + 1);
                opponent->board[i][target] = 'O';
                p->tracking[i][target] = 'O'; 
            }
        }
    }
}


void botTurn(Player *bot, Player *opponent, int *shipSunk) {
    int targetX = -1, targetY = -1;
    int foundTarget = 0;
    char type;
    int target;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (bot->tracking[i][j] == 'X') {
                if (isValidPosition(i + 1, j) && bot->tracking[i + 1][j] == '~') {
                    targetX = i + 1;
                    targetY = j;
                    foundTarget = 1;
                    type = 'C';
                    target = targetY;
                    break;
                }
                if (isValidPosition(i - 1, j) && bot->tracking[i - 1][j] == '~') {
                    targetX = i - 1;
                    targetY = j;
                    foundTarget = 1;
                    type = 'C';
                    target = targetY;
                    break;
                }
                if (isValidPosition(i, j + 1) && bot->tracking[i][j + 1] == '~') {
                    targetX = i;
                    targetY = j + 1;
                    foundTarget = 1;
                    type = 'R';
                    target = targetX;
                    break;
                }
                if (isValidPosition(i, j - 1) && bot->tracking[i][j - 1] == '~') {
                    targetX = i;
                    targetY = j - 1;
                    foundTarget = 1;
                    type = 'R';
                    target = targetX;
                    break;
                }
            }
        }
        if (foundTarget) {
            break;
        }
    }

    if (bot->torpedoUnlocked) {
        if (!foundTarget) {
            target = rand() % BOARD_SIZE;
            type = (rand() % 2) ? 'R' : 'C';
        }
        printf("Mr_Bot used Torpedo move on %c number %d\n", type, target+1);
        useTorpedo(bot, opponent, target, type);
        return;
    }

    if (bot->artilleryUnlocked) {
        if (!foundTarget) {
            do {
                targetX = rand() % (BOARD_SIZE - 1);
                targetY = rand() % (BOARD_SIZE - 1);
            } while (bot->tracking[targetX][targetY] != '~');
        }
        printf("Mr_Bot used Artillery move on %c%d\n", 'A'+targetY, targetX+1);
        useArtillery(bot, opponent, targetX, targetY, shipSunk);
        return;
    }

    if (!foundTarget) { 
        do {
            targetX = rand() % BOARD_SIZE;
            targetY = rand() % BOARD_SIZE;
        } while (bot->tracking[targetX][targetY] != '~');
    }
    printf("Mr_Bot used Fire move on %c%d\n", 'A'+targetY, targetX+1);
    fire(bot, opponent, targetX, targetY, shipSunk);
}


void gameLoop(Player *p1, Player *p2) {
    int x, y;
    char move[10];

    while (p1->sunkShips < 4 && p2->sunkShips < 4) {
        printf("%s's turn:\n", p1->name);
        displayGrid(p2);

        int shipSunk = 0;
        int turnPassed = 0;
        if (p1->artilleryUnlocked) {
            turnPassed = 1;
        }

        if (!p1->isBot) {
            printf("Enter move (Fire, Radar, Smoke, Artillery, Torpedo): ");
            scanf("%s", move);

            if (strcmp(move, "Fire") == 0) {
                printf("Enter coordinates (e.g., B3): ");
                char input[5];
                scanf("%s", input);
                if (input[2] != '\0') {
                    if (input[1]=='1' && input[2]=='0') {
                        x = 9;
                    } else {
                        x = 20;
                    }
                } else {
                    x = input[1] - '1';
                }
                y = input[0] - 'A';
                fire(p1, p2, x, y, &shipSunk);
            } else if (strcmp(move, "Radar") == 0) {
                printf("Enter top-left coordinates (e.g., B3) for radar: ");
                char input[5];
                scanf("%s", input);
                if (input[2] != '\0') {
                    if (input[1]=='1' && input[2]=='0') {
                        x = 9;
                    } else {
                        x = 20;
                    }
                } else {
                    x = input[1] - '1';
                }
                y = input[0] - 'A';
                useRadar(p1, p2, x, y);
            } else if (strcmp(move, "Smoke") == 0) {
                printf("Enter top-left coordinates (e.g., B3) for smoke: ");
                char input[5];
                scanf("%s", input);
                if (input[2] != '\0') {
                    if (input[1]=='1' && input[2]=='0') {
                        x = 9;
                    } else {
                        x = 20;
                    }
                } else {
                    x = input[1] - '1';
                }
                y = input[0] - 'A';
                useSmoke(p1, x, y);
            } else if (strcmp(move, "Artillery") == 0) {
                if (p1->artilleryUnlocked) {
                    printf("Enter top-left coordinates (e.g., B3) for artillery: ");
                    char input[5];
                    scanf("%s", input);
                    if (input[2] != '\0') {
                        if (input[1]=='1' && input[2]=='0') {
                            x = 9;
                        } else {
                            x = 20;
                        }
                    } else {
                        x = input[1] - '1';
                    }
                    y = input[0] - 'A';
                    useArtillery(p1, p2, x, y, &shipSunk);
                } else {
                    printf("Artillery move is unlocked! You lose your turn.\n");
                }
            } else if (strcmp(move, "Torpedo") == 0) {
                if (p1->torpedoUnlocked) {
                    printf("Enter target (0-9 for row/column) followed by type (R for row, C for column): ");
                    int target;
                    char type;
                    scanf("%d %c", &target, &type);
                
                    if (type == 'R') {
                        useTorpedo(p1, p2, target, 'R');
                    } else if (type == 'C') {
                        useTorpedo(p1, p2, target, 'C');
                    } else {
                        printf("Invalid type. Use 'R' for row or 'C' for column. You lose your turn.\n");
                    }
                } else {
                    printf("Torpedo move is not unlocked! You lose your turn.\n");
                }
            } else {
                printf("Invalid move! You lose your turn.\n");
            }
        } else {
            botTurn(p1,p2,&shipSunk);
        }

        if (!shipSunk && turnPassed) {
            p1->torpedoUnlocked = 0;
            p1->artilleryUnlocked = 0;
        }

        Player *temp = p1;
        p1 = p2;
        p2 = temp;
    }

    printf("''''''''''''''''''''");
    if (p1->shipsRemaining == 0) {
        printf("\n%s wins the game!\n", p2->name);
    } else {
        printf("\n%s wins!\n", p1->name);
    }
    printf("''''''''''''''''''''");
}


int main() {
    srand(time(NULL));
    Player player1, player2;

    char gameType;
    int validInput;
    do {
        printf("Choose Game Type (P for 2-Player Mode, B for 1-Player_vs_Bot Mode): ");
        validInput = scanf("%c", &gameType);
        if (!validInput) {
            while (getchar() != '\n');
        }
        if (gameType != 'P' && gameType != 'B') {
            printf("Invalid input. Try again.\n");
        }
    } while(gameType != 'P' && gameType != 'B');

    player1.isBot = 0;
    if (gameType == 'B') {
        player2.isBot = 1;
    } else {
        player2.isBot = 0;
    }

    initializeBoard(player1.board);
    initializeBoard(player2.board);
    initializeBoard(player1.tracking);
    initializeBoard(player2.tracking);

    int difficulty;
    do {
        printf("Choose Game Difficulty (0 for Easy, 1 for Hard): ");
        validInput = scanf("%d", &difficulty);
        if (!validInput) {
            while (getchar() != '\n');
        }
        if (difficulty!=0 && difficulty!=1) {
            printf("Invalid input. Try again.\n");
        }
    } while (difficulty!=0 && difficulty!=1);

    setDifficulty(&player1, difficulty);
    setDifficulty(&player2, difficulty);

    printf("Enter Player 1 name: ");
    scanf("%s", player1.name);

    if (gameType == 'P') {
        do {
            printf("Enter Player 2 name: ");
            scanf("%s", player2.name);

            if (strcmp(player1.name, player2.name) == 0) {
                printf("Player 2 name cannot be the same as Player 1 name. Please enter again.\n");
            }
        } while (strcmp(player1.name, player2.name) == 0);
    } else {
        strcpy(player2.name, "Mr_Bot");
    }

    player1.shipsRemaining = 4;  
    player2.shipsRemaining = 4;

    clearScreen();
    placeShips(&player1);
    clearScreen();

    if (gameType == 'P') {
        placeShips(&player2);
        clearScreen();
    } else {
        placeShipsBot(&player2);
    }

    int firstPlayer = rand() % 2;
    if (firstPlayer == 0) {
        printf("%s goes first!\n", player1.name);
        gameLoop(&player1, &player2);
    } else {
        printf("%s goes first!\n", player2.name);
        gameLoop(&player2, &player1);
    }

    return 0;
}