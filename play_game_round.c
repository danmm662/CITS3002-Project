//
//  Created by Kieren Underwood on 2/5/19.
//



#include "game.h"

//Replaced repeated code with for loop
void init_game_data(void) {
    
    struct playerInfo *pArray = mmap( NULL,        
                                      MAX_PLAYERS * sizeof(playerInfo), //Number of players * size of the struct
                                      PROT_READ | PROT_WRITE,      //Read and write access to memory
                                      MAP_SHARED | MAP_ANONYMOUS,  //Shared so all processes can access
                                      -1,
                                      0 );

    for(int i = 0; i < MAX_PLAYERS; i++) {
        pArray[i].playerID = (i + 100);
        pArray[i].numLives = MAX_LIVES;
        pArray[i].taken = false;
        pArray[i].eliminated = false;
    }
}

void playGame(void) {

    //int playersLeft = MAX_PLAYERS; Commented out because compiler is annoying
    bool victorFound = false;
    int elimsSent = 0;

    while(!victorFound) {
    
        pid_t child_pid, wpid;
        int status = 0;

        for(int i = 0; i < MAX_PLAYERS; i++) {
            
            //WE ARE forking() HERE AGAIN. SO WE NEED TO SET UP SOME SHARED MEMORY, 
            //SUCH THAT WE CAN RETURN WHETHER OR NOT THEY WON OR LOST THE ROUND...
            if((child_pid = fork()) == 0) {
                int player = pArray[i].playerID;
                int client_no = pArray[i].client_fd;
                //Should our play round return a bool here?

                //THIS PLAY ROUND HAS TO WAIT FOR THE CLIENT TO SEND A MESSAGE FIRST
                //AND THEN PROCESS THE ROUND... SO MAYBE HAVE A FUNCTION THAT JUST SITS THERE AND WAITS FOR A CLIENT 
                //TO CHOOSE AN OPTION
                playRound(player, client_no);
                exit(EXIT_SUCCESS);
            }
        }

        while((wpid = wait(&status)) > 0 );

        //Check to see whether or not the players won the last round
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if(!pArray[i].won_last_round) {
                pArray[i].numLives--;
            }

            //Then check if they should be eliminated
            if(pArray[i].numLives < 1) {
                send_message(pArray[i].client_fd, ELIM);
                elimsSent++;
            }
        }
        
        //Check if there is a victor and send victory message if there is one
        if(elimsSent >= 3) {
            victorFound = true;
            for(int i = 0; i < MAX_PLAYERS; i++) {
                if(!pArray[i].eliminated) {
                    send_message(pArray[i].client_fd, VICT);
                }
            }
        }

    }
}

void playRound(int player, int client_no) {
    
    //HERE WE MUST WAIT FOR THE INCOMING SIGNAL FROM THE CLIENT 

    //THEN WE PROCESS IT
    
    //int * diceRoll = roll_dice(); Commented out because compiler is annoying

    //DO stuff

}

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
            fprintf(stderr, "Invalid choice\n");
            exit(EXIT_FAILURE);
    }
}


void generateNewPlayer(int client_fd, int currPlayers) {
    
    pArray[currPlayers].client_fd = client_fd;
    pArray[currPlayers].taken = true;
    
    /*
    switch(currPlayers) {
        case 0:
            player1.client_fd = client_fd;
            player1.taken = true;
            break;
        case 1:
            player2.client_fd = client_fd;
            player2.taken = true;
            break;
        case 2:
            player3.client_fd = client_fd;
            player3.taken = true;
            break;
        case 3:
            player4.client_fd = client_fd;
            player4.taken = true;
            break;
        default :
            fprintf(stderr, "Too many players have been accepted\n");
    }*/
}
