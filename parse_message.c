#include "game.h" 

//Determines what type of message is received, checks the user id,
//returns the message type and int choice if it is the contains choice

struct messageProperties parse_message(char *s){
	
	//Initialise the properties structure, sets default values for it
	struct messageProperties properties;
	properties.id = -1;
	properties.flag = ERR;
	properties.conChoice = 0;

	char *buf1;
	buf1 = calloc(BUFFER_SIZE, sizeof(char));

	if(strcmp(s, "INIT") == 0){
		properties.flag = INIT;
	}
	else {  
		// need to first check if the first 3 characters of received message is a valid user id
		
		int id;
		buf1 = strtok(s, ",");

		id = atoi(buf1);

		//Check if id provided is a valid id or not, tell client if it isn't
		/*if(!isValidId(id)){
			tell client their id is invalid
		}*/
		
		properties.id = id;

		buf1 = strtok(NULL, ",");

		if(strcmp(buf1, "MOV") != 0){
			//tell client that message is invalid, must have MOV after id
		}

		buf1 = strtok(NULL, ",");

		if(strcmp(buf1, "EVEN") == 0){		//Can't use switches with strings, maybe another way to check what move the client used?
			properties.flag = EVEN;
		}
		else if(strcmp(buf1, "ODD") != 0){
			properties.flag = ODD;
		}
		else if(strcmp(buf1, "DOUB") != 0){
			properties.flag = DOUB;
		}
		else if(strcmp(buf1, "CON") != 0){
			properties.flag = CON;
			
			buf1 = strtok(NULL, ",");
			int num = atoi(buf1);
			
			if(num < 1 || num > 6){
				//let client know they picked an invalid dice number
			}
			properties.conChoice = num;			
		}
	}	
	//free(buf1);
	return properties;
}