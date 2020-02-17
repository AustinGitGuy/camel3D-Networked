#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"
#include "RakNet/GetTime.h"

#include <vector>
#include <string>

#include "A3_DEMO/a3_Networking/a3_Networking_gs_tictactoe.c"
#include "A3_DEMO/a3_Networking/a3_Networking_gs_checkers.c"
#include "C3_DEMO/c3EventManager.h"

//#include "c3Event.h"

#ifndef __CAMEL3D_DEMOSTATE_H
#define __CAMEL3D_DEMOSTATE_H


enum GameMessages {
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1, ID_NAME_JOIN, ID_NAME_LEAVE, ID_GAME_MESSAGE_PRIVATE, ID_GAME_MOVE, ID_INVITE, ID_GAME_EVENT, DEFAULT_EVENT_ID
};

enum UserGameState {
	PLAYER_ACTIVE, PLAYER_INACTIVE, SPECTATOR, LOBBY
};

enum ActiveGame{
	TIC_TAC_TOE, CHECKERS
};

enum EventType {
	NULL_TYPE, MOVE_EVENT, STRETCH_EVENT, COLORCHANGE_EVENT, CLONE_EVENT, NUM_EVENT_TYPES
};

const unsigned int MAXCLIENTS = 10;

const int EVENT_SIZE = 10;

#pragma pack(push, 1)
struct EventTypeStruct {
	EventType type = NULL_TYPE;
	float float1, float2, float3;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MsgStruct {
	unsigned char id;

	char senderName[127];
	char receiveName[127];
	char msg[127];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct EventStruct {
	unsigned char id;
	EventTypeStruct events[EVENT_SIZE];
	RakNet::Time timeStamp;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GameMove {
	unsigned char id;
	char senderName[127];
	char receiveName[127];
	ActiveGame currentGame;
	int xPos;
	int yPos;
	int newXPos;
	int newYPos;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UserProfile {
	RakNet::SystemAddress address;
	char name[127];
	UserGameState playState;
	bool isHost;
};
#pragma pack(pop)

struct ProfileList {
	UserProfile profiles[MAXCLIENTS];
	int iter = 0;
};

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct c3_DemoState					c3_DemoState;
#endif	// __cplusplus

const int MAX_CHARACTERS = 127;

	struct c3_DemoState
	{
		//---------------------------------------------------------------------
		// general variables pertinent to the state
	
		// terminate key pressed
		a3i32 exitFlag;
	
		// global vertical axis: Z = 0, Y = 1
		a3i32 verticalAxis;
	
		// asset streaming between loads enabled (careful!)
		a3i32 streaming;
	
		// window and full-frame dimensions
		a3ui32 windowWidth, windowHeight;
		a3real windowWidthInv, windowHeightInv, windowAspect;
		a3ui32 frameWidth, frameHeight;
		a3real frameWidthInv, frameHeightInv, frameAspect;
		a3i32 frameBorder;
	
	
		//---------------------------------------------------------------------
		// objects that have known or fixed instance count in the whole demo
	
		// text renderer
		a3i32 textInit, textMode, textModeCount;
		a3_TextRenderer text[1];
	
		// input
		a3_MouseInput mouse[1];
		a3_KeyboardInput keyboard[1];
		a3_XboxControllerInput xcontrol[4];
	
		// pointer to fast trig table
		a3f32 trigTable[4096 * 4];
	
		//---------------------------------------------------------------------
		//Networking stuff
	
		a3_Timer renderTimer[1];
	
		RakNet::RakPeerInterface* peer;

		struct RemoteInfo {
			char remoteName[32];
			int data;
		};

		RemoteInfo remote[20];

		//------------------------------
		//Camel3D Stuff
		char str[MAX_CHARACTERS];
		char ip[MAX_CHARACTERS];
		char user[MAX_CHARACTERS];
		int index = 0;
		unsigned short serverPort = 60000;
		RakNet::Packet* packet;
		UserProfile profile; //This is for the server (if we are client)
		ProfileList clientProfiles;
		bool isServer;
		bool inGame = false;
		bool inConsole = false;
		bool localSim = false;
		int lobbyStage = 0;

		std::string chatLog[100];

		int chatIter = 0;

		//-----------------------------------
		//Game Stuff
		gs_tictactoe tttGame;
		gs_checkers checkersGame;

		bool isTTT = false;
		bool isPlayer1 = false;

		float xPos = 0, yPos = 0;
		float xScale = 0.25f, yScale = 0.25f;

		float red, blue, green;

		int numCubes = 0;
	};

	void c3demoRender(c3_DemoState const* demoState);

	void c3demoUpdate(c3_DemoState const* demoState);

	void c3demoInput(c3_DemoState* demoState);

	void c3demoNetworkingRecieve(c3_DemoState* demoState);

	void SendToClient(RakNet::RakPeerInterface* peer, const ProfileList* clientProfiles, MsgStruct msg, int client);

	void c3demoNetworkingLobby(c3_DemoState* demoState);

	void c3demoInputLab3(c3_DemoState* demoState, a3i32 asciiKey);

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif