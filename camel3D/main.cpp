//Tutorial grabbed from http://www.jenkinssoftware.com/raknet/manual/

#include <stdio.h>
#include <string.h>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"

enum GameMessages{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

int main(void){
	unsigned int maxClients = 10;
	unsigned short serverPort = 60000;

	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	bool isServer;

	printf("(C) or (S)erver?\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'c') || (str[0] == 'C')){
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
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
		fgets(str, 512, stdin);
		if(str[0] == '/n'){
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);

	}

	while(1){
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

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOut.Write("Hello world");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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
				break;
			case ID_CONNECTION_LOST:
				if(isServer){
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;
				case ID_GAME_MESSAGE_1: {
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data,packet->length,false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
				}
				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}


	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
