#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint> // For uintptr_t

namespace globals {
    inline __int64 va_text = 0;    
    inline uintptr_t u_world = 0;  
    inline uintptr_t Gameinstance = 0; 
    inline uintptr_t LocalPlayers = 0; 
    inline uintptr_t GameState = 0; 
    inline uintptr_t Localplayer = 0;  
    inline uintptr_t PlayerController = 0;  
    inline uintptr_t LocalPawn = 0; 
    inline uintptr_t PlayerArray = 0;  
    inline uintptr_t localTeam = 0;
    inline uintptr_t currentWeapon;
    inline uintptr_t bulletSpeed;
    inline uintptr_t bulletGravity;
    inline int Num = 0;  
    inline uintptr_t location_pointer = 0;  
    inline uintptr_t rotation_pointer = 0; 
    inline bool readGlobals = true;  
    inline bool CR3Fixed = false;
    inline uint64_t cached_dtb;
}

namespace closestPlayer {
    inline uintptr_t closest_player_private = 0;
    inline uintptr_t closest_player_mesh = 0;
    inline int index = 0;
    inline uintptr_t root_component = 0;
    inline int closest_player_distance = FLT_MAX;
    inline float closest_distance_from_center = FLT_MAX;
}

#endif