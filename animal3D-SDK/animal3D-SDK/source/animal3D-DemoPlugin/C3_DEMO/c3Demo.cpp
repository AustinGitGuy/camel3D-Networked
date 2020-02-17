#include "C3_DEMO/c3DemoState.h"
#include "c3MoveObjectEvent.h"
#include "c3ColorChangeEvent.h"
#include "c3StretchObjectEvent.h"


#include <GL/glew.h>

//Prints all connected cilents and their IP addresses
void PrintClientList(const ProfileList* clientProfiles)
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

void PrintBoardToConsole(c3_DemoState* demoState)
{
	int i, j;
	char temp[10][10];
	if (demoState->isTTT)//if game is TTT
	{
		for (i = 0; i < GS_TICTACTOE_BOARD_HEIGHT; i++)//Rows
		{
			strcpy(temp[i], "");
			for (j = 0; j < GS_TICTACTOE_BOARD_WIDTH; j++)//content of rows
			{
				gs_tictactoe_space_state ttt = gs_tictactoe_getSpaceState(demoState->tttGame, i, j);
				if (ttt == gs_tictactoe_space_x)//If space is occupied by an x
				{
					strcat(temp[i], "|X");
				}
				else if(ttt == gs_tictactoe_space_o)//If space is occupied by an o
				{
					strcat(temp[i], "|O");
				}
				else if (ttt == gs_tictactoe_space_open)//If space is occupied by nothing
				{
					strcat(temp[i], "| ");
				}
				else {
					strcat(temp[i], "|null");
					demoState->chatLog[demoState->chatIter] = "ERROR! TicTacToe returned null";
					demoState->chatIter++;
				}
			}
			strcat(temp[i], "|");
			demoState->chatLog[demoState->chatIter] = (std::string)temp[i];
			demoState->chatIter++;
		}
	}
	else//should write local coordinates for battlehsip to see where you've fired etc.****This might need to be addressed later ************
	{
		for (i = 0; i < GS_CHECKERS_BOARD_HEIGHT; i++)//Rows
		{
			strcpy(temp[i], "");
			for (j = 0; j < GS_CHECKERS_BOARD_WIDTH; j++)//content of rows
			{
				gs_checkers_space_state check = gs_checkers_getSpaceState(demoState->checkersGame, i, j);
				if (check == gs_checkers_space_white)//If space is occupied by an x
				{
					strcat(temp[i], "|w");
				}
				else if (check == gs_checkers_space_black)//If space is occupied by an o
				{
					strcat(temp[i], "|b");
				}
				else if (check == gs_checkers_space_open)//If space is occupied by nothing
				{
					strcat(temp[i], "| ");
				}
				else if(check == gs_checkers_space_invalid){
					strcat(temp[i], "|-");
				}
			}

			strcat(temp[i], "|");
			demoState->chatLog[demoState->chatIter] = (std::string)temp[i];
			demoState->chatIter++;
		}
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

void DisplayAllCommands() 
{
	DisplayCommandsChat();
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

void SendToClient(RakNet::RakPeerInterface* peer, const ProfileList* clientProfiles, GameMove msg, int client = -1) {
	if (client == -1) {
		for (int i = 0; i < clientProfiles->iter; i++) {
			peer->Send((char*)&msg, sizeof(GameMove), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[i].address, false);

		}
	}
	else {
		peer->Send((char*)&msg, sizeof(GameMove), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[client].address, false);
	}
}

void c3demoUpdate(c3_DemoState const* demoState){
	//Game loop goes here

}

void c3demoInput(c3_DemoState* demoState){
	//Grab keybaord input and send appropriate command based on them
	
	char input[127];
	strcpy(input, demoState->str);
	if(input[0] != '\n'){
		//Check if a command is not typed
		if(input[0] != '/'){
			char tmp[MAX_CHARACTERS + 20];
			MsgStruct send;

			if(demoState->isServer){
				strcpy(send.senderName, "Server");
			}
			else {
				strcpy(send.senderName, demoState->clientProfiles.profiles[0].name); //Appends Cilent Username to Beginning of Message
			}

			strcpy(tmp, " (Public): ");
			strcat(tmp, input);//Appends a notification that Message is public

			send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;

			strcpy(send.msg, tmp);

			if(demoState->isServer){
				SendToClient(demoState->peer, &demoState->clientProfiles, send);
				demoState->chatLog[demoState->chatIter] = std::string(send.senderName) + std::string(send.msg);
				demoState->chatIter++;
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
				if(input[6] == 'a' || input[6] == 'A'){
					DisplayAllCommands();
				}
				else{
					DisplayCommandsChat();
				}
			}
			else if (input[1] == 'w' || input[1] == 'W')//Private message
			{
				char* trash;

				trash = strtok(input, " ");
				trash = strtok(NULL, " ");

				MsgStruct send;
				char tmp[127];

				if (demoState->isServer) {
					strcpy(send.senderName, "Server: ");//Appends Server to Beginning of Message
				}
				else {
					strcpy(send.senderName, demoState->clientProfiles.profiles[0].name); //Appends Client Username to Beginning of Message
				}

				strcpy(tmp, " (Whisper): ");//Appends a notification that Message is private


				send.id = (RakNet::MessageID)ID_GAME_MESSAGE_PRIVATE;
				strcpy(send.receiveName, trash);//Setting who is receiving the message

				trash = strtok(NULL, " ");
				strcpy(send.msg, strcat(tmp, trash));

				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

			}
			else if (input[1] == 'c' || input[1] == 'C')//Displays list of connected clients
			{
				if (demoState->isServer)//If server run command
				{
					PrintClientList(&demoState->clientProfiles);
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
				strcpy(send.senderName, demoState->clientProfiles.profiles[0].name);

				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

				//handlePackets.detach();
			}
			else//If no command is recognized display message and display list of commands
			{
				printf("Invalid Command. Please use one of the below commands.\n");
				DisplayAllCommands();
			}
		}
	}
	
}

void c3demoNetworkingRecieve(c3_DemoState* demoState) {
	//Copy for loop to here

	bool running = true;

	RakNet::Packet* packet;

	for (packet = demoState->peer->Receive(); packet; demoState->peer->DeallocatePacket(packet), packet = demoState->peer->Receive()) {
		switch(packet->data[0]){
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			demoState->chatLog[demoState->chatIter] = "Another client has disconnected.";
			demoState->chatIter++; 
			//When a client has disconnected display message saying they have left
			MsgStruct send;
			send.id = (RakNet::MessageID)ID_NAME_LEAVE;
			strcpy(send.msg, demoState->clientProfiles.profiles[0].name);

			//Cast to a char* to send the struct as a packet
			demoState->peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			demoState->chatLog[demoState->chatIter] = "Another client has lost the connection.";
			demoState->chatIter++; 
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			demoState->chatLog[demoState->chatIter] = "Another client has connected.";
			demoState->chatIter++; 
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED: {
			demoState->chatLog[demoState->chatIter] = "Our connection request has been accepted.";
			demoState->chatIter++;
			// Use a struct to send the id of the game
			MsgStruct send;
			send.id = (RakNet::MessageID)ID_NAME_JOIN;
			strcpy(send.msg, demoState->clientProfiles.profiles[0].name);

			//Cast to a char* to send the struct as a packet
			demoState->peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

			demoState->profile.address = packet->systemAddress;
			demoState->profile.isHost = true;
			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
			demoState->chatLog[demoState->chatIter] = "A connection is incoming.";
			demoState->chatIter++; 
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			demoState->chatLog[demoState->chatIter] = "The server is full.";
			demoState->chatIter++; 
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (demoState->isServer) {
				demoState->chatLog[demoState->chatIter] = "A client has disconnected.";
				demoState->chatIter++;
			}
			else {
				demoState->chatLog[demoState->chatIter] = "We have been disconnected.";
				demoState->chatIter++;
			}	

		case ID_CONNECTION_LOST:
			if (demoState->isServer) {
				demoState->chatLog[demoState->chatIter] = "A client lost the connection.";
				demoState->chatIter++;
			}
			else {
				demoState->chatLog[demoState->chatIter] = "Connection lost.";
				demoState->chatIter++;
			}

			break;
		case ID_GAME_MESSAGE_1: {

			//Cast it back to a struct to be read
			MsgStruct* read = (MsgStruct*)packet->data;

			demoState->chatLog[demoState->chatIter] = (std::string)read->senderName + (std::string)read->msg;
			demoState->chatIter++;

			//Message recieved
			if(demoState->isServer) SendToClient(demoState->peer, &demoState->clientProfiles, *read);
			break;
		}

		case ID_NAME_JOIN: {

			//If we are getting a name then store it for later.
			//Since this is the server we can still use the profileList struct
			MsgStruct* read = (MsgStruct*)packet->data;
			strcpy(demoState->clientProfiles.profiles[demoState->clientProfiles.iter].name, read->msg);
			demoState->clientProfiles.profiles[demoState->clientProfiles.iter].address = packet->systemAddress;

			if(strcmp(read->msg, demoState->user) == 0){
				MsgStruct send;
				send.id = (RakNet::MessageID)ID_INVITE;
				strcpy(send.msg, "ServerInvite");

				SendToClient(demoState->peer, &demoState->clientProfiles, send, demoState->clientProfiles.iter);
			}

			demoState->chatLog[demoState->chatIter] = "Client connected with name " + (std::string)demoState->clientProfiles.profiles[demoState->clientProfiles.iter].name;
			demoState->chatIter++;
			demoState->clientProfiles.iter++;
			break;
		}
		case ID_NAME_LEAVE: {

			//If we are getting a name then store it for later.
			//Since this is the server we can still use the profileList struct
			MsgStruct* read = (MsgStruct*)packet->data;


			demoState->chatLog[demoState->chatIter] = (std::string)read->senderName + " has disconnected";
			demoState->chatIter++;
			demoState->clientProfiles.iter--;
			break;
		}
		case MOVE_EVENT_ID: {

			EventStruct* read = (EventStruct*)packet->data;
			MoveObjectEvent event(read->float1, read->float2, read->float3);
			demoState->c3EventManager.PushEvent(&event);
			break;
		}
		case COLOR_EVENT_ID: {

			EventStruct* read = (EventStruct*)packet->data;
			ColorChangeEvent event(read->float1, read->float2, read->float3);
			demoState->c3EventManager.PushEvent(&event);
			break;
		}
		case SCALE_EVENT_ID: {

			EventStruct* read = (EventStruct*)packet->data;
			StretchObjectEvent event(read->float1, read->float2, read->float3);
			demoState->c3EventManager.PushEvent(&event);
			break;
		}
		case ID_GAME_MESSAGE_PRIVATE: {
			//Cast it back to a struct to be read
			MsgStruct* read = (MsgStruct*)packet->data;

			int i;
			for (i = 0; i < demoState->clientProfiles.iter; i++)
			{
				if (strcmp(read->receiveName, demoState->clientProfiles.profiles[i].name) == 0)//compares Name in str to list of names in clientProfiles
				{
					break;
				}

			}

			if (i > demoState->clientProfiles.iter)
			{
				strcpy(read->msg, "Could not find user.\n");

				demoState->peer->Send((char*)&read, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				break;
			}
			else
			{
				demoState->chatLog[demoState->chatIter] = (std::string)read->senderName + " " + read->receiveName + " " + (std::string)read->msg;
				demoState->chatIter++;

				//Message recieved
				SendToClient(demoState->peer, &demoState->clientProfiles, *read, i);
				break;
			}
		}
		case ID_GAME_MOVE: {
			
			break;
		}
		case ID_INVITE:
			
			break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
		
	}
	
}

void c3demoRender(c3_DemoState const* demoState){

	//Clear the screen

	glClear(GL_COLOR_BUFFER_BIT);
	if(demoState->inGame){
		////Draw the chatroom if we are in
		//a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Chat: %s", demoState->str);

		////Draw the chatlog
		//for(int i = demoState->chatIter; i >= 0; i--){
		//	int val = demoState->chatIter - i + 1;
		//	a3textDraw(demoState->text, -1, -.95 + (.05) * (val), -1, 1, 1, 1, 1, demoState->chatLog[i].c_str());
		//}
	}
	else if(demoState->lobbyStage == 0){
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "C to connect, H to host, Q to quit: %s", demoState->str);
	}
	else if(demoState->lobbyStage == 1){
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: ");
	}
	else if(demoState->lobbyStage == 2){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: %s", demoState->str);
		}
	}
	else if(demoState->lobbyStage == 4){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter your user name: %s", demoState->str);
		}
	}
}

void c3demoNetworkingLobby(c3_DemoState* demoState){

	glClear(GL_COLOR_BUFFER_BIT);
	if(demoState->lobbyStage == -1){
		printf("Lobby called when it shouldn't have!\n");
	}
	else if(demoState->lobbyStage == 0){
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "C to connect, H to host, Q to quit: %s", demoState->str);
		return;
	}
	else if(demoState->lobbyStage == 1){
		if((demoState->str[0] == 'c') || (demoState->str[0] == 'C')){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: ");
			RakNet::SocketDescriptor sd;
			demoState->peer->Startup(1, &sd, 1);
			demoState->isServer = false;
			demoState->lobbyStage++;
		}
		else if ((demoState->str[0] == 'q') || (demoState->str[0] == 'Q')) {
			//Kill the program
			demoState->exitFlag = 1;
			return;
		}
		else {
			// We need to let the server accept incoming connections from the clients
			demoState->peer->SetMaximumIncomingConnections(MAXCLIENTS);

			demoState->chatLog[demoState->chatIter] = "Starting the server";
			demoState->chatIter++;
			demoState->inGame = true;
			demoState->lobbyStage = -1;
			RakNet::SocketDescriptor sd(60000, 0);
			demoState->peer->Startup(MAXCLIENTS, &sd, 1);
			demoState->isServer = true;
		}
	}
	else if(demoState->lobbyStage == 2){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: %s", demoState->str);
		}
		return;
	}
	else if(demoState->lobbyStage == 3){
		if(!demoState->isServer){
			//If client: get the ip and the username, then start the client
			if(demoState->index == 0){
				strcpy(demoState->ip, "127.0.0.1");
			}
			else {
				strcpy(demoState->ip, demoState->str);
			}
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter your user name: ");
			demoState->lobbyStage++;
		}
	}
	else if(demoState->lobbyStage == 4){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter your user name: %s", demoState->str);
		}
		return;
	}
	else if(demoState->lobbyStage == 5){
		if(!demoState->isServer){
			if(demoState->index == 0){
				strcpy(demoState->str, "Blank");
			}
			strcpy(demoState->clientProfiles.profiles[0].name, demoState->str);

			demoState->chatLog[demoState->chatIter] = "Starting the client";
			demoState->chatIter++;
			demoState->peer->Connect(demoState->ip, 60000, 0, 0);
			demoState->inGame = true;
			demoState->lobbyStage = -1;
		}
	}
}

void c3demoInputLab3(c3_DemoState* demoState, a3i32 asciiKey)
{

	if(!demoState->isServer)
	{
		if(!demoState->inConsole)
		{
			EventStruct send;
			
			send.timeStamp = RakNet::GetTime();

			//Positional Movement
			//Movement in X axis
			if (asciiKey == 119) //lowercase w
			{
				//Move object positive x
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;

				send.float1 = 1;
				send.float2 = 0;
				send.float3 = 0;

				printf("Sending the positive x movement packet\n");

				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

			}
			else if (asciiKey == 115) //lowercase s
			{
				//move object negative x
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;

				send.float1 = -1;
				send.float2 = 0;
				send.float3 = 0;

				printf("Sending the negative x movement packet\n");

				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			//Movement in Z axis
			if (asciiKey == 97)// lowercase a
			{
				//move object negative z
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;

				send.float1 = 0;
				send.float2 = 0;
				send.float3 = -1;

				printf("Sending the negative z movement packet\n");

				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			else if (asciiKey == 100) //lowercase d
			{
				//move object positive z
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;
				
				send.float1 = 0;
				send.float2 = 0;
				send.float3 = 1;

				printf("Sending the positive z movement packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			//Movement in Y axis
			if (asciiKey == 113)// lowercase q
			{
				//move object positive y
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;
				
				send.float1 = 0;
				send.float2 = 1;
				send.float3 = 0;

				printf("Sending the positive y movement packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			else if (asciiKey == 101) //lowercase e
			{
				//move object negative y
				send.id = (RakNet::MessageID)MOVE_EVENT_ID;

				send.float1 = 0;
				send.float2 = -1;
				send.float3 = 0;

				printf("Sending the negative y movement packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}

			//Color Changes
			if (asciiKey == 114)//lowercase r
			{
				//Add to red value
				send.id = (RakNet::MessageID)COLOR_EVENT_ID;

				send.float1 = 1;
				send.float2 = 0;
				send.float3 = 0;

				printf("Sending the color change red packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			if (asciiKey == 103)//lowercase g
			{
				//add to green value
				send.id = (RakNet::MessageID)COLOR_EVENT_ID;

				send.float1 = 0;
				send.float2 = 1;
				send.float3 = 0;

				printf("Sending the color change green packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			if (asciiKey == 98)//lowercase b
			{
				//add to blue value
				send.id = (RakNet::MessageID)COLOR_EVENT_ID;

				send.float1 = 0;
				send.float2 = 0;
				send.float3 = 1;

				printf("Sending the color change blue packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}

			//Stretch obj
			if (asciiKey == 98)//lowercase b
			{
				//Stretch height
				send.id = (RakNet::MessageID)SCALE_EVENT_ID;

				send.float1 = 0;
				send.float2 = 1;
				send.float3 = 0;

				printf("Sending the height scale packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			else if (asciiKey == 109)//lowercase m
			{
				//stretch width
				send.id = (RakNet::MessageID)SCALE_EVENT_ID;

				send.float1 = 1;
				send.float2 = 0;
				send.float3 = 0;

				printf("Sending the width scale packet\n");
				demoState->peer->Send((char*)& send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
			else if (asciiKey == 110)//lowercase m
			{
				//stretch depth
				send.id = (RakNet::MessageID)SCALE_EVENT_ID;

				send.float1 = 0;
				send.float2 = 0;
				send.float3 = 1;

				printf("Sending the depth scale packet\n");
				demoState->peer->Send((char*)&send, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
			}
		}
	}
	
}
