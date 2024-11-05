#include "threads.h"

/*void findTextSection() {
	globals::va_text = 0;
	if (!globals::va_text) {
		for (auto i = 0; i < FLT_MAX ; i++) {
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

void findTextSection() {
	if (!globals::va_text) {
		for (auto i = 0; i < 25; i++) {
			globals::va_text = mem.current_process.base_address + i * 0x1000;
			globals::u_world = mem.Read<uintptr_t>(globals::va_text + offsets::Uworld);
		}
		consoleLog("Found uworld: 0x%zx", globals::u_world);
		consoleLog("Found text section: 0x%llx", globals::va_text);
	}
}*/

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
		if (globals::readGlobals == false || globals::CR3Fixed == false) { 
			Sleep(10);
			continue; 
		}
		globals::u_world = mem.Read<uintptr_t>(mem.current_process.base_address + offsets::Uworld);

		globals::GameState = mem.Read<uintptr_t>(globals::u_world + offsets::GameState);

		globals::Gameinstance = mem.Read<uintptr_t>(globals::u_world + offsets::GameInstance);

		globals::LocalPlayers = mem.Read<uintptr_t>(globals::Gameinstance + offsets::LocalPlayers);

		globals::Localplayer = mem.Read<uintptr_t>(globals::LocalPlayers);

		globals::PlayerController = mem.Read<uintptr_t>(globals::Localplayer + offsets::PlayerController);

		globals::LocalPawn = mem.Read<uintptr_t>(globals::PlayerController + offsets::LocalPawn);

		globals::PlayerArray = mem.Read<uintptr_t>(globals::GameState + offsets::PlayerArray);

		globals::Num = mem.Read<int>(globals::GameState + (offsets::PlayerArray + sizeof(uintptr_t)));

		/*std::cout << globals::Localplayer << std::endl;
		std::cout << globals::GameState << std::endl;
		std::cout << globals::Gameinstance << std::endl;
		std::cout << globals::PlayerController << std::endl;
		std::cout << globals::LocalPawn << std::endl;
		std::cout << globals::PlayerArray << std::endl;*/

		//VMMDLL_SCATTER_HANDLE scatterHandle = mem.CreateScatterHandle();
		//mem.AddScatterReadRequest(scatterHandle, globals::u_world + offsets::CameraLocation, &globals::location_pointer, sizeof(uintptr_t));
		//mem.AddScatterReadRequest(scatterHandle, globals::u_world + offsets::CameraRotation, &globals::rotation_pointer, sizeof(uintptr_t));
		//mem.ExecuteReadScatter(scatterHandle);
		///*std::cout << globals::location_pointer << std::endl;
		//std::cout << globals::rotation_pointer << std::endl;*/
		//VMMDLL_Scatter_CloseHandle(scatterHandle);

		
		
		Sleep(1000);
	}
}

bool triggerAlways = false;

void triggerbot() {
	while (true) {
		while (Keyboard.IsKeyDown(0x06) || triggerAlways) {
			if (mem.Read<uintptr_t>(globals::PlayerController + offsets::TargetedFortPawn) != 0)
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

void cr3Fix() {
	while (true) {
		if (!mem.Read<int>(mem.current_process.base_address)) mem.FixCr3(); 
		Sleep(50);

		//mem.FixCr3();
		//Sleep(4000);
	}
}