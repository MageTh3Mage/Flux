#include "threads.h"

void findTextSection() {
	globals::va_text = 0;
	if (!globals::va_text) {
		for (auto i = 0; i < INT_MAX; i++) {
			globals::va_text = mem.current_process.base_address + i * 0x1000;
			globals::u_world = mem.Read<uintptr_t>(globals::va_text + offsets::Uworld);
			auto Level = mem.Read<uintptr_t>(globals::u_world + offsets::PersistentLevel);
			if (globals::u_world && Level && mem.Read<uintptr_t>(Level + offsets::OwningWorld) == globals::u_world)
				break; // checks if the persistant level + owning world = uworld (then uworld is correct)
		}
		consoleLog("Found text section: 0x%llx", globals::va_text);
		consoleLog("Found uworld: 0x%zx", globals::u_world);
	}
}

void resetHeadPos() {
	while (true) {
		for (uint32_t i = 0; i < globals::Num; i++) {
			cache::headpos[i] = Vector3(0, 0, 0);
		}
		Sleep(2000);

	}
}
void readGlobals() {
	while (true) {
		if (globals::readGlobals == false) { 
			Sleep(10);
			continue; 
		}
		globals::u_world = mem.Read<uintptr_t>(globals::va_text + offsets::Uworld);

		globals::GameState = mem.Read<uintptr_t>(globals::u_world + offsets::GameState);

		globals::Gameinstance = mem.Read<uintptr_t>(globals::u_world + offsets::GameInstance);

		globals::LocalPlayers = mem.Read<uintptr_t>(globals::Gameinstance + offsets::LocalPlayers);

		globals::Localplayer = mem.Read<uintptr_t>(globals::LocalPlayers);

		globals::PlayerController = mem.Read<uintptr_t>(globals::Localplayer + offsets::PlayerController);

		globals::LocalPawn = mem.Read<uintptr_t>(globals::PlayerController + offsets::LocalPawn);

		globals::PlayerArray = mem.Read<uintptr_t>(globals::GameState + offsets::PlayerArray);

		globals::Num = mem.Read<int>(globals::GameState + (offsets::PlayerArray + sizeof(uintptr_t)));

		VMMDLL_SCATTER_HANDLE scatterHandle = mem.CreateScatterHandle();
		mem.AddScatterReadRequest(scatterHandle, globals::u_world + 0x110, &globals::location_pointer, sizeof(uintptr_t));
		mem.AddScatterReadRequest(scatterHandle, globals::u_world + 0x120, &globals::rotation_pointer, sizeof(uintptr_t));
		mem.ExecuteReadScatter(scatterHandle);

		VMMDLL_Scatter_CloseHandle(scatterHandle);

		
		
		Sleep(100);
	}
}

bool triggerAlways = false;

void triggerbot() {
	while (true) {
		while (Keyboard.IsKeyDown(0x51) || triggerAlways) {
			if (mem.Read<uintptr_t>(globals::PlayerController + 0x1898) != 0) // Offsets::TargetedFortPawn);
			{
				SendAim("click");
				Sleep(30);
			}
			//Sleep(1);
		}
		if (!triggerAlways) {
			Sleep(1);
		}
	}
}

/*void readWeapon() {
	while (true) {
		globals::currentWeapon = mem.Read<uintptr_t>(globals::LocalPawn + 0x9d8);
		globals::bulletSpeed = mem.Read<float>(globals::currentWeapon + 0x1d0c);
		globals::bulletGravity = mem.Read<float>(globals::currentWeapon + 0x1aa0);
		std::cout << globals::currentWeapon << std::endl;
		//std::cout << bulletSpeed << std::endl;
		//std::cout << bulletGravity << std::endl;
		Sleep(200);
	}
}

void weaponName(int i) {
	uint64_t wdata = cache::cachedWd[i];
	if (!cache::cachedWd[i]) {
		cache::cachedWeaponName[i] = "No Weapon";
		return;
	}
	uint64_t ftext_ptr = mem.Read<uint64_t>(wdata + 0x40);
	if (!cache::cachedftext_ptr[i]) {
		cache::cachedWeaponName[i] = "No Weapon";
		return;
	}
	uint64_t ftext_data = mem.Read <uint64_t>(ftext_ptr + 0x28);
	if (!cache::cachedftext_data[i]) {
		cache::cachedWeaponName[i] = "No Weapon";
		return;
	}
	int ftext_length = mem.Read <int>(ftext_ptr + 0x30);
	if (!cache::cachedftext_length[i]) {
		cache::cachedWeaponName[i] = "No Weapon";
		return;
	}
	if (cache::cachedftext_length[i] > 0 && cache::cachedftext_length[i] < 50)
	{
		wchar_t* ftext_buf = new wchar_t[cache::cachedftext_length[i]];
		mem.Read(cache::cachedftext_data[i], ftext_buf, cache::cachedftext_length[i] * sizeof(wchar_t));
		std::wstring wstr_buf(ftext_buf);
		cache::cachedWeaponName[i] = std::string(wstr_buf.begin(), wstr_buf.end());
		delete[] ftext_buf;
		cache::cachedWeaponName;
	}
	auto weaponName = cache::cachedWeaponName[i];
}*/


void cr3Fix() {
	while (true) {
		if (!mem.Read<int>(mem.current_process.base_address)) mem.FixCr3(); 
		Sleep(60);

		//mem.FixCr3();
		//Sleep(4000);
	}
}