#include <iostream>
#include <Windows.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include "Memory.h"
#include "InputManager.h"
#include "log.h"
#include "gui.h"
#include "globals.h"
#include "structs.h"
#include "offsets.h"
#include "threads.h"
int main() {
	SetConsoleTitleA("Flux DMA");
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << R"(


  $$$$$$$$\ $$\                           $$$$$$$\  $$\      $$\  $$$$$$\  
  $$  _____|$$ |                          $$  __$$\ $$$\    $$$ |$$  __$$\ 
  $$ |      $$ |$$\   $$\ $$\   $$\       $$ |  $$ |$$$$\  $$$$ |$$ /  $$ |
  $$$$$\    $$ |$$ |  $$ |\$$\ $$  |      $$ |  $$ |$$\$$\$$ $$ |$$$$$$$$ |
  $$  __|   $$ |$$ |  $$ | \$$$$  /       $$ |  $$ |$$ \$$$  $$ |$$  __$$ |
  $$ |      $$ |$$ |  $$ | $$  $$<        $$ |  $$ |$$ |\$  /$$ |$$ |  $$ |
  $$ |      $$ |\$$$$$$  |$$  /\$$\       $$$$$$$  |$$ | \_/ $$ |$$ |  $$ |
  \__|      \__| \______/ \__/  \__|      \_______/ \__|     \__|\__|  \__|
           
=============================================================================
                               
    )" << std::endl;
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_RED);


	if (!mem.Init("FortniteClient-Win64-Shipping.exe")) {
		errorLog("Fortnite is not currently running, make sure it is open.");
		pause();
	}
	else {
		consoleLog("Successfully initialized on Fortnite!");
	}

	consoleLog("Got base address of 0x%zx", mem.current_process.base_address);

	if (Keyboard.InitKeyboard()) consoleLog("Keyboard Initialized!");
	else errorLog("Keyboard Couldn't Initialize!");

	findTextSection();

	std::thread readGlobalsLoop(readGlobals);
	readGlobalsLoop.detach();
	std::thread headPosReset(resetHeadPos);
	headPosReset.detach();
	std::thread espLoop(esp);
	espLoop.detach();
	std::thread aimLoop(aimbot);
	aimLoop.detach();
	std::thread triggerLoop(triggerbot);
	triggerLoop.detach();
	std::thread weaponLoop(getAllWeaponNames);
	weaponLoop.detach();
	std::thread cr3FixLoop(cr3Fix);
	cr3FixLoop.detach();
	std::thread nesp(nameesp);
	nesp.detach();
	/*if (settings::enableOnScreenESP); {
		std::thread OnScreenThread(OnScreen);
		OnScreenThread.detach();
	}*/
	GuiLoop();

	pause();
	return 0;
}
