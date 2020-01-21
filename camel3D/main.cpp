//Tutorial grabbed from http://www.jenkinssoftware.com/raknet/manual/

#include <stdio.h>
#include <string.h>
#include <thread>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"

enum GameMessages{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

#pragma pack(push, 1)
struct MsgStruct {
	unsigned char id;
	unsigned char msg[127];
};
#pragma pack(pop)

struct UserProfile {
	RakNet::SystemAddress address;
	char* name;
	bool isHost;
};

void PacketHandler(RakNet::RakPeerInterface* peer, bool isServer, unsigned int maxClients, unsigned int serverPort){
	char msg[127];
	RakNet::Packet* packet;

	bool running = true;

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
				send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;

				//Cast to a char* to send the struct as a packet
				peer->Send((char*)&send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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

				//Message recieved
				printf("Message recieved with key %i\n", read->id);
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
	unsigned int maxClients = 10;
	unsigned short serverPort = 60000;

	char str[127];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
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
		peer->Startup(maxClients, &sd, 1);
		isServer = true;
	}

	if(isServer){
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(maxClients);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 127, stdin);
		if(str[0] == '\n'){
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);
	}

	bool running = true;

	std::thread handlePackets(PacketHandler, peer, isServer, maxClients, serverPort);

	while(running){
		fgets(str, 127, stdin);
		if(str[0] != '\n'){
			//Check if a command is typed
			if(str[0] != '/'){
				MsgStruct send;
				send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;

				//Cast to a char* to send the struct as a packet
			}
			//Otherwise send the message
			else {

			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
