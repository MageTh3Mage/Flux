#pragma once
#include "imgui.h"

namespace settings {
    inline bool v_sync = false; 
    inline bool enableOnScreenESP = false;
    inline bool aimbot = true;    
    inline int fov = 200;  
    inline int aimkeybind = 0x02;  
    inline int smoothing = 5.0f;
    inline int delay = 0;
    inline int offset = 0;
    inline bool TeamCheck = false;
    inline bool VisCheck = true;
    inline bool ClosestBone = false;
    inline bool head = false;
    inline bool neck = false;
    inline bool chest = false;
    inline bool pelvis = false;
}

namespace esp_settings {
    inline bool esp = true;
    inline ImVec4 esp_color = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);  
    inline ImVec4 fov_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  
    inline int skeleton_thickness = 1; 
    inline bool name = true;
    inline bool skeleton = true;
    inline bool distance = true;
    inline bool radar = true;
}