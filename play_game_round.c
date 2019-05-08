//
//  Created by Kieren Underwood on 2/5/19.
//

#include "game.h"

//typedef enum {INIT, EVEN, ODD, DOUB, CON} flag; 

int * roll_dice() {
    sleep(1);
    int max = 6;
    int* dice = calloc(2, sizeof(int));
    dice[0] = (rand() % max) + 1;
    dice[1] = (rand() % max) + 1;
    return dice;
}

bool check_even(int * dice) {
    //sum of the dice is an even number (but not doubles)
    if(dice[0] == dice[1]) {
        return false;
    }
    int sum = dice[0] + dice[1];
    return(sum % 2 == 0);
}

bool check_odd(int * dice) {
    //sum of the dice is odd number that is > 5
    int sum = dice[0] + dice[1];
    return(sum > 5 && (sum % 2 == 1));
}

bool check_doubles(int * dice) {
    //two of the same numbers
    return(dice[0] == dice[1]);
}

bool check_contains(int * dice, int n) {
    //at least one dice is n
    return(dice[0] == n || dice[1] == n);
}

bool check(int * dice, int flag, int con_choice) {
    switch(flag) {
        case EVEN :
            return check_even(dice);
        case ODD :
            return check_odd(dice);
        case DOUB :
            return check_doubles(dice);
        case CON :
            return check_contains(dice, con_choice);
        default :
            printf("Get fucked");
            exit(EXIT_FAILURE);
    }
}

/*Commented out so the program will compile
void init_game_data(void) {
    int playerInfo[10][2];
    for(int i = 0; i < 10; i++) {
        playerInfo[i][0] = 0;
        playerInfo[i][1] = 0;
    }
}

void clear_game_info(void) {
    for(int i = 0; i < 10; i++) {
        playerInfo[i][0] = 0;
        playerInfo[i][1] = 0;
    }
}*/