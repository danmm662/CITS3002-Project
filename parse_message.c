// CITS3002 Project 2019
// Names:            Kieren Underwood,   Daniel Maitland
// Student Numbers:  21315543            21986102
// Compiles on Linux

#include "game.h" 

/*
* Determines what type of message is received, checks the user id,
* Returns the message type and int choice if it is the contains choice
* If any error is found, or the packet is too long, an ERR flag is sent back
* The default when ERR is found is to have the client lose a life
* If the client_fd does not match the playerID, we define this as cheating, and the player will be kicked
*/
struct messageProperties parse_message(char *s, int client_fd){
	
	//Initialise the properties structure, sets default values for it
	struct messageProperties properties;
	properties.id = -1;
	properties.flag = ERR;
	properties.conChoice = 0; 

	char *buf1;
	buf1 = calloc(BUFFER_SIZE, sizeof(char));

	if (strcmp(s, "INIT") == 0) {
		properties.flag = INIT;
	}
	else {  

		if(strlen(s) > 13) {	//Check if packet is too long
			return properties;
		}

		int id;
		buf1 = strtok(s, ",");
		if(buf1 == NULL) {			//Make sure message contains commas
			return properties;	
		}

		id = atoi(buf1);  //This will cause an error if buf1 is not an int, may need to test for that first

		//Ensures that id matches playerID stored in array, mark player as cheating if not
		if(client_fd != -1) {
			if(pArray[id - 100].client_fd != client_fd) {
				properties.flag = CHEAT;
				return properties;
			}
		}
		
		properties.id = id;

		buf1 = strtok(NULL, ",");
		if(buf1 == NULL) {				//Make sure message contains commas
			return properties;	
		}

		if(strcmp(buf1, "MOV") != 0){ 	//Checks that message contains MOV, returns the struct with the
			return properties;			//ERR flag if it doesn't
		}

		buf1 = strtok(NULL, ",");
		if(buf1 == NULL) {				//Make sure message contains commas
			return properties;	
		}

		//Now we check what guess the client chose
		if(strcmp(buf1, "EVEN") == 0){		
			properties.flag = EVEN;			
		}
		else if(strcmp(buf1, "ODD") == 0){
			properties.flag = ODD;
		}
		else if(strcmp(buf1, "DOUB") == 0){
			properties.flag = DOUB;
		}
		else if(strcmp(buf1, "CON") == 0){

			buf1 = strtok(NULL, ",");
			if(buf1 == NULL) {			 //Make sure message contains commas
				return properties;	
			}
			
			if (!isdigit(buf1[0])) {	 //Makes sure that it is actually a digit before calling atoi()
				return properties;
			}

			int num = atoi(buf1);
			
			if(num < 1 || num > 6) {		//Returns the properties struct with the ERR flag if invalid number
				return properties;
			}
			properties.flag = CON;
			properties.conChoice = num;			
		}
	}	
	return properties;
}