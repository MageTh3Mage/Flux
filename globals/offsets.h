#pragma once

#include <windows.h>
namespace offsets {
    // Offsets from v31.50

    inline uintptr_t Uworld = 0x1221B738;
    inline uintptr_t Viewpoint = 0x10F90A10;
    inline uintptr_t OwningWorld = 0xC0;
    inline uintptr_t GameInstance = 0x1D8;
    inline uintptr_t LocalPlayers = 0x38;
    inline uintptr_t GNames = 0x123FAFc0;
    inline uintptr_t GObjects = 0x122A6A80;

    inline uintptr_t PlayerController = 0x30;
    inline uintptr_t LocalPawn = 0x338;
    inline uintptr_t PlayerState = 0x2B0;
    inline uintptr_t RootComponent = 0x198;
    inline uintptr_t GameState = 0x160;
    inline uintptr_t PersistentLevel = 0x30;
    inline uintptr_t LastSubmitTime = 0x2E8;
    inline uintptr_t LastRenderTimeOnScreen = 0x2E0;
    inline uintptr_t WeaponData = 0x520;
    inline uintptr_t skeletal_mesh = 0x310;
    inline uintptr_t ActorCount = 0xA8;
    inline uintptr_t CameraManager = 0x348;
    inline uintptr_t AActor = 0xA0; // Same as Actor
    inline uintptr_t CurrentActor = 0xA8;
    inline uintptr_t Mesh = 0x310;
    inline uintptr_t Revivefromdbnotime = 0x4C30;
    inline uintptr_t TeamId = 0x1211; //might be 0x1201
    inline uintptr_t ActorTeamId = 0x10E0;
    inline uintptr_t IsDBNO = 0x93A;
    inline uintptr_t LocalActorPos = 0x120;
    inline uintptr_t ComponetToWorld = 0x1C0;
    inline uintptr_t BoneArray = 0x570;
    inline uintptr_t BoneCache = 0x5B8;
    inline uintptr_t Velocity = 0x168;
    inline uintptr_t Private = 0x308; // Same as PawnPrivate
    inline uintptr_t PlayerArray = 0x2A8;
    inline uintptr_t relativelocation = 0x120;
    inline uintptr_t UCharacterMovementComponent = 0x318;
    inline uintptr_t entity_actor = 0x310; // Same as ActorMesh
    inline uintptr_t bIsReloadingWeapon = 0x388;
    inline uintptr_t GlobalAnimRateScale = 0xA18;
    inline uintptr_t Wireframe = 0x194;
    inline uintptr_t SkeletalMeshes = 0x56E;
    inline uintptr_t PawnMaterials_ALL = 0x5A60;
}
