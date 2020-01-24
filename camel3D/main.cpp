//Tutorial grabbed from http://www.jenkinssoftware.com/raknet/manual/

#include <stdio.h>
#include <string.h>
#include <thread>
#include <iostream>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"

enum GameMessages{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,ID_NAME
};

const unsigned int MAXCLIENTS = 10;

#pragma pack(push, 1)
struct MsgStruct {
	unsigned char id;
	char msg[127];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UserProfile {
	RakNet::SystemAddress address;
	char name[127];
	bool isHost;
};
#pragma pack(pop)

struct ProfileList {
	UserProfile profiles[MAXCLIENTS];
	int iter = 0;
};

void SendToClient(RakNet::RakPeerInterface* peer, ProfileList* clientProfiles, MsgStruct msg, int client = -1){
	if(client == -1){
		for(int i = 0; i < clientProfiles->iter; i++){
			peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[i].address, false);
		}
	}
	else {
		peer->Send((char*)&msg, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientProfiles->profiles[client].address, false);
	}
}

void PacketHandler(RakNet::RakPeerInterface* peer, bool isServer, unsigned int maxClients, unsigned int serverPort, UserProfile* profile, ProfileList* clientProfiles){
	char msg[127];
	
	bool running = true;

	RakNet::Packet* packet;

	while(running){
		for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()){
			switch(packet->data[0]){
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
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
				send.id = (RakNet::MessageID)ID_NAME;
				strcpy(send.msg, clientProfiles->profiles[0].name);

				//Cast to a char* to send the struct as a packet
				peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				profile->address = packet->systemAddress;
				profile->isHost = true;
				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if(isServer){
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				
			case ID_CONNECTION_LOST:
				if(isServer){
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				
				break;
			case ID_GAME_MESSAGE_1: {

				//Cast it back to a struct to be read
				MsgStruct* read = (MsgStruct*)packet->data;

				printf("%s\n", read->msg);

				//Message recieved
				SendToClient(peer, clientProfiles, *read);
				break;
			}

			case ID_NAME: {

				//If we are getting a name then store it for later.
				//Since this is the server we can still use the profileList struct
				MsgStruct* read = (MsgStruct*)packet->data;
				strcpy(clientProfiles->profiles[clientProfiles->iter].name, read->msg);
				clientProfiles->profiles[clientProfiles->iter].address = packet->systemAddress;

				printf("Client connected with name %s\n", clientProfiles->profiles[clientProfiles->iter].name);
				clientProfiles->iter++;
				break;
			}
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}
}

int main(void){
	unsigned short serverPort = 60000;

	char str[127];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	UserProfile profile; //This is for the server (if we are client)
	ProfileList clientProfiles;
	bool isServer;

	printf("C to connect to a chat room or H to host a chat room\n");
	fgets(str, 127, stdin);
	if ((str[0] == 'c') || (str[0] == 'C')){
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
	//You can actually type anything other than c for hosting
	else {
		RakNet::SocketDescriptor sd(serverPort, 0);
		peer->Startup(MAXCLIENTS, &sd, 1);
		isServer = true;
	}

	if(isServer){
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(MAXCLIENTS);
	}
	else {
		//If client: get the ip and the username, then start the client
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 127, stdin);
		if(str[0] == '\n'){
			strcpy(str, "127.0.0.1");
		}

		printf("Enter your user name: ");
		if(str[0] == '\n'){
			strcpy(str, "Blank");
		}
		fgets(clientProfiles.profiles[0].name, 127, stdin);

		char* nameEnd;
		nameEnd = strchr(clientProfiles.profiles[0].name, '\n');
		*nameEnd = ':';

		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);
	}

	bool running = true;

	//Start a new thread to handle packets so we can use this one to run input manager
	std::thread handlePackets(PacketHandler, peer, isServer, MAXCLIENTS, serverPort, &profile, &clientProfiles);

	while(running){
		fgets(str, 127, stdin);
		if(str[0] != '\n'){
			//Check if a command is typed
			if(str[0] != '/'){

				char* nameEnd;
				nameEnd = strchr(str, '\n');
				*nameEnd = ' ';

				char tmp[127];

				if(isServer){
					strcpy(tmp, "Server: ");
				}
				else {
					strcpy(tmp, clientProfiles.profiles[0].name);
				}
				

				MsgStruct send;
				send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;
				strcpy(send.msg, strcat(tmp, str));

				if(isServer){
					SendToClient(peer, &clientProfiles, send);
				}
				else {
					//Cast to a char* to send the struct as a packet
					peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, profile.address, false);
				}
			}
			//Otherwise check command
			else {

			}
		}

	}


	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
