#include "main.hpp"

c_siege siege;

int main()
{

	printf("[~] Waiting for Rainbow Six Siege\n");
	do
	{
		siege.window_handle = FindWindow("R6Game", NULL);
		Sleep(250);
	} while (siege.window_handle == nullptr);

	printf("[+] Found Rainbow 6 Siege\n");

	siege.base_address = siege.get_base_address(siege.window_handle);
	printf("[+] Base Address -> 0x%llx\n", siege.base_address);
	siege.process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, siege.process_id);

	siege.get_game_manager();

	printf("[+] GameManager -> 0x%llx\n", siege.game_manager);
	printf("[+] playerCount -> %d\n", siege.get_player_count());


	auto player_list = siege.get_player_list();
	printf("[+] playerList -> 0x%llx\n", player_list);
	
	while (1) {
		for (int i = 0; i < siege.get_player_count(); i++)
		{
			uintptr_t entity;
			ReadProcessMemory(siege.process_handle, (LPVOID)(player_list + i * 0x8), &entity, sizeof(entity), NULL);
			//printf("[+] entityObj ->  0x%llx\n", entity);

			uintptr_t pawn = siege.get_pawn_from_entity(entity);

			//printf("[+] pawn ->0x%llx\n", pawn);

			uint64_t actor = siege.get_actor_from_pawn(pawn);
			//printf("[+] actor ->0x%llx\n", actor);

			Vector3 position = siege.get_actor_position(actor);
			printf("Entity Position: %f, %f, %f\n", position.x, position.y, position.z);

			uint64_t p_outline_component;
			ReadProcessMemory(siege.process_handle, (LPVOID)(actor + 0x1C8), &p_outline_component, sizeof(p_outline_component), NULL);

			p_outline_component ^= 0xD5C0B11588D67FD4ui64;
			//printf("[+] outlineComp -> 0x%llx\n", p_outline_component);

			uint64_t outline = 0x20748000;
			WriteProcessMemory(siege.process_handle, (LPVOID)(p_outline_component + 0xB0), &outline, sizeof(outline), NULL);
			Sleep(30);
		}
	}
}