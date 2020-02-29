#include "C3_DEMO/c3DemoState.h"
#include "c3SpawnBoidEvent.h"

#include <GL/glew.h>

void SendToClient(RakNet::RakPeerInterface* peer, const ProfileList* clientProfiles, MsgStruct msg, int client = -1){
	if (client == -1) {
		for (int i = 0; i < clientProfiles->iter; i++) {
			peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[i].address, false);

		}
	}
	else {
		peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[client].address, false);
	}
}

void SendToClient(RakNet::RakPeerInterface* peer, const ProfileList* clientProfiles, EventStruct msg, RakNet::SystemAddress notSend = RakNet::UNASSIGNED_SYSTEM_ADDRESS){
	if(notSend == RakNet::UNASSIGNED_SYSTEM_ADDRESS){
		peer->Send((char*)&msg, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, NULL, true);
	}
	else {
		peer->Send((char*)&msg, sizeof(EventStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, notSend, true);
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

			
			if (input[1] == 'w' || input[1] == 'W')//Private message
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


				//send.id = (RakNet::MessageID)ID_GAME_MESSAGE_PRIVATE;
				strcpy(send.receiveName, trash);//Setting who is receiving the message

				trash = strtok(NULL, " ");
				strcpy(send.msg, strcat(tmp, trash));

				demoState->peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);

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








		/*
		case DEFAULT_EVENT_ID: {
			EventStruct* read = (EventStruct*)packet->data;

			for (int i = 0; i < EVENT_SIZE; i++){
				if (read->events[i].type == MOVE_EVENT){
					MoveObjectEvent* event = new MoveObjectEvent(read->events[i].float1, read->events[i].float2, read->events[i].float3);
					event->sent = false;
					EventManager::GetInstance()->PushEvent(event);
				}
				else if (read->events[i].type == COLORCHANGE_EVENT){
					ColorChangeEvent* event = new ColorChangeEvent(read->events[i].float1, read->events[i].float2, read->events[i].float3);
					event->sent = false;
					EventManager::GetInstance()->PushEvent(event);
				}
				else if (read->events[i].type == STRETCH_EVENT){
					StretchObjectEvent* event = new StretchObjectEvent(read->events[i].float1, read->events[i].float2, read->events[i].float3);
					event->sent = false;
					EventManager::GetInstance()->PushEvent(event);
				}
				else if (read->events[i].type == CLONE_EVENT){
					CloneObjectEvent* event = new CloneObjectEvent();
					event->sent = false;
					EventManager::GetInstance()->PushEvent(event);
				}
			}

			if(demoState->isServer){
				SendToClient(demoState->peer, &demoState->clientProfiles, *read, packet->systemAddress);
			}

			
			break;
		}
		*/
		//case ID_GAME_MESSAGE_PRIVATE: {
		//	//Cast it back to a struct to be read
		//	MsgStruct* read = (MsgStruct*)packet->data;

		//	int i;
		//	for (i = 0; i < demoState->clientProfiles.iter; i++)
		//	{
		//		if (strcmp(read->receiveName, demoState->clientProfiles.profiles[i].name) == 0)//compares Name in str to list of names in clientProfiles
		//		{
		//			break;
		//		}

		//	}

		//	if (i > demoState->clientProfiles.iter)
		//	{
		//		strcpy(read->msg, "Could not find user.\n");

		//		demoState->peer->Send((char*)&read, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		//		break;
		//	}
		//	else
		//	{
		//		demoState->chatLog[demoState->chatIter] = (std::string)read->senderName + " " + read->receiveName + " " + (std::string)read->msg;
		//		demoState->chatIter++;

		//		//Message recieved
		//		SendToClient(demoState->peer, &demoState->clientProfiles, *read, i);
		//		break;
		//	}
		//}
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
		
	}
	
}

void c3demoRender(c3_DemoState* demoState){

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

		/*for(int i = 0; i < demoState->numCubes; i++){
			if(i == 0){
				glColor3f(demoState->red, demoState->blue, demoState->green);
				glBegin(GL_QUADS);
				glVertex2f(demoState->xPos - demoState->xScale, demoState->yPos + demoState->yScale);
				glVertex2f(demoState->xPos - demoState->xScale, demoState->yPos - demoState->yScale);
				glVertex2f(demoState->xPos + demoState->xScale, demoState->yPos - demoState->yScale);
				glVertex2f(demoState->xPos + demoState->xScale, demoState->yPos + demoState->yScale);
				glEnd();
			}
			
			if(i > 0){
				glColor3f(demoState->red, demoState->blue, demoState->green);
				glBegin(GL_QUADS);
				glVertex2f(0 - demoState->xScale, 0 + demoState->yScale);
				glVertex2f(0 - demoState->xScale, 0 - demoState->yScale);
				glVertex2f(0 + demoState->xScale, 0 - demoState->yScale);
				glVertex2f(0 + demoState->xScale, 0 + demoState->yScale);
				glEnd();
			}
		}*/

		demoState->flock.UpdateFlock();
		if(!demoState->isServer) demoState->flock.DrawFlock(demoState->frameWidth, demoState->frameHeight);
	}
	else if(demoState->lobbyStage == 0){
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "C to connect, H to host, Q to quit, L to locally simulate: %s", demoState->str);
	}
	else if(demoState->lobbyStage == 1){
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: ");
	}
	else if(demoState->lobbyStage == 2){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: %s", demoState->str);
		}
		else {
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server mode, 1 for push, 2 for shared, 3 for coupled: %s", demoState->str);
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
		a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "C to connect, H to host, Q to quit, L to locally simulate: %s", demoState->str);
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
		else if((demoState->str[0] == 'h') || (demoState->str[0] == 'H')){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server mode, 1 for push, 2 for shared, 3 for coupled: %s", demoState->str);
			demoState->isServer = true;
			demoState->lobbyStage++;
		}
		else {
			demoState->chatLog[demoState->chatIter] = "Simulating locally";
			demoState->chatIter++;
			demoState->inGame = true;
			demoState->lobbyStage = -1;
			demoState->localSim = true;
		}
	}
	else if(demoState->lobbyStage == 2){
		if(!demoState->isServer){
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server IP or hit enter for 127.0.0.1: %s", demoState->str);
		}
		else {
			a3textDraw(demoState->text, -1, -.95, -1, 1, 1, 1, 1, "Enter server mode, 1 for push, 2 for shared, 3 for coupled: %s", demoState->str);
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
		else {
			if(demoState->str[0] == '1'){
				demoState->type = ServerType::DATA_PUSH;
			}
			if (demoState->str[0] == '2') {
				demoState->type = ServerType::DATA_SHARED;
			}
			if (demoState->str[0] == '3') {
				demoState->type = ServerType::DATA_COUPLED;
			}
			// We need to let the server accept incoming connections from the clients
			demoState->peer->SetMaximumIncomingConnections(MAXCLIENTS);
			demoState->chatLog[demoState->chatIter] = "Starting the server";
			demoState->chatIter++;
			demoState->inGame = true;
			demoState->lobbyStage = -1;
			RakNet::SocketDescriptor sd(60000, 0);
			demoState->peer->Startup(MAXCLIENTS, &sd, 1);
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
			if(asciiKey == 101){
				SpawnBoidEvent* event = new SpawnBoidEvent();
				EventManager::GetInstance()->PushEvent(event);
			}
		}
	}
	
}

//Inital transfer of all boids to other client
void c3ShareBoids(c3_DemoState* demoState) 
{
	NewBoidStruct send;

	Flock demoFlock = demoState->flock;


	for (int i = 0; i < FLOCK_SIZE; i++)
	{
		demoFlock.



		demoState->peer->Send((char*)&send, sizeof(NewBoidStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
	}



}
