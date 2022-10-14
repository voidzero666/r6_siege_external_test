#pragma once
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <xmmintrin.h>
#include "TlHelp32.h"
#include "defines.h"
#include "vector.h"

#define OFFSET_GAMEMANAGER 0x71FBDD8
#define OFFSET_PROFILEMANAGER 0x7033EB0
#define OFFSET_CAMERAMANAGER 0x60F0398

class c_siege
{
public:
	uintptr_t base_address;
	DWORD process_id;
	HWND window_handle = nullptr;
	HANDLE process_handle;

	//game vars
	uint64_t game_manager;
	uint64_t profile_manager;
	uint64_t camera_manager;

	uint64_t outline_component;
	uint64_t local_player;

	//fncs

	uintptr_t get_base_address(HWND hwnd)
	{
		uintptr_t base = NULL;

		GetWindowThreadProcessId(hwnd, &process_id);

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);

			if (Module32First(hSnap, &modEntry))
			{
				do
				{
					if (!_stricmp(modEntry.szModule, "RainbowSix.exe"))
					{
						base = (uintptr_t)modEntry.modBaseAddr;
					}

				} while (Module32Next(hSnap, &modEntry));
			}
		}
		CloseHandle(hSnap);
		return base;
	}

	uint32_t get_player_count() {
		uint32_t playerCount;
		ReadProcessMemory(process_handle, (LPVOID)(game_manager + 0xF0), &playerCount, sizeof(playerCount), NULL);
		playerCount ^= 0x87CA8BD06502E5FE;
		playerCount &= 0x3FFFFFFF;
		return playerCount;
	}

	uint64_t get_player_list() {
		uint64_t playerList;
		ReadProcessMemory(process_handle, (LPVOID)(game_manager + 0xE8), &playerList, sizeof(playerList), NULL);
		playerList ^= 0x87CA8BD06502E5FE;
		return playerList;
	}

	uint64_t get_replication_info() {
		uint64_t replication_info;
		ReadProcessMemory(process_handle, (LPVOID)(game_manager + 0x70), &replication_info, sizeof(replication_info), NULL);

		uint64_t rolled_replication_info = __ROL8__(__ROL8__(replication_info, 38) - 94i64, 32);

		return rolled_replication_info;
	}

	uint32_t get_team_id() {
		uint64_t replInfo = get_replication_info();

		if (replInfo < 0x0001000 || replInfo > 0x7FFFFFFEFFFF) {
			return 0;
		}
		auto v9 = *(uint64_t*)(replInfo + 0x728) - 88i64;
		if ((v9 ^ 0x8087EB2FEBB40640ui64) == 23)
			return 6;
		else
			return ((*(uint64_t*)((v9 ^ 0x8087EB2FEBB40640ui64) - 23 + 0xB4) + 0x4DE0D74E) ^ 0x11A281F9) - 27;
	}

	uintptr_t get_pawn_from_entity(uintptr_t entityPtr) {
		uintptr_t pawn;
		ReadProcessMemory(process_handle, (LPVOID)(entityPtr + 0x38), &pawn, sizeof(pawn), NULL);
		pawn = (_rotl64(pawn, 40) ^ 0x52A261F3563389E0i64) - 108;
		return pawn;
	}

	uint64_t get_actor_from_pawn(uintptr_t entityPawn) {
		uint64_t actor;
		ReadProcessMemory(process_handle, (LPVOID)(entityPawn + 0x18), &actor, sizeof(actor), NULL);

		return _rotl64(actor + 0x3705D616EBB5E15E, 0x1) + 0xA2A7BD6FF29CDA3;
	}

	uint64_t get_local_player()
	{
		uint64_t local;
		ReadProcessMemory(process_handle, (LPVOID)(profile_manager + 0x48), &local, sizeof(local), NULL);

		return _rotl64((local - 0x2D7DB14E189509D5i64) ^ 0x6D59970CA20D421Di64, 16);
	}

	void get_game_manager() {
		ReadProcessMemory(process_handle, (LPVOID)(base_address + OFFSET_GAMEMANAGER), &game_manager, sizeof(game_manager), NULL);
		game_manager = (((game_manager - 0x5013A972F916FB7Ei64) ^ 0x280DDB3232666F21i64) - 0x5A);
	}

	void get_profile_manager() {
		ReadProcessMemory(process_handle, (LPVOID)(base_address + OFFSET_PROFILEMANAGER), &profile_manager, sizeof(profile_manager), NULL);
		profile_manager = __ROL8__(profile_manager - 0x2A647AA9C0680BECi64, 44);
	}

	void get_camera_manager() {
		ReadProcessMemory(process_handle, (LPVOID)(base_address + OFFSET_CAMERAMANAGER), &camera_manager, sizeof(camera_manager), NULL);
	}

	Vector3 get_actor_position(uint64_t actor) {
		Vector3 pos;
		ReadProcessMemory(process_handle, (LPVOID)(actor + 0x50), &pos, sizeof(pos), NULL);
		return pos;
	}
};

extern c_siege siege;
