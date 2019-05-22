// CITS3002 Project 2019
// Names:            Kieren Underwood,   Daniel Maitland
// Student Numbers:  21315543            21986102
// Compiles on Linux

#include "game.h"

//Rolls our dice--should be different each time
int * roll_dice() {
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

bool check_doubles(int dice[]) {
    //two of the same numbers
    return(dice[0] == dice[1]);
}

bool check_contains(int * dice, int n) {
    //at least one dice is n
    return(dice[0] == n || dice[1] == n);
}

//Main method, which simply calls other check methods
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
            //Client just loses a life for this, should not ever be called, but just in case
            fprintf(stderr, "Invalid choice\n");
            return false;
    }
}
