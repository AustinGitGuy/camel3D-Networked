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

void c3demoUpdate(c3_DemoState* demoState){
	//Game loop goes here
	if(demoState->isServer){
		demoState->flock.UpdateFlock(demoState->frameWidth, demoState->frameHeight);
		c3SendBoidToClient(demoState);
	}
	else if(demoState->serverType != DATA_PUSH){
		demoState->flock.UpdateFlock(demoState->frameWidth, demoState->frameHeight);
		c3SendBoidToServer(demoState);
	}
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

			ServerTypeStruct msg;
			msg.id = ID_SEND_SERVERTYPE;
			msg.type = demoState->serverType;

			demoState->peer->Send((char*)&msg, sizeof(ServerTypeStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

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

		case ID_SEND_SERVERTYPE: {
			ServerTypeStruct* read = (ServerTypeStruct*)packet->data;
			demoState->serverType = read->type;
			if(demoState->serverType != DATA_PUSH){
				//Create the local flock
				for(int i = 0; i < 10; i++){
					demoState->flock.addBird(Vector3(20 + i * 50, 100), true);
				}
				for(int i = 0; i < 5; i++){
					demoState->flock.addBird(Vector3(20 + i * 50, 200), true);
				}
			}
			break;
		}
		case ID_SEND_BOID: {
			BoidStruct* read = (BoidStruct*)packet->data;
			if(read->boidId >= demoState->flock.GetPositionIndex()){
				demoState->flock.addBird(read->position, false);
			}
			else {

				if (demoState->serverType == ServerType::DATA_COUPLED && demoState->isServer == false)
				{
					//If the local position is not the same as the server position then set the boid's position to the server position
					if (demoState->flock.getBoidPosition(read->boidId) != read->position)
					{
						demoState->flock.setBoidPosition(read->boidId, read->position);
					}
					
				}
				else
				{
					demoState->flock.setBoidPosition(read->boidId, read->position);
				}

				

			}

			//Send it to the clients if we are the server
			if(demoState->isServer){
				demoState->peer->Send((char*)&send, sizeof(BoidStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
			}
			break;
		}
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

			//Create the local flock
			for(int i = 0; i < 10; i++){
				demoState->flock.addBird(Vector3(20 + i * 50, 100));
			}
			for(int i = 0; i < 5; i++){
				demoState->flock.addBird(Vector3(20 + i * 50, 200));
			}

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
				demoState->serverType = ServerType::DATA_PUSH;
			}
			if (demoState->str[0] == '2') {
				demoState->serverType = ServerType::DATA_SHARED;
			}
			if (demoState->str[0] == '3') {
				demoState->serverType = ServerType::DATA_COUPLED;
			}

			if(demoState->serverType != ServerType::DATA_PUSH){
				//Create the local flock
				for (int i = 0; i < 10; i++) {
					demoState->flock.addBird(Vector3(20 + i * 50, 100), true, i + 15);
				}
				for (int i = 0; i < 5; i++) {
					demoState->flock.addBird(Vector3(20 + i * 50, 200), true, i + 25);
				}
			}
			else {
				//Create the local flock
				for (int i = 0; i < 10; i++) {
					demoState->flock.addBird(Vector3(20 + i * 50, 100));
				}
				for (int i = 0; i < 5; i++) {
					demoState->flock.addBird(Vector3(20 + i * 50, 200));
				}
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

//Inital transfer of all boids to server
void c3SendBoidToServer(c3_DemoState* demoState) 
{
	BoidStruct send;

	Flock demoFlock = demoState->flock;
	send.id = ID_SEND_BOID;

	//Send boids to the server
	for(int i = 0; i < demoFlock.GetPositionIndex(); i++){
		if(demoFlock.checkLocalBoid(i)){
			send.position = demoFlock.getBoidPosition(i);
			send.boidId = i;
			demoState->peer->Send((char*)&send, sizeof(BoidStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->profile.address, false);
		}
	}
}

//Inital transfer of all boids to other clients
void c3SendBoidToClient(c3_DemoState* demoState)
{
	if(demoState->clientProfiles.iter <= 0) return;
	BoidStruct send;

	Flock demoFlock = demoState->flock;
	send.id = ID_SEND_BOID;

	//Send boids to the clients
	for (int i = 0; i < demoFlock.GetPositionIndex(); i++){
		if(demoFlock.checkLocalBoid(i)){
			send.position = demoFlock.getBoidPosition(i);
			send.boidId = i;

			for (int j = 0; j < demoState->clientProfiles.iter; j++)
			{
				demoState->peer->Send((char*)& send, sizeof(BoidStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->clientProfiles.profiles[j].address, false);
			}

		}
	}
}
