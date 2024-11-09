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

typedef union _virt_addr_t
{
	uint64_t value;
	struct
	{
		uintptr_t offset : 12;
		uintptr_t pt_index : 9;
		uintptr_t pd_index : 9;
		uintptr_t pdpt_index : 9;
		uintptr_t pml4_index : 9;
		uintptr_t reserved : 16;
	};
} virt_addr_t;

struct _MMPTE_HARDWARE {
	uint64_t Valid : 1;                  // Indicates if the entry is valid
	uint64_t Write : 1;                  // Write permission
	uint64_t Owner : 1;                  // User or supervisor mode
	uint64_t WriteThrough : 1;           // Write-through caching
	uint64_t CacheDisable : 1;           // Cache disabled
	uint64_t Accessed : 1;               // Accessed bit
	uint64_t Dirty : 1;                  // Dirty bit
	uint64_t LargePage : 1;              // Page size (1 for large pages)
	uint64_t Global : 1;                 // Global page bit
	uint64_t CopyOnWrite : 1;            // Copy-on-write flag
	uint64_t Prototype : 1;              // Prototype PTE
	uint64_t reserved : 1;               // Reserved bit
	uint64_t PageFrameNumber : 36;       // Page frame number (physical address)
	uint64_t reserved2 : 4;              // Reserved
	uint64_t SoftwareWsIndex : 11;       // Software working set index
	uint64_t NoExecute : 1;              // No-execute bit
};

struct _MMPTE {
	_MMPTE_HARDWARE Hard;                // Hardware part of the PTE
	// Other parts of the _MMPTE structure may go here if needed
};
uint64_t GetProcessCr3(uint64_t base) {
	if (!base)
		return 0;

	PVMMDLL_MAP_PHYSMEM phys_mem_map = NULL;
	if (!VMMDLL_Map_GetPhysMem(mem.vHandle, &phys_mem_map)) {
		return 0;
	}

	if (phys_mem_map->dwVersion != VMMDLL_MAP_PHYSMEM_VERSION) {
		VMMDLL_MemFree(phys_mem_map);
		return 0;
	}

	virt_addr_t virt_base{};
	virt_base.value = base;

	SIZE_T read = 0;
	uint64_t current_phys_address = 0;
	DWORD phys_pid = (DWORD)-1;
	for (DWORD i = 0; i < phys_mem_map->cMap; i++) {
		VMMDLL_MAP_PHYSMEMENTRY entry = phys_mem_map->pMap[i];
		current_phys_address = entry.pa;

		for (int j = 0; j < (entry.cb / 0x1000); j++, current_phys_address += 0x1000) {
			_MMPTE entries[4] = {};

			if (!mem.Read((current_phys_address + 8 * virt_base.pml4_index), &entries[0], sizeof(_MMPTE), phys_pid) ||
				!entries[0].Hard.Valid ||
				!mem.Read(((entries[0].Hard.PageFrameNumber << 12) + 8 * virt_base.pdpt_index), &entries[1], sizeof(_MMPTE), phys_pid) ||
				!entries[1].Hard.Valid ||
				!mem.Read(((entries[1].Hard.PageFrameNumber << 12) + 8 * virt_base.pd_index), &entries[2], sizeof(_MMPTE), phys_pid) ||
				!entries[2].Hard.Valid ||
				!mem.Read(((entries[2].Hard.PageFrameNumber << 12) + 8 * virt_base.pt_index), &entries[3], sizeof(_MMPTE), phys_pid) ||
				!entries[3].Hard.Valid)
			{
				//std::cout << current_phys_address << std::endl;
				continue;
			}

			VMMDLL_MemFree(phys_mem_map);
			return current_phys_address >> 12;
		}
	}

	VMMDLL_MemFree(phys_mem_map);
	return 0;
}

void cr3() {

	VMMDLL_PROCESS_INFORMATION ProcessInformation;
	SIZE_T cbProcessInformation = sizeof(VMMDLL_PROCESS_INFORMATION);
	ZeroMemory(&ProcessInformation, sizeof(VMMDLL_PROCESS_INFORMATION));
	ProcessInformation.magic = VMMDLL_PROCESS_INFORMATION_MAGIC;
	ProcessInformation.wVersion = VMMDLL_PROCESS_INFORMATION_VERSION;
	printf("CALL:    VMMDLL_ProcessGetInformation\n");
	while (true) {
		bool result = VMMDLL_ProcessGetInformation(mem.vHandle, mem.current_process.PID, &ProcessInformation, &cbProcessInformation);
		if (result) {
			printf("SUCCESS: VMMDLL_ProcessGetInformation\n");
			printf("         win.vaEPROCESS = 0x%08x\n", ProcessInformation.win.vaEPROCESS);

			uint64_t section_base_address = mem.Read<uint64_t>((ProcessInformation.win.vaEPROCESS + 0x520), 4); // 4 is pid of system
			std::cout << section_base_address << std::endl;
			uint64_t cr3 = GetProcessCr3(section_base_address);
			std::cout << cr3 << std::endl;
			VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_PROCESS_DTB | mem.current_process.PID, cr3);
		}

	}
}