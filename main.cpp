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
constexpr uint64_t MAX_PHYADDR = 0xFFFFFFFFF;
uint64_t lastCorrectDtbPhysicalAddress = 0x0;

inline uint64_t GetFurtherDistance(uint64_t A, uint64_t Min, uint64_t Max) {
	uint64_t distanceToMin = (A > Min) ? (A - Min) : (Min - A);
	uint64_t distanceToMax = (A > Max) ? (A - Max) : (Max - A);

	return (distanceToMin > distanceToMax) ? distanceToMin : distanceToMax;
}

bool GetDTBSuccess(uint64_t dtbPhysicalAddress) {
	VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_PROCESS_DTB | mem.current_process.PID, dtbPhysicalAddress);

	bool result = VMMDLL_Map_GetModuleFromNameU(mem.vHandle, mem.current_process.PID, (LPSTR)mem.current_process.process_name.c_str(), &mem.module_entry, NULL);

	std::cout << "Tried: " << dtbPhysicalAddress << std::endl;
	return result;
}
bool bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage, uint64_t* outDTBAddress)
{
	// eac cr3 always end with 0x-----XX000
	// dtbStartPhysicalAddr should be a multiple of 0x1000
	if ((dtbStartPhysicalAddr & 0xFFF) != 0) return false;
	if (dtbStartPhysicalAddr > MAX_PHYADDR) return false;

	dtbStartPhysicalAddr -= dtbStartPhysicalAddr % stepPage;
	dtbStartPhysicalAddr += lastCorrectDtbPhysicalAddress % stepPage;

	auto start = std::chrono::high_resolution_clock::now();
	bool result = false;
	uint64_t furtherDistance = GetFurtherDistance(dtbStartPhysicalAddr, 0x0, MAX_PHYADDR);
	size_t maxStep = furtherDistance / stepPage;
	// printf("[*] furtherDistance:0x%llx, maxStep:0x%zx\n", furtherDistance, maxStep);

	for (size_t step = 0; step < maxStep; step++) {
		// bruteforce dtb from middle
		uint64_t guessDtbAddr = dtbStartPhysicalAddr + step * stepPage;
		if (guessDtbAddr < MAX_PHYADDR) {
			if (GetDTBSuccess(guessDtbAddr)) {
				result = true;
				*outDTBAddress = guessDtbAddr;
				break;
			}
		}
		// dont forget the other side
		if (dtbStartPhysicalAddr > step * stepPage) {
			guessDtbAddr = dtbStartPhysicalAddr - step * stepPage;
			if (GetDTBSuccess(guessDtbAddr)) {
				result = true;
				*outDTBAddress = guessDtbAddr;
				break;
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	printf("[+] bruteforce dtb %s to find dtb:0x%llx, time use:%lldms\n", result ? "success" : "failed", result ? *outDTBAddress : 0x0, duration.count());

	// In case we cannot get the dtb through this shortcut method.
	if (result == false && stepPage != 0x1000) {
		return bruteforceDtb(dtbStartPhysicalAddr, 0x1000, outDTBAddress);
	}
	return result;
}

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

	//if (VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_CONFIG_TLBCACHE_TICKS, 1)) printf("set refresh\n");
	if (!mem.Init("FortniteClient-Win64-Shipping.exe")) {
		errorLog("Fortnite is not currently running, make sure it is open.");
		pause();
	}
	else {
		consoleLog("Successfully initialized on Fortnite!");
	}
	
	//uint64_t dtb = 0x0;
	//// do not set stepPage more than 0x100000
	//while (true) {
	//	if (bruteforceDtb(0x300000000, 0x100000, &dtb)) {
	//		lastCorrectDtbPhysicalAddress = dtb;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//}

	consoleLog("Got base address of 0x%zx", mem.current_process.base_address);

	if (Keyboard.InitKeyboard()) consoleLog("Keyboard Initialized!");
	else errorLog("Keyboard Couldn't Initialize!");
	

	//findTextSection();
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
	std::thread vis(visCheckLoop);
	vis.detach();
	
	/*if (settings::enableOnScreenESP); {
		std::thread OnScreenThread(OnScreen);
		OnScreenThread.detach();
	}*/
	//while (true) {
	//	VMMDLL_PROCESS_INFORMATION ProcessInformation;
	//	SIZE_T cbProcessInformation = sizeof(VMMDLL_PROCESS_INFORMATION);
	//	ZeroMemory(&ProcessInformation, sizeof(VMMDLL_PROCESS_INFORMATION));
	//	ProcessInformation.magic = VMMDLL_PROCESS_INFORMATION_MAGIC;
	//	ProcessInformation.wVersion = VMMDLL_PROCESS_INFORMATION_VERSION;
	//	//printf("CALL:    VMMDLL_ProcessGetInformation\n");
	//	bool result = VMMDLL_ProcessGetInformation(mem.vHandle, mem.current_process.PID, &ProcessInformation, &cbProcessInformation);
	//	if (result) {
	//		//printf("SUCCESS: VMMDLL_ProcessGetInformation\n");
	//		//printf("         Name = %s\n", ProcessInformation.szName);
	//		printf("         PageDirectoryBase = 0x%016llx\n", ProcessInformation.paDTB);
	//		//printf("         PageDirectoryBaseUser = 0x%016llx\n", ProcessInformation.paDTB_UserOpt);
	//		//printf("         ProcessState = 0x%08x\n", ProcessInformation.dwState);
	//		//printf("         PID = 0x%08x\n", ProcessInformation.dwPID);
	//		//printf("         ParentPID = 0x%08x\n", ProcessInformation.dwPPID);
	//	}
	//	Sleep(1000);

	//}
	GuiLoop();

	pause();
	return 0;
}
