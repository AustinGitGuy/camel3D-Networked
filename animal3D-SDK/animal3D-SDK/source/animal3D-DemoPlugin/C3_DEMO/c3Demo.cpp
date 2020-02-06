#include "C3_DEMO/c3DemoState.h"

#include <GL/glew.h>

//Prints all connected cilents and their IP addresses
void PrintClientList(ProfileList* clientProfiles)
{
	printf("\n****CLIENT LIST****");
	printf("\nUsername    IP Address");
	printf("\n------------------------\n");
	for (int i = 0; i < clientProfiles->iter; i++)
	{
		printf("%s      ", clientProfiles->profiles[i].name);
		printf("%s\n", clientProfiles->profiles[i].address.ToString());
	}
}

//Below Fucntion is used to display all available commands for tic-tac-toe
void DisplayCommandsChat()
{
	printf("\nAvailable Commands\n------------------\n");
	printf("/help : Displays available commands\n"
		"/whisper : Send a private message to the defined user (/whisper Username Message)\n"
		"/clients : Displays list of currently connected clients (Host Only)\n"
		"/ place{ x } {y} : Place a tic - tac - toe piece at x, y\n"
		"/quit : Return to chatroom selection.\n"
		"Type your message and press enter to send a public message\n");
}

void DisplayCommandsTTT()
{
	printf("\nAvailable Commands\n------------------\n");
	printf("/help : Displays available commands\n"
		"/whisper : Send a private message to the defined user (/whisper Username Message)\n"
		"/clients : Displays list of currently connected clients (Host Only)\n"
		"/ place{ x } {y} : Place a tic - tac - toe piece at x, y\n"
		"/quit : Return to chatroom selection.\n"
		"Type your message and press enter to send a public message\n");
}


//Below Fucntion is used to display all available commands for battleship
void DisplayCommandsBattleship() {
	printf("\nAvailable Commands\n------------------\n");
	printf("/help : Displays available commands\n"
		"/whisper : Send a private message to the defined user (/whisper Username Message)\n"
		"/clients : Displays list of currently connected clients (Host Only)\n"
		"/quit : Return to chatroom selection.\n"
		"Type your message and press enter to send a public message\n");
}


void DisplayAllCommands() 
{
	bool TTTtrue;
	DisplayCommandsChat();
	
	if (TTTtrue)//Check which game is being played
	{
		DisplayCommandsTTT();
	}
	else
	{
		DisplayCommandsBattleship();
	}
}



void SendToClient(RakNet::RakPeerInterface* peer, const ProfileList* clientProfiles, MsgStruct msg, int client = -1) {
	if (client == -1) {
		for (int i = 0; i < clientProfiles->iter; i++) {
			peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[i].address, false);

		}
	}
	else {
		peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[client].address, false);
	}
}


void c3demoRender(c3_DemoState const* demoState) {

	//Clear the screen

	if(demoState->inGame){
		//Draw the chatroom if we are in
		glClear(GL_COLOR_BUFFER_BIT);
		a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Chat: %s", demoState->str);
	}
}

void c3demoUpdate(c3_DemoState const* demoState) {
	//Game loop goes here
	

	


}

void c3demoInput(c3_DemoState const* demoState){
	//Grab keybaord input and send appriotrate command based on them

	char input[127];
	strcpy(input, demoState->str);
	if (input[0] != '\n') {
		//Check if a command is typed
		if (input[0] != '/') {

			char* nameEnd;
			nameEnd = strchr(input, '\n');
			*nameEnd = ' ';

			char tmp[127];
			MsgStruct send;

			if (demoState->isServer) {
				strcpy(tmp, "Server: ");//Appends Server to Beginning of Message
			}
			else {
				strcpy(send.senderName, demoState->clientProfiles->profiles[0].name);//Appends Cilent Username to Beginning of Message
			}

			strcat(tmp, ": (Public) ");//Appends a notification that Message is public

			send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;
			strcpy(send.msg, strcpy(tmp, input));

			if (demoState->isServer) {
				SendToClient(demoState->peer, demoState->clientProfiles, send);
			}
			else {
				//Cast to a char* to send the struct as a packet
				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
		}
		//Otherwise check command
		else {

			if (input[1] == 'h' || input[1] == 'H')//Displays list of commands
			{
				if (input[6] == 't' || input[6] == 'T')//Displays list of commands
				{
					DisplayCommandsTTT();
				}
				else if (input[6] == 'b' || input[6] == 'B'){
					DisplayCommandsBattleship();
				}
				else if (input[6] == 'a' || input[6] == 'A'){
					DisplayAllCommands();
				}
				else
				{
					DisplayCommandsChat();
				}
			}
			else if (input[1] == 'g' || input[1] == 'G')//Game Move.  Make a move in game
			{
				//Game move
				
			}
			else if (input[1] == 'w' || input[1] == 'W')//Private message
			{
				char* trash;

				char* nameEnd;
				nameEnd = strchr(input, '\n');
				*nameEnd = ' ';

				trash = strtok(input, " ");
				trash = strtok(NULL, " ");

				MsgStruct send;
				char tmp[127];

				if (demoState->isServer) {
					strcpy(tmp, "Server: ");//Appends Server to Beginning of Message
				}
				else {
					strcpy(send.senderName, demoState->clientProfiles->profiles[0].name);//Appends Client Username to Beginning of Message
				}

				strcpy(tmp, ": (Whisper) ");//Appends a notification that Message is private


				send.id = (RakNet::MessageID)ID_GAME_MESSAGE_PRIVATE;
				strcpy(send.receiveName, trash);//Setting who is receiving the message
				//strcpy(send.senderName, clientProfiles.profiles[0].name); //setting who is sending the message
				trash = strtok(NULL, " ");
				strcpy(send.msg, strcat(tmp, trash));

				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

			}
			else if (input[1] == 'c' || input[1] == 'C')//Displays list of connected clients
			{
				if (demoState->isServer)//If server run command
				{
					PrintClientList(demoState->clientProfiles);
				}
				else//If not server deny access
				{
					printf("Access to command denied.\n");
				}
			}
			else if (input[1] == 'q' || input[1] == 'Q')//Leaves current chat room and brings user back to the Lobby
			{
				MsgStruct send;
				send.id = (RakNet::MessageID)ID_NAME_LEAVE;
				strcpy(send.senderName, demoState->clientProfiles->profiles[0].name);

				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

				//handlePackets.detach();
			}
			else//If no command is recognized display message and display list of commands
			{
				printf("Invalid Command. Please use one of the below commands.\n");
				DisplayCommandsTTT();
			}
		}
	}
}

void c3demoNetworkingSend(c3_DemoState const* demoState) {
	//Send packets out here


}

void c3demoNetworkingRecieve(c3_DemoState const* demoState) {
	//Copy for loop to here

	bool running = true;

	RakNet::Packet* packet;

	for (packet = demoState->peer->Receive(); packet; demoState->peer->DeallocatePacket(packet), packet = demoState->peer->Receive()) {
		switch (packet->data[0]) {
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			//When a client has disconnected display message saying they have left
			MsgStruct send;
			send.id = (RakNet::MessageID)ID_NAME_LEAVE;
			strcpy(send.msg, demoState->clientProfiles->profiles[0].name);

			//Cast to a char* to send the struct as a packet
			demoState->peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED: {
			printf("Our connection request has been accepted.\n");

			// Use a struct to send the id of the game
			MsgStruct send;
			send.id = (RakNet::MessageID)ID_NAME_JOIN;
			strcpy(send.msg, demoState->clientProfiles->profiles[0].name);

			//Cast to a char* to send the struct as a packet
			demoState->peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);


			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (demoState->isServer) {
				printf("A client has disconnected.\n");

			}
			else {
				printf("We have been disconnected.\n");
			}

		case ID_CONNECTION_LOST:
			if (demoState->isServer) {
				printf("A client lost the connection.\n");
			}
			else {
				printf("Connection lost.\n");
			}

			break;
		case ID_GAME_MESSAGE_1: {

			//Cast it back to a struct to be read
			MsgStruct* read = (MsgStruct*)packet->data;

			printf("%s ", read->senderName);
			printf("%s\n", read->msg);

			//Message recieved
			SendToClient(demoState->peer, demoState->clientProfiles, *read);
			break;
		}

		case ID_NAME_JOIN: {

			//If we are getting a name then store it for later.
			//Since this is the server we can still use the profileList struct
			MsgStruct* read = (MsgStruct*)packet->data;
			strcpy(demoState->clientProfiles->profiles[demoState->clientProfiles->iter].name, read->msg);
			demoState->clientProfiles->profiles[demoState->clientProfiles->iter].address = packet->systemAddress;

			printf("Client connected with name %s\n", demoState->clientProfiles->profiles[demoState->clientProfiles->iter].name);
			demoState->clientProfiles->iter++;
			break;
		}
		case ID_NAME_LEAVE: {

			//If we are getting a name then store it for later.
			//Since this is the server we can still use the profileList struct
			MsgStruct* read = (MsgStruct*)packet->data;


			printf("%s has disconnected\n", read->senderName);
			demoState->clientProfiles->iter--;
			break;
		}

		case ID_GAME_MESSAGE_PRIVATE: {
			//Cast it back to a struct to be read
			MsgStruct* read = (MsgStruct*)packet->data;

			int i;
			for (i = 0; i < demoState->clientProfiles->iter; i++)
			{
				if (strcmp(read->receiveName, demoState->clientProfiles->profiles[i].name) == 0)//compares Name in str to list of names in clientProfiles
				{
					break;
				}

			}

			if (i > demoState->clientProfiles->iter)
			{
				strcpy(read->msg, "Could not find user.\n");

				demoState->peer->Send((char*)&read, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				break;
			}
			else
			{
				printf("%s ", read->senderName);
				printf("%s", read->receiveName);
				printf("%s\n", read->msg);

				//Message recieved
				SendToClient(demoState->peer, demoState->clientProfiles, *read, i);
				break;
			}
		}
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
	
}

void c3demoNetworkingRecieveNonConst(c3_DemoState* demoState){

	bool running = true;

	RakNet::Packet* packet;

	
	for (packet = demoState->peer->Receive(); packet; demoState->peer->DeallocatePacket(packet), packet = demoState->peer->Receive()) {
		switch (packet->data[0]) {
		case ID_CONNECTION_REQUEST_ACCEPTED: {
			printf("Our connection request has been accepted.\n");

			demoState->profile.address = packet->systemAddress;
			demoState->profile.isHost = true;
			break;
		}
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
}

void c3demoNetworkingLobby(c3_DemoState* demoState) 
{
	//system("CLS");

	glClear(GL_COLOR_BUFFER_BIT);
	printf("Lobby stage: %i\n", demoState->lobbyStage);
	if(demoState->lobbyStage == -1){
		printf("Lobby called when it shouldn't have!\n");
	}
	else if(demoState->lobbyStage == 0) {
		a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "C to connect, H to host, Q to quit: %s", demoState->str);
		return;
	}
	else if(demoState->lobbyStage == 1){
		if((demoState->str[0] == 'c') || (demoState->str[0] == 'C')){
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: ");
			RakNet::SocketDescriptor sd;
			demoState->peer->Startup(1, &sd, 1);
			demoState->isServer = false;
		}
		else if ((demoState->str[0] == 'q') || (demoState->str[0] == 'Q')) {
			//Kill the program
			demoState->exitFlag = 1;
			return;
		}
		else {
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "What Game would you like to play? (B)attleship or (T)ic-Tac-Toe: ");
			RakNet::SocketDescriptor sd(demoState->serverPort, 0);
			demoState->peer->Startup(MAXCLIENTS, &sd, 1);
			demoState->isServer = true;
		}
		demoState->lobbyStage++;
	}
	else if(demoState->lobbyStage == 2){
		if(demoState->isServer){
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "What Game would you like to play? (B)attleship or (T)ic-Tac-Toe: %s", demoState->str);
		}
		else {
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: %s", demoState->str);
		}
		return;
	}
	else if(demoState->lobbyStage == 3){
		if(demoState->isServer){
			if (demoState->str[0] == 'T')//If host chose tic tac toe
			{
				//Start Tic tac toe game
			}
			else
			{
				//Start Battleship
			}

			// We need to let the server accept incoming connections from the clients
			demoState->peer->SetMaximumIncomingConnections(MAXCLIENTS);

			//TODO: Queue this onto the text stack
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Starting the server.");
			demoState->inGame = true;
			demoState->lobbyStage = -1;
		}
		else {
			//If client: get the ip and the username, then start the client
			if(demoState->index == 0){
				strcpy(demoState->ip, "127.0.0.1");
			}
			else {
				strcpy(demoState->ip, demoState->str);
			}
			a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter your user name: ");
			demoState->lobbyStage++;
		}
	}
	else if(demoState->lobbyStage == 4){
		a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter your user name: %s", demoState->str);
		return;
	}
	else if(demoState->lobbyStage == 5){
		if(demoState->index == 0){
			strcpy(demoState->str, "Blank");
		}
		strcpy(demoState->clientProfiles->profiles[0].name, demoState->str);

		char* nameEnd;
		nameEnd = strchr(demoState->clientProfiles->profiles[0].name, '\n');
		*nameEnd = '\0';

		//TODO: Add this to the text stack
		a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Starting the client.");
		demoState->peer->Connect(demoState->ip, demoState->serverPort, 0, 0);
		demoState->inGame = true;
		demoState->lobbyStage = -1;
	}
}