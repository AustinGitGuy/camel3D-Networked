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
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1, ID_NAME_JOIN, ID_NAME_LEAVE, ID_GAME_MESSAGE_PRIVATE
};

const unsigned int MAXCLIENTS = 10;

#pragma pack(push, 1)
struct MsgStruct {
	unsigned char id;
	char name[127];
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

//Prints all connected cilents and their IP addresses
void PrintClientList(RakNet::RakPeerInterface* peer, ProfileList* clientProfiles, MsgStruct msg)
{
	for (int i = 0; i < clientProfiles->iter; i++)
	{
		
	}


}

//Below Fucntion is used to display all available commands for program
void DisplayCommands() 
{
	printf("/help : Displays available commands\n /private : Send a private message to the defined user (/private Username Message)\n /cilents Displays List of currently connected clients (Host Only) /quit : Return to chatroom selection.\n");
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
				//When a client has disconnected display message saying they have left
				MsgStruct send;
				send.id = (RakNet::MessageID)ID_NAME_LEAVE;
				strcpy(send.msg, clientProfiles->profiles[0].name);

				//Cast to a char* to send the struct as a packet
				peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				//Remove Disconnected user from profile list
				//profile->address = packet->systemAddress;
				//profile->isHost = true;
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

			case ID_NAME_JOIN: {

				//If we are getting a name then store it for later.
				//Since this is the server we can still use the profileList struct
				MsgStruct* read = (MsgStruct*)packet->data;
				strcpy(clientProfiles->profiles[clientProfiles->iter].name, read->msg);
				clientProfiles->profiles[clientProfiles->iter].address = packet->systemAddress;

				printf("Client connected with name %s\n", clientProfiles->profiles[clientProfiles->iter].name);
				clientProfiles->iter++;
				break;
			}
			case ID_NAME_LEAVE: {

				//If we are getting a name then store it for later.
				//Since this is the server we can still use the profileList struct
				MsgStruct* read = (MsgStruct*)packet->data;
				strcpy(clientProfiles->profiles[clientProfiles->iter].name, read->msg);
				clientProfiles->profiles[clientProfiles->iter].address = packet->systemAddress;

				printf(clientProfiles->profiles[clientProfiles->iter].name, "has disconnected %s\n");
				clientProfiles->iter--;
				break;
			}

			case ID_GAME_MESSAGE_PRIVATE: {

				//Cast it back to a struct to be read
				MsgStruct* read = (MsgStruct*)packet->data;

				int i;
				for (i = 0; i < clientProfiles->iter; i++)
				{
					if (strcmp(read->name, clientProfiles->profiles[i].name) == 0)//compares Name in str to list of names in clientProfiles
					{
						break;
					}
					
				}

				if (i >= clientProfiles->iter)
				{
					strcpy(read->msg, "Could not find user.\n");

					peer->Send((char*)& read, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					break;

				}
				else
				{
					printf("%s\n", read->msg);

					//Message recieved
					SendToClient(peer, clientProfiles, *read, i);
					break;
				}

				
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
	bool running = true;
	bool programTrue;//Used to reset back to lobby


	printf("C to connect to a chat room or H to host a chat room or Q to quit the program\n");
	fgets(str, 127, stdin);
	if ((str[0] == 'c') || (str[0] == 'C')){
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
	else if ((str[0] == 'q') || (str[0] == 'Q')) {
		running = false;
	}
	else { //You can actually type anything other than c for hosting
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
					strcpy(tmp, "Server: ");//Appends Server to Beginning of Message
				}
				else {
					strcpy(tmp, clientProfiles.profiles[0].name);//Appends Cilent Username to Beginning of Message
				}

				strcpy(tmp, "(Public) ");//Appends a notification that Message is public

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

				if (str[1] == 'h')//Displays list of commands
				{
					DisplayCommands();
				}
				else if(str[1] == 'p')//Private message
				{
					

					char* trash;
					trash = strchr(str, '/');
					*trash = ' ';
					trash = strchr(str, 'p');
					*trash = ' ';

					

					char* nameEnd;
					nameEnd = strchr(str, '\n');
					*nameEnd = ' ';

					char tmp[127];

					if (isServer) {
						strcpy(tmp, "Server: ");//Appends Server to Beginning of Message
					}
					else {
						strcpy(tmp, clientProfiles.profiles[0].name);//Appends Client Username to Beginning of Message
					}

					strcpy(tmp, "(Whisper) ");//Appends a notification that Message is private

					MsgStruct send;
					send.id = (RakNet::MessageID)ID_GAME_MESSAGE_1;
					strcpy(send.name, str);
					strcpy(send.msg, strcat(tmp, str));

					peer->Send((char*)& send, sizeof(MsgStruct), HIGH_PRIORITY, RELIABLE_ORDERED, 0, profile.address, false);

				}
				else if (str[1] == 'c')//Displays list of connected clients
				{
					if (isServer)//If server run command
					{
						//PrintClientList(peer, clientProfiles, )
					}
					else//If not server deny access
					{
						printf("Access to command denied.\n");
					}
				}
				else if (str[1] == 'q')//Leaves current chat room and brings user back to the Lobby
				{

				}
				else//If no command is recognized display message and display list of commands
				{
					printf("Invalid Command. Please use one of the below commands.\n");
					DisplayCommands();
				}


			}
		}

	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
