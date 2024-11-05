#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include "structs.h"

namespace cache{
	inline uintptr_t players[1000] = {};
	inline uintptr_t privates[1000] = {};
	inline uintptr_t team[1000];
	inline uintptr_t localteam = 0;
	inline Vector3 localheadpos = {};
	inline uintptr_t meshes[1000] = {};
	inline uintptr_t boneArray[1000] = {};
	inline FTransform componentToWorld[1000] = {};
	inline uintptr_t cachedWd[1000] = {};
	inline uintptr_t Weapon[1000] = {};
	inline uint64_t ftext_ptr[1000] = {};
	inline uint64_t ftext_data[1000] = {};
	inline uint64_t testPlatform[1000] = {};
	inline wchar_t platform[1000][64] = {};
	inline int ftext_length[1000] = {};
	inline std::string weaponName[1000] = {};
	inline int32_t ammo[1000] = {};
	inline BYTE tier[1000] = {};
	inline Vector3 headpos[1000] = {};
	inline float distances[1000] = {};
	inline float bulletSpeed;
	inline float bulletGravity;
	inline float bulletSpeedNew;
	inline float bulletGravityNew;
	inline std::string ign[1000] = {};
	inline std::vector<std::vector<FTransform>> allBoneTransform(1000, std::vector<FTransform>(15));
	inline std::vector<std::vector<Vector3>> allBoneW2S(1000, std::vector<Vector3>(15));
}
