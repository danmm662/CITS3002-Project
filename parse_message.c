//#include "headerfile.h" (replace with header file)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum flag{INIT, EVEN, ODD, DOUB, CON}; //Probably put this into header file

//Determines what type of message is received, checks the user id,
//returns the message type and int choice if it is the contains choice

int parse_message(char *s){
	
	//First check if it is the INIT message from player attempting to connect
	if(strcmp(s, "INIT") == 0){
		//label message with enum CONNECT, let the main program know
		return INIT;
	}
	else {  //This assumes that message recieved is a move
		// need to first check if the first 3 characters of received message is a valid user id
		char *buf;
		buf = calloc(sizeof(s), sizeof(char));
		int id;
		buf = strtok(s, ",");

		if(isnumber(buf) != 0){
			//Tell client that they must provide their 3 digit id at start of their message
		}

		id = atoi(buf);

		//Check if id provided is a valid id or not, tell client if it isn't
		/*if(!isValidId(id)){
			tell client their id is invalid
		}*/
		
		buf = strtok(NULL, ",");

		if(strcmp(buf, "MOV") != 0){
			//tell client that message is invalid, must have MOV after id
		}

		buf = strtok(NULL, ",");

		if(strcmp(buf, "EVEN") == 0){
			//let server know client with id x made EVEN move
			return EVEN;
		}
		else if(strcmp(buf, "ODD") != 0){
			//let server know client with id x made ODD move
			return ODD;
		}
		else if(strcmp(buf, "DOUB") != 0){
			//let server know client with id x made DOUB move
			return DOUB;
		}
		else if(strcmp(buf, "CON") != 0){
			buf = strtok(NULL, ",");
			int num = atoi(buf);
			
			if(num < 1 || num > 6){
				//let client know they picked an invalid dice number
			}

			//let server know client with id x made CON move, choosing num
		}
		else {
			//let client know they made an invalid move
		}
	}
}