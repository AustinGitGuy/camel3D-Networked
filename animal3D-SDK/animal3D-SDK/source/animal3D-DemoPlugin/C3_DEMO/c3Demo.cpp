#include "C3_DEMO/c3DemoState.h"

#include <GL/glew.h>

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
	glClear(GL_COLOR_BUFFER_BIT);

	if(demoState->inGame){
		//Draw the chatroom if we are in
		a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Chat: %s", demoState->str);
	}
	else {

	}
}

void c3demoUpdate(c3_DemoState const* demoState) {
	//Game loop goes here
	

}

void c3demoInput(c3_DemoState const* demoState) {
	//Grab keybaord input and send appriotrate command based on them











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
	a3_TextRenderer texRend;

	//a3textDraw(texRend.handle)
	printf("C to connect to a chat room or H to host a chat room or Q to quit the program\n");
	fgets(demoState->str, 127, stdin);

	if ((demoState->str[0] == 'c') || (demoState->str[0] == 'C')) {
		RakNet::SocketDescriptor sd;
		demoState->peer->Startup(1, &sd, 1);
		demoState->isServer = false;
	}
	else if ((demoState->str[0] == 'q') || (demoState->str[0] == 'Q')) {
		demoState->programTrue = false;
		demoState->running = false;
		
	}
	else { //You can actually type anything other than c for hosting
		RakNet::SocketDescriptor sd(demoState->serverPort, 0);
		demoState->peer->Startup(MAXCLIENTS, &sd, 1);
		demoState->isServer = true;
	}

	if (demoState->isServer) {
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		demoState->peer->SetMaximumIncomingConnections(MAXCLIENTS);
	}
	else {
		//If client: get the ip and the username, then start the client
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(demoState->str, 127, stdin);
		if (demoState->str[0] == '\n') {
			strcpy(demoState->str, "127.0.0.1");
		}

		printf("Enter your user name: ");
		if (demoState->str[0] == '\n') {
			strcpy(demoState->str, "Blank");
		}
		fgets(demoState->clientProfiles->profiles[0].name, 127, stdin);

		char* nameEnd;
		nameEnd = strchr(demoState->clientProfiles->profiles[0].name, '\n');
		*nameEnd = '\0';

		printf("Starting the client.\n");
		demoState->peer->Connect(demoState->str, demoState->serverPort, 0, 0);
	}
}