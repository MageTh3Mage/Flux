#include "threads.h"
#include "utils.h"
#include <thread>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;

#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64


__forceinline std::string username(__int64 player)
{
	//int xa = 0;
	//while (true) {
	//	xa = xa + 4;
	//	__int64 Name = mem.Read<__int64>(player + 2712);
	//	if (Name == 0 || xa > 1000000) continue;
	//	int Length = mem.Read<int>(Name + 16i64);
	//	__int64 v6 = Length;
	//	if (Length == 0 || Length > 200 || Length < 0) continue;
	//	std::cout << Length << std::endl;
	//	std::cout << xa << std::endl;
	//	//continue;
	//	if (!v6) continue;
	//	uintptr_t Data = mem.Read<__int64>(Name + 8);
	//	if (!Data) continue;
	//	wchar_t* NameBuffer = new wchar_t[Length];
	//	mem.Read(Data, NameBuffer, (Length * 2));

	//	char v21;
	//	int v22;
	//	int i;
	//	int v25;
	//	_WORD* v23;

	//	v21 = v6 - 1;
	//	if (!(_DWORD)v6)
	//		v21 = 0;
	//	v22 = 0;
	//	v23 = (_WORD*)NameBuffer;
	//	for (i = (v21) & 3; ; *v23++ += i & 7)
	//	{
	//		v25 = v6 - 1;
	//		if (!(_DWORD)v6)
	//			v25 = 0;
	//		if (v22 >= v25)
	//			break;
	//		i += 3;
	//		++v22;
	//	}

	//	std::wstring Temp{ NameBuffer };
	//	delete[] NameBuffer;
	//	std::cout << std::string(Temp.begin(), Temp.end());
	//}

	__int64 Name = mem.Read<__int64>(player + 2712);
	if (!Name) return std::string("bot/AI");
	int Length = mem.Read<int>(Name + 16i64);
	__int64 v6 = Length;
	if (!v6) return std::string("bot/AI");
	uintptr_t Data = mem.Read<__int64>(Name + 8);
	if (!Data) return std::string("bot/AI");
	wchar_t* NameBuffer = new wchar_t[Length];
	mem.Read(Data, NameBuffer, (Length * 2));

	char v21;
	int v22;
	int i;
	int v25;
	_WORD* v23;

	v21 = v6 - 1;
	if (!(_DWORD)v6)
		v21 = 0;
	v22 = 0;
	v23 = (_WORD*)NameBuffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(_DWORD)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring Temp{ NameBuffer };
	delete[] NameBuffer;
	return std::string(Temp.begin(), Temp.end());
}
void nameesp() {
	while (true) {
		if (globals::u_world == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::Gameinstance == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::GameState == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::LocalPlayers == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::PlayerController == (uintptr_t)nullptr) {
			continue;
		}
		for (int i = 0; i < globals::Num; i++) {
			if (cache::players[i] == (uintptr_t)nullptr) continue;
			if (cache::privates[i] == (uintptr_t)nullptr) continue;
			if (cache::privates[i] == globals::LocalPawn) {
				//localTeam = cachedTeam[i];
				continue;
			}
			if (cache::meshes[i] == (uintptr_t)nullptr) continue;
			cache::ign[i] = username(cache::players[i]);
		}
		Sleep(50);
	}
}
void weaponName(int i) {
	if (cache::ftext_length[i] > 0 && cache::ftext_length[i] < 50)
	{
		wchar_t* ftext_buf = new wchar_t[cache::ftext_length[i]];
		mem.Read(cache::ftext_data[i], ftext_buf, cache::ftext_length[i] * sizeof(wchar_t));
		std::wstring wstr_buf(ftext_buf);
		cache::weaponName[i] = std::string(wstr_buf.begin(), wstr_buf.end());
		//return std::string(wstr_buf.begin(), wstr_buf.end());
		delete[] ftext_buf;
	}

}


void esp() {
	if (settings::enableOnScreenESP) {
		ConnectOnScreen("tcp://192.168.1.244:5858");
	};
	while (true) {
		if (globals::u_world == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::Gameinstance == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::GameState == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::LocalPlayers == (uintptr_t)nullptr) {
			continue;
		}
		if (globals::PlayerController == (uintptr_t)nullptr) {
			continue;
		}

		VMMDLL_SCATTER_HANDLE playerHandle = mem.CreateScatterHandle();

		for (uint32_t i = 0; i < globals::Num; i++) {
			mem.AddScatterReadRequest(playerHandle, (globals::PlayerArray + i * 0x8), &cache::players[i], sizeof(uintptr_t));
		}
		mem.ExecuteReadScatter(playerHandle);
		VMMDLL_Scatter_CloseHandle(playerHandle);

		VMMDLL_SCATTER_HANDLE privateHandle = mem.CreateScatterHandle();

		// scatter player private
		for (uint32_t i = 0; i < globals::Num; i++) {
			if (cache::players[i] == (uintptr_t)nullptr) continue;
			mem.AddScatterReadRequest(privateHandle, (cache::players[i] + offsets::Private), &cache::privates[i], sizeof(uintptr_t));
			mem.AddScatterReadRequest(privateHandle, (cache::players[i] + 0x1201), &cache::team[i], sizeof(uintptr_t));
		}
		mem.ExecuteReadScatter(privateHandle);
		VMMDLL_Scatter_CloseHandle(privateHandle);

		VMMDLL_SCATTER_HANDLE meshHandle = mem.CreateScatterHandle();

		// scatter player meshes
		for (uint32_t i = 0; i < globals::Num; i++) {
			if (cache::privates[i] == (uintptr_t)nullptr) continue;
			mem.AddScatterReadRequest(meshHandle, (cache::privates[i] + offsets::Mesh), &cache::meshes[i], sizeof(uintptr_t));
		}
		mem.ExecuteReadScatter(meshHandle);
		VMMDLL_Scatter_CloseHandle(meshHandle);
		//for (int i = 0; i < Num; i++) {
		//	if (cache::Players[i] == (uintptr_t)nullptr) continue;
		//	cache::Ign[i] = (username((uintptr_t)cache::players[i]));
		//}
		VMMDLL_SCATTER_HANDLE arrayHandle = mem.CreateScatterHandle();
		for (int i = 0; i < globals::Num; i++) {
			if (cache::meshes[i] == (uint64_t)nullptr) continue;
			mem.AddScatterReadRequest(arrayHandle, (cache::meshes[i] + offsets::BoneArray), &cache::boneArray[i], sizeof(uint64_t));
			mem.AddScatterReadRequest(arrayHandle, (cache::meshes[i] + offsets::ComponetToWorld), &cache::componentToWorld[i], sizeof(FTransform));
			mem.AddScatterReadRequest(arrayHandle, (cache::privates[i] + 0x9d8), &cache::cachedWd[i], sizeof(uintptr_t)); // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPawn&member=CurrentWeapon
		}
		mem.ExecuteReadScatter(arrayHandle);
		VMMDLL_Scatter_CloseHandle(arrayHandle);
			
		VMMDLL_SCATTER_HANDLE arrayHandle2 = mem.CreateScatterHandle();
		for (int i = 0; i < globals::Num; i++) {
			//std::cout << cache::cachedWd[i] << std::endl;
			if (cache::cachedWd[i] != (uint64_t)nullptr)
			{
				//mem.AddScatterReadRequest(slekeHandle, (cache::cachedWd[i] + 0xedc), &cachedWeaponAmmo[i], sizeof(int32_t));
				mem.AddScatterReadRequest(arrayHandle2, (cache::cachedWd[i] + 0xf1c), &cache::ammo[i], sizeof(int32_t)); // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortWeapon&member=AmmoCount
				mem.AddScatterReadRequest(arrayHandle2, (cache::cachedWd[i] + 0x520), &cache::Weapon[i], sizeof(uint64_t)); // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortWeapon&member=WeaponData
			}
			if (cache::boneArray[i] == (uint64_t)nullptr) mem.AddScatterReadRequest(arrayHandle2, (cache::meshes[i] + offsets::BoneArray), &cache::boneArray[i], sizeof(uint64_t));
		}
		mem.ExecuteReadScatter(arrayHandle2);
		VMMDLL_Scatter_CloseHandle(arrayHandle2);

		VMMDLL_SCATTER_HANDLE slekeHandle = mem.CreateScatterHandle();
		for (int i = 0; i < globals::Num; i++) {
			if (cache::boneArray[i] == (uint64_t)nullptr) continue;
			if (sizeof(cache::componentToWorld[i]) != sizeof(FTransform)) continue;
			if (cache::boneArray[i] != (uint64_t)nullptr) {
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (67 * 0x60), &cache::allBoneTransform[i][0], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (2 * 0x60), &cache::allBoneTransform[i][1], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (66 * 0x60), &cache::allBoneTransform[i][2], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (9 * 0x60), &cache::allBoneTransform[i][3], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (38 * 0x60), &cache::allBoneTransform[i][4], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (10 * 0x60), &cache::allBoneTransform[i][5], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (39 * 0x60), &cache::allBoneTransform[i][6], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (11 * 0x60), &cache::allBoneTransform[i][7], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (40 * 0x60), &cache::allBoneTransform[i][8], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (78 * 0x60), &cache::allBoneTransform[i][9], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (71 * 0x60), &cache::allBoneTransform[i][10], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (79 * 0x60), &cache::allBoneTransform[i][11], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (72 * 0x60), &cache::allBoneTransform[i][12], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (75 * 0x60), &cache::allBoneTransform[i][13], sizeof(FTransform));
				mem.AddScatterReadRequest(slekeHandle, cache::boneArray[i] + (82 * 0x60), &cache::allBoneTransform[i][14], sizeof(FTransform));
				//mem.AddScatterReadRequest(slekeHandle, cachedBoneArray[i] + (0 * 0x60), &cache::allBoneTransform[i][15], sizeof(FTransform));
			}
			
		}
		mem.ExecuteReadScatter(slekeHandle);
		VMMDLL_Scatter_CloseHandle(slekeHandle);


		GetCameraInfo();
		closestPlayer::closest_distance_from_center = FLT_MAX;
		int closestPlayerIndex = FLT_MAX;
		for (uint32_t i = 0; i < globals::Num; i++) {
			//std::cout << cache::Weapon[i] << std::endl;
			/*if (cache::ammo[i] == 8) {
				std::cout << cache::ammo[i] << std::endl;
				std::cout << aw << std::endl;

			}*/
			for (int x = 0; x < 15; x++) cache::allBoneW2S[i][x] = Vector3(0, 0, 0);
			if (cache::players[i] == (uintptr_t)nullptr) continue;
			if (cache::privates[i] == (uintptr_t)nullptr) continue;
			if (cache::privates[i] == globals::LocalPawn) {
				//if (cache::tier[i] == 4) {
				//	std::cout << p << std::endl;
				//	p = p - 2;
				//////	std::cout << p << ",";

				//////	/*2601, 3993, 4939, 
				//////		6331, 
				//////		
				//////		9115, 9561, 
				//////		
				//////		10507, 10953, 
				//////		
				//////		11899, 12345,*/
				//}
				////globals::localTeam = cache::team[i];

				//for (int x = 0; x < 10000; x += 4) {
				//	if (mem.Read<float>(cache::cachedWd[i] + x) == 3.f) { // drum gun gravity
				//	    std::cout << std::hex << x << "\n"; // 0x1f34
				//	}
				//	if (mem.Read<float>(cache::cachedWd[i] + x) == 80000.f) { // nemesis ar bullet speed
				//		std::cout << std::hex << x << "\n"; // 0x1cc4
				//		// 0x1f30
				//	}
				//}

				cache::localteam = cache::team[i];
				D3DMATRIX matrix = MatrixMultiplication(cache::allBoneTransform[i][0].ToMatrixWithScale(), cache::componentToWorld[i].ToMatrixWithScale());
				Vector3 bonePositions[15] = {};
				bonePositions[0] = Vector3(matrix._41, matrix._42, matrix._43);
				//bonePositions[15] = Vector3(matrix2._41, matrix2._42, matrix2._43);
				Vector3 Headpos = bonePositions[0];

				cache::localheadpos = Headpos;

				continue;
			}
			
			if (cache::team[i] == globals::localTeam) continue;
			if (cache::meshes[i] == (uintptr_t)nullptr) continue;
			

			D3DMATRIX matrix = MatrixMultiplication(cache::allBoneTransform[i][0].ToMatrixWithScale(), cache::componentToWorld[i].ToMatrixWithScale());
			Vector3 bonePositions[15] = {};
			bonePositions[0] = Vector3(matrix._41, matrix._42, matrix._43);
			//bonePositions[15] = Vector3(matrix2._41, matrix2._42, matrix2._43);
			Vector3 Headpos = bonePositions[0];
			cache::headpos[i] = Headpos;
			Vector3 Headbox = ProjectWorldToScreen({ Headpos.x, Headpos.y, Headpos.z + 20 });
			cache::distances[i] = viewInfo.location.Distance(Headpos) / 100;

			if (Headbox.x < -20 || Headbox.y < -100 || Headbox.x > 1940 || Headbox.y > 1100) continue;



			for (int x = 0; x < 15; x++) {
				D3DMATRIX matrix = MatrixMultiplication(cache::allBoneTransform[i][x].ToMatrixWithScale(), cache::componentToWorld[i].ToMatrixWithScale());
				if (x == 2) {
					bonePositions[x] = Vector3(matrix._41, matrix._42, matrix._43 - 10);
				}
				else {
					bonePositions[x] = Vector3(matrix._41, matrix._42, matrix._43);
				}
			}

			D3DMATRIX tempMatrix = Matrix(viewInfo.rotation);
			Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
			Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
			Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

			//Vector3 cache::allBoneW2S[currentPlayerBones][15] = {};
			for (int currentBone = 0; currentBone < 15; currentBone++) {
				Vector3 vDelta = bonePositions[currentBone] - viewInfo.location;
				Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
				if (vTransformed.z < 1.f)
					vTransformed.z = 1.f;

				cache::allBoneW2S[i][currentBone] = Vector3((1920 / 2.0f) + vTransformed.x * (((1920 / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, (1080 / 2.0f) - vTransformed.y * (((1920 / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, 0);
			}
			std::string string = "";
			for (int currentBone = 0; currentBone < 15; currentBone++) {
				string = string + std::to_string(cache::allBoneW2S[i][currentBone].x) + ",";
				string = string + std::to_string(cache::allBoneW2S[i][currentBone].y) + ",";
			}
			int o = 1;
			if (IsVisible(cache::meshes[i]) == true) o = 1;
			else o = 0;
			string = string + std::to_string(cache::distances[i]) + "," + std::to_string(o) + ",";

			if (settings::enableOnScreenESP) {
				std::thread OnScreen(SendOnScreen, string);
				OnScreen.detach();
			};
			if (cache::localteam == cache::team[i]) continue;
			float dx = Headbox.x - 960;
			float dy = Headbox.y - 540;
			float dist = sqrtf(dx * dx + dy * dy);

			if (dist < settings::fov && dist < closestPlayer::closest_distance_from_center && closestPlayer::closest_player_private == 0) {
				closestPlayerIndex = i;
				closestPlayer::closest_distance_from_center = dist;
			}
		}
		if (closestPlayer::closest_distance_from_center < settings::fov && closestPlayer::closest_player_private == 0) {
			closestPlayer::closest_player_distance = cache::distances[closestPlayerIndex];
			closestPlayer::closest_player_private = cache::privates[closestPlayerIndex];
			closestPlayer::closest_player_mesh = cache::meshes[closestPlayerIndex];
			closestPlayer::root_component = mem.Read<uintptr_t>(closestPlayer::closest_player_private + offsets::RootComponent);
		}
		
	}
}

void getAllWeaponNames() {
	while (true) {
		VMMDLL_SCATTER_HANDLE ftext_ptrHandle = mem.CreateScatterHandle();
		for (uint32_t i = 0; i < globals::Num; i++) {
			if (cache::Weapon[i] == (uintptr_t)nullptr) continue;
			uint64_t wdata = cache::Weapon[i];
			mem.AddScatterReadRequest(ftext_ptrHandle, (wdata + 0xa2), &cache::tier[i], sizeof(BYTE)); // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UFortItemDefinition&member=Rarity
			mem.AddScatterReadRequest(ftext_ptrHandle, (wdata + 0x40), &cache::ftext_ptr[i], sizeof(uint64_t));
		}
		mem.ExecuteReadScatter(ftext_ptrHandle);
		VMMDLL_Scatter_CloseHandle(ftext_ptrHandle);

		VMMDLL_SCATTER_HANDLE ftext_dataHandle = mem.CreateScatterHandle();
		for (uint32_t i = 0; i < globals::Num; i++) {
			if (cache::ftext_ptr[i] == (uintptr_t)nullptr) continue;
			mem.AddScatterReadRequest(ftext_dataHandle, (cache::ftext_ptr[i] + 0x28), &cache::ftext_data[i], sizeof(uint64_t));
			mem.AddScatterReadRequest(ftext_dataHandle, (cache::ftext_ptr[i] + 0x30), &cache::ftext_length[i], sizeof(int));
		}
		mem.ExecuteReadScatter(ftext_dataHandle);
		VMMDLL_Scatter_CloseHandle(ftext_dataHandle);
		VMMDLL_SCATTER_HANDLE bulletHandle = mem.CreateScatterHandle();
		for (uint32_t i = 0; i < globals::Num; i++) {
			if (cache::privates[i] == globals::LocalPawn) {
				uintptr_t wdata = mem.Read<uintptr_t>(cache::privates[i] + 0x9d8);
				mem.AddScatterReadRequest(bulletHandle, (wdata + 0x1cc4), &cache::bulletSpeedNew, sizeof(float));
				mem.AddScatterReadRequest(bulletHandle, (wdata + 0x1f34), &cache::bulletGravityNew, sizeof(float));

				//std::cout << static_cast<int>(mem.Read<float>(cache::cachedWd[i] + 0x1aa0));
			}
			weaponName(i);
		}
		mem.ExecuteReadScatter(bulletHandle);
		VMMDLL_Scatter_CloseHandle(bulletHandle);
		cache::bulletSpeed = cache::bulletSpeedNew;
		cache::bulletGravity = cache::bulletGravityNew;
		//std::cout << cache::bulletGravity << " " << cache::bulletSpeed << std::endl;
		Sleep(300);
	}
}



