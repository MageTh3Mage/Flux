#pragma once
#include "imgui.h"

namespace settings {
    inline bool v_sync = false; 
    inline bool enableOnScreenESP = false;
    inline bool aimbot = true;    
    inline int fov = 400;  
    inline int aimkeybind = 0x05;  
    inline int smoothing = 5.0f;
    inline int delay = 0;
    inline int offset = 0;
    inline bool TeamCheck = true;
    inline bool VisCheck = true;
    inline bool ClosestBone = false;
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
    inline bool platform = true;
}
