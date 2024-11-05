#pragma once

#include <windows.h>
namespace offsets {
    // Offsets from v32.00
    inline uintptr_t Uworld = 0x12DA3A98;
    inline uintptr_t GameInstance = 0x208;

    inline uintptr_t AActor = 0xA0; // Same as Actor
    inline uintptr_t ActorCount = 0xA8;
    inline uintptr_t ActorTeamId = 0x10E0;
    inline uintptr_t BoneArray = 0x580;
    inline uintptr_t BoneCache = 0x590;
    inline uintptr_t bIsReloadingWeapon = 0x398;
    inline uintptr_t CameraLocation = 0x130;
    inline uintptr_t CameraManager = 0x348;
    inline uintptr_t CameraRotation = 0x140;
    inline uintptr_t ComponetToWorld = 0x1c0;
    inline uintptr_t CurrentActor = 0xA8;
    inline uintptr_t entity_actor = 0x328; // Same as ActorMesh
    inline uintptr_t GameState = 0x190;
    inline uintptr_t GlobalAnimRateScale = 0xA18;
    inline uintptr_t IsDBNO = 0x93A;

    inline uintptr_t LastRenderTimeOnScreen = 0x2F0;  ////////////////////////////
    inline uintptr_t LastSubmitTime = 0x2EC;           ////////////////////////////

    inline uintptr_t LocalActorPos = 0x120;
    inline uintptr_t LocalPawn = 0x350;
    inline uintptr_t LocalPlayers = 0x38;
    inline uintptr_t Mesh = 0x328;
    inline uintptr_t OwningWorld = 0xC0;
    inline uintptr_t PawnMaterials_ALL = 0x5A60;
    inline uintptr_t PersistentLevel = 0x40;
    inline uintptr_t Platform = 0x408;
    inline uintptr_t PlayerArray = 0x2c0;
    inline uintptr_t PlayerController = 0x30;
    inline uintptr_t PlayerState = 0x2B0;
    inline uintptr_t Private = 0x320; // Same as PawnPrivate
    inline uintptr_t Revivefromdbnotime = 0x4BE8;
    inline uintptr_t RootComponent = 0x1B0;
    inline uintptr_t skeletal_mesh = 0x310;
    inline uintptr_t SkeletalMeshes = 0x56E;

    inline uintptr_t TargetedFortPawn = 0x18e8; //Triggerbot Offset

    inline uintptr_t TeamId = 0x1239;
    inline uintptr_t UCharacterMovementComponent = 0x318;
    inline uintptr_t Velocity = 0x168;
    inline uintptr_t Viewpoint = 0x10F90A10;

    ///////////////////// WEAPON RELATED OFFSETS
    inline uintptr_t CurrentWeapon = 0x9F8;
    inline uintptr_t WeaponData = 0x548;
    inline uintptr_t AmmoCount = 0xF0C;
    inline uintptr_t Rarity = 0xA2; //same as "Tier"
    inline uintptr_t ItemName = 0x40;
    //////////////////////

    inline uintptr_t Wireframe = 0x194;
    inline uintptr_t relativelocation = 0x120;
}
