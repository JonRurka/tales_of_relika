#pragma once

#include <stdint.h>

class OpCodes {
public:
	enum class Server : uint8_t {
		System_Reserved = 0xff,
		Submit_Identity = 0x01,
		World_Command = 0x02,
		Join_Match = 0x03
	};

	enum class Server_World : uint8_t {
		Debug_Start = 0x01,
		Request_World_Player_Data = 0x02,
		Request_Players = 0x03,
		Update_Orientation = 0x04,
		Player_Event = 0x05,
		Notify_Player_Ready = 0x06,
	};

	enum class World_Chunk_Events : uint8_t {
		None = 0x00,
	};

	enum class Client : uint8_t {
		System_Reserved = 0xff,
		Identify_Result = 0x01,
		World_Player_Data_Result = 0x02,
		Spawn_Players = 0x03,
		Update_Orientations = 0x04,
		Sync_Player_Orientation = 0x05,
		Player_Events = 0x06,
		Chunk_Events = 0x07,
	};

	enum class Player_Events : uint8_t {
		None = 0x00,
		Process_Move,
		Jump,
	};

	enum class Player_Chunk_Events : uint8_t {
		None = 0x00,
		NotifyLoaded,
	};

};