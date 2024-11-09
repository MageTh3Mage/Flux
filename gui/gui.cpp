#include "gui.h"
#include <windows.h>

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
bool                     g_SwapChainOccluded = false;
UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

const char* GetKeyName(int key) {
    switch (key) {
    case 0x01: return "Left Mouse Button";
    case 0x02: return "Right Mouse Button";
    case 0x04: return "Middle Mouse Button";
    case 0x05: return "Mouse X1";
    case 0x06: return "Mouse X2";
    default:
        static char keyName[128];
        GetKeyNameTextA(MapVirtualKeyA(key, MAPVK_VK_TO_VSC) << 16, keyName, 128); // Keyboard keys
        return keyName;
    }
}

static bool waiting_for_key = false;

void SetupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Border] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);

    // Adjust padding
    style.WindowPadding = ImVec2(20, 30);
    style.FramePadding = ImVec2(0, 0);
    style.ItemSpacing = ImVec2(10, 8);

    // Adjust window rounding and border size
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.f;
    style.ChildBorderSize = 0.f;

    // Set colors for the theme
    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.f, 0.f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.f, 0.f, 0.f, 1.00f);

    // Apply RGB (0, 179, 255) as ImVec4(0.0f, 0.7f, 1.0f)
    colors[ImGuiCol_CheckMark] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.7f, 0.0f, 1.0f, 1.00f);
}

const double PI = 3.14159265358979323846;

std::pair<int, int> rotatePoint(int angle, int x, int y) {
    // Center anchor point of a 350x350 box
    int centerX = 175;
    int centerY = 175;

    // Convert the angle from degrees to radians
    double radians = angle * PI / 180.0;

    // Translate the point to origin
    int translatedX = x - centerX;
    int translatedY = y - centerY;

    // Apply the rotation matrix
    int rotatedX = static_cast<int>(translatedX * cos(radians) - translatedY * sin(radians) + 0.5);
    int rotatedY = static_cast<int>(translatedX * sin(radians) + translatedY * cos(radians) + 0.5);

    // Translate the rotated point back to the center anchor
    int newX = rotatedX + centerX;
    int newY = rotatedY + centerY;

    return { newX, newY };
}

void ShowMenuWithSidebar()
{
    ImGui::SetNextWindowSize(ImVec2(650, 340));
    ImGui::Begin("Flux DMA", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    if (!globals::CR3Fixed) {

    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(1920, 1080), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)));
    }

    // Sidebar child window
    ImGui::BeginChild("Sidebar", ImVec2(165, 0), true);
    static int current_tab = 0; // Track current selected tab

    // Calculate the total height needed for buttons and spacing
    float button_height = 50;
    float total_button_height = 4 * button_height + 3 * ImGui::GetStyle().ItemSpacing.y;

    // Calculate the position to center the buttons
    float available_height = ImGui::GetContentRegionAvail().y;
    float offset_y = (available_height - total_button_height) / 2;
    if (offset_y > 0)
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);

    // Sidebar buttons with equal spacing
    if (ImGui::Button("Aim", ImVec2(150, 50))) current_tab = 0;
    ImGui::Spacing();
    if (ImGui::Button("ESP", ImVec2(150, 50))) current_tab = 1;
    ImGui::Spacing();
    if (ImGui::Button("Colors", ImVec2(150, 50))) current_tab = 2;
    ImGui::Spacing();
    if (ImGui::Button("Misc", ImVec2(150, 50))) current_tab = 3;

    ImGui::EndChild();

    // Align content next to the sidebar
    ImGui::SameLine();

    // Content child window
    ImGui::BeginChild("Content", ImVec2(0, 0), true);

    // Menu content based on the selected tab
    switch (current_tab)
    {
    case 0: // Aim settings
        ImGui::Text("Aimbot");
        ImGui::Spacing();
        ImGui::Checkbox("Enable Aimbot", &settings::aimbot);
        ImGui::Spacing();
        if (settings::aimbot) {
            ImGui::SliderInt(" Smoothing", &settings::smoothing, 1, 50);  // Uniform label
            ImGui::Spacing();
            ImGui::SliderInt(" Delay (ms)", &settings::delay, 0, 15000);  // Uniform label
            ImGui::Spacing();
            ImGui::SliderInt(" FOV", &settings::fov, 0, 2000);            // Uniform label and corrected range
            ImGui::Spacing();
            ImGui::SliderInt(" Offset", &settings::offset, -100, 15);           // Uniform label
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Checkbox(" Visual Check", &settings::VisCheck);
            ImGui::Spacing();
            ImGui::Checkbox(" Closest Bone", &settings::ClosestBone);
            ImGui::Spacing();
            ImGui::Checkbox(" Team Check", &settings::TeamCheck);
            ImGui::Text(" Aim Key: ");
            ImGui::SameLine();
            if (ImGui::Button(waiting_for_key ? "Press any key..." : GetKeyName(settings::aimkeybind))) {
                waiting_for_key = true; // Start waiting for key press
            }
        }
        // If waiting for key, capture the next key press (including mouse buttons)
        if (waiting_for_key) {
            // Iterate over all possible virtual key codes, including mouse buttons
            for (int key = 0x01; key <= 0xFE; ++key) {
                if (GetAsyncKeyState(key) & 0x8000) {
                    settings::aimkeybind = key;  // Assign the pressed key to the aimkeybind
                    waiting_for_key = false;     // Stop waiting for key
                }
            }
        }
        break;

    case 1: // ESP settings
        ImGui::Text("ESP Settings");
        ImGui::Spacing();
        ImGui::Checkbox("Enable ESP", &esp_settings::esp);
        ImGui::Spacing();

        if (esp_settings::esp) { 
            ImGui::Checkbox("Radar", &esp_settings::radar);
            ImGui::Spacing();
            ImGui::Checkbox("Skeleton ESP", &esp_settings::skeleton);
            ImGui::Spacing();
            ImGui::Checkbox("Distances", &esp_settings::distance);
            ImGui::Spacing();
            ImGui::Checkbox("Names", &esp_settings::name);
        }
        break;

    case 2: // Color settings
        ImGui::Text("Color Settings");
        ImGui::Spacing();
        ImGui::ColorEdit3("ESP Color", (float*)&esp_settings::esp_color);  // Color setting for ESP
        ImGui::Spacing();
        ImGui::ColorEdit3("Aimbot FOV Color", (float*)&esp_settings::fov_color);  // Color setting for Aimbot FOV
        break;

    case 3: // Miscellaneous settings
        ImGui::Text("Miscellaneous Settings");
        // Add your miscellaneous settings here
        break;
    }




	//ImGui::EndChild();
 //   ImGui::SameLine();
 //   ImGui::BeginChild("Content2", ImVec2(10, 0), true); // Content child window
 //   ImGui::EndChild();
 //   ImGui::SameLine();
 //   ImGui::BeginChild("Side", ImVec2(220, 0), true); // Content child window
 //   ImGui::Text("Target Bone");
 //   ImGui::Spacing();
 //   if (ImGui::BeginTable("CheckboxTable", 2)) {
 //       // First row
 //       ImGui::TableNextRow();
 //       ImGui::TableNextColumn();
 //       ImGui::Checkbox(" Head Bone", &settings::head); // 1st checkbox
 //       ImGui::TableNextColumn();
 //       ImGui::Checkbox(" Neck Bone", &settings::neck); // 2nd checkbox

 //       // Second row
 //       ImGui::TableNextRow();
 //       ImGui::TableNextRow();
 //       ImGui::TableNextRow();
 //       ImGui::TableNextRow();
 //       ImGui::TableNextRow();
 //       ImGui::TableNextRow();
 //       ImGui::TableNextColumn();
 //       ImGui::Checkbox("Chest Bone", &settings::chest); // 3rd checkbox
 //       ImGui::TableNextColumn();
 //       ImGui::Checkbox("Pelvis Bone", &settings::pelvis); // 4th checkbox

 //       ImGui::EndTable();
 //   }
 //   ImGui::Spacing();
 //   ImGui::Text("Aimbot Keybind");
 //   ImGui::Spacing();
 //   ImGui::Button("VK_RBUTTON", ImVec2(100, 30));

    ImGui::EndChild();
    ImGui::End();

    
   
    //int radarX = 1450;
    //int radarY = 300;
    int skeletonespthickness = esp_settings::skeleton_thickness;        
    ImVec4 skeletonColor = esp_settings::esp_color; // Use ESP color
    ImVec4 skeletonColor2 = ImVec4(1, 0, 0, 1); // Use ESP color
    ImVec4 textColor = ImVec4(1, 1, 1, 1); // Use ESP color
    if (esp_settings::esp) {
        for (uint32_t i = 0; i < globals::Num; i++) {
            //if (cache::privates[i] == globals::LocalPawn || cache::headpos[i].x == 0 || cache::headpos[i].y == 0) continue;
            if (cache::allBoneW2S[i][0].x == 0 && cache::allBoneW2S[i][1].x == 0 && cache::allBoneW2S[i][2].x == 0 && cache::allBoneW2S[i][14].x == 0) continue;
            //if (cache::players[i] == (uintptr_t)nullptr) continue;
            //if (cache::privates[i] == (uintptr_t)nullptr) continue;
            //if (cache::privates[i] == globals::LocalPawn) continue;
            //if (cache::team[i] == globals::localTeam) continue;
            //if (cache::meshes[i] == (uintptr_t)nullptr) continue;
            if (esp_settings::skeleton && cache::isVis[i]) {
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][0].x, cache::allBoneW2S[i][0].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][3].x, cache::allBoneW2S[i][3].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][4].x, cache::allBoneW2S[i][4].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][5].x, cache::allBoneW2S[i][5].y), ImVec2(cache::allBoneW2S[i][3].x, cache::allBoneW2S[i][3].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][6].x, cache::allBoneW2S[i][6].y), ImVec2(cache::allBoneW2S[i][4].x, cache::allBoneW2S[i][4].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][5].x, cache::allBoneW2S[i][5].y), ImVec2(cache::allBoneW2S[i][7].x, cache::allBoneW2S[i][7].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][6].x, cache::allBoneW2S[i][6].y), ImVec2(cache::allBoneW2S[i][8].x, cache::allBoneW2S[i][8].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][10].x, cache::allBoneW2S[i][10].y), ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][9].x, cache::allBoneW2S[i][9].y), ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][12].x, cache::allBoneW2S[i][12].y), ImVec2(cache::allBoneW2S[i][10].x, cache::allBoneW2S[i][10].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][11].x, cache::allBoneW2S[i][11].y), ImVec2(cache::allBoneW2S[i][9].x, cache::allBoneW2S[i][9].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][13].x, cache::allBoneW2S[i][13].y), ImVec2(cache::allBoneW2S[i][12].x, cache::allBoneW2S[i][12].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][14].x, cache::allBoneW2S[i][14].y), ImVec2(cache::allBoneW2S[i][11].x, cache::allBoneW2S[i][11].y), ImGui::GetColorU32(skeletonColor), skeletonespthickness);
            }
            if (esp_settings::skeleton && !cache::isVis[i]) {
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][0].x, cache::allBoneW2S[i][0].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][3].x, cache::allBoneW2S[i][3].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][4].x, cache::allBoneW2S[i][4].y), ImVec2(cache::allBoneW2S[i][2].x, cache::allBoneW2S[i][2].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][5].x, cache::allBoneW2S[i][5].y), ImVec2(cache::allBoneW2S[i][3].x, cache::allBoneW2S[i][3].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][6].x, cache::allBoneW2S[i][6].y), ImVec2(cache::allBoneW2S[i][4].x, cache::allBoneW2S[i][4].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][5].x, cache::allBoneW2S[i][5].y), ImVec2(cache::allBoneW2S[i][7].x, cache::allBoneW2S[i][7].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][6].x, cache::allBoneW2S[i][6].y), ImVec2(cache::allBoneW2S[i][8].x, cache::allBoneW2S[i][8].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][10].x, cache::allBoneW2S[i][10].y), ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][9].x, cache::allBoneW2S[i][9].y), ImVec2(cache::allBoneW2S[i][1].x, cache::allBoneW2S[i][1].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][12].x, cache::allBoneW2S[i][12].y), ImVec2(cache::allBoneW2S[i][10].x, cache::allBoneW2S[i][10].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][11].x, cache::allBoneW2S[i][11].y), ImVec2(cache::allBoneW2S[i][9].x, cache::allBoneW2S[i][9].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][13].x, cache::allBoneW2S[i][13].y), ImVec2(cache::allBoneW2S[i][12].x, cache::allBoneW2S[i][12].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(cache::allBoneW2S[i][14].x, cache::allBoneW2S[i][14].y), ImVec2(cache::allBoneW2S[i][11].x, cache::allBoneW2S[i][11].y), ImGui::GetColorU32(skeletonColor2), skeletonespthickness);
            }
            if (esp_settings::distance) {
                char distanceStr[64];
                snprintf(distanceStr, sizeof(distanceStr), "[%0.fm]", cache::distances[i]);
                ImGui::GetBackgroundDrawList()->AddText({ float(cache::allBoneW2S[i][0].x - (ImGui::CalcTextSize(distanceStr)[0] / 2)), float(cache::allBoneW2S[i][14].y + ImGui::CalcTextSize(distanceStr)[1]/2) }, ImGui::GetColorU32(textColor), distanceStr);
            }
            if (esp_settings::name) {
                char charArray[50];
                strcpy_s(charArray, cache::ign[i].c_str());
                ImGui::GetBackgroundDrawList()->AddText({ float(cache::allBoneW2S[i][0].x - (ImGui::CalcTextSize(charArray)[0] / 2)), float(cache::allBoneW2S[i][0].y - ImGui::CalcTextSize(charArray)[1]) }, ImGui::GetColorU32(textColor), charArray);
                
                ImVec4 tierColor;
                int tier = static_cast<int>(cache::tier[i]);
                //Common = 0,
                //Uncommon = 1,
                //Rare = 2,
                //Epic = 3,
                //Legendary = 4,
                //Mythic = 5,
                //Transcendent = 6,
                //Unattainable = 7,
                //NumRarityValues = 8,
                //EFortRarity_MAX = 9
                if (tier == 0) tierColor = ImVec4(0.7, 0.7, 0.7, 1);
                else if (tier == 1) tierColor = ImVec4(0, 1, 0, 1);
                else if (tier == 2) tierColor = ImVec4(0, 0.3, 1, 1);
                else if (tier == 3) tierColor = ImVec4(0, 1, 1, 1);
                else if (tier == 4) tierColor = ImVec4(1, 1, 0, 1);
                else if (tier == 5) tierColor = ImVec4(0.5, 1, 0, 1);
                else if (tier == 6) tierColor = ImVec4(0, 1, 1, 1);
                else tierColor = ImVec4(1, 1, 1, 1);
                char charArray2[50];
                strcpy_s(charArray2, cache::weaponName[i].c_str());
                ImGui::GetBackgroundDrawList()->AddText({ float(cache::allBoneW2S[i][0].x - (ImGui::CalcTextSize(charArray2)[0] / 2)), float(cache::allBoneW2S[i][0].y - ImGui::CalcTextSize(charArray)[1] - ImGui::CalcTextSize(charArray2)[1]) }, ImGui::GetColorU32(tierColor), charArray2);
            }
            if (esp_settings::platform) {
                std::wstring balls_sus(cache::platform[i]);
                std::string str_platform(balls_sus.begin(), balls_sus.end());
                //std::cout << str_platform << std::endl;
                float posX = float(cache::allBoneW2S[i][0].x - (ImGui::CalcTextSize(str_platform.c_str()).x / 2));
                float posY = float(cache::allBoneW2S[i][14].y + ImGui::CalcTextSize(str_platform.c_str()).y / 2) + 20;
                ImVec4 color;

                if (str_platform.find("XBL") != std::string::npos || str_platform.find("XSX") != std::string::npos) {
                    color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
                }
                else if (str_platform.find("PSN") != std::string::npos || str_platform.find("PS4/PS5") != std::string::npos) {
                    color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
                }
                else if (str_platform.find("WIN") != std::string::npos) {
                    color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Sky Blue
                }
                else if (str_platform.find("SWH") != std::string::npos) {
                    color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
                }
                //else {
                //    return; // No valid platform found, exit early
                //}

                ImGui::GetBackgroundDrawList()->AddText({ posX, posY }, ImGui::GetColorU32(color), str_platform.c_str());
            }

        }

        if (esp_settings::radar) {
            ImGui::SetNextWindowSize(ImVec2(350, 350));
            ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(ImGui::GetWindowPos().x + 175, ImGui::GetWindowPos().y + 175), 3, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));

            for (uint32_t i = 0; i < globals::Num; i++) {

                //if (cache::privates[i] == globals::LocalPawn || cache::headpos[i].x == 0 || cache::headpos[i].y == 0) continue;
                //char distanceStrRadar[64];
                //snprintf(distanceStrRadar, sizeof(distanceStrRadar), "%0.fm", cache::distances[i]);
                std::pair<int, int> newPoint = rotatePoint(-viewInfo.rotation.y, int((cache::localheadpos.y - cache::headpos[i].y) / 100) + 175, int((cache::headpos[i].x - cache::localheadpos.x) / 100) + 175);
                ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(ImGui::GetWindowPos().x - newPoint.first + 350, ImGui::GetWindowPos().y - newPoint.second + 350), 3, ImGui::GetColorU32(ImVec4(1, 0, 0, 1)));
                //ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x - newPoint.first + 350 - (ImGui::CalcTextSize(distanceStrRadar)[0] / 2), ImGui::GetWindowPos().y - newPoint.second + 350 - (ImGui::CalcTextSize(distanceStrRadar)[1] * 1.5)), ImGui::GetColorU32(skeletonColor), distanceStrRadar);
                
            }

            ImGui::End();
        }

        
    }
   

    //for (Vector3 head : cache::headpos) {
    //for (uint32_t i = 0; i < globals::Num; i++) {
    //}
    float fps = ImGui::GetIO().Framerate;
    char fpsText[64];
    snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", fps);
    ImGui::GetBackgroundDrawList()->AddCircle({ float(1920 / 2), float(1080 / 2) }, settings::fov, ImGui::GetColorU32(esp_settings::fov_color), 100.f, 1.f);

    // Set the position for the FPS counter
    ImVec2 position = ImVec2(10.0f, 10.0f); // Top-left corner

    // Set the color for the FPS text
    ImColor color = ImColor(255, 255, 255, 255); // White

    // Draw the FPS text on the background
    ImGui::GetBackgroundDrawList()->AddText(position, color, fpsText);

    // Position for the CR3 status text
    ImVec2 cr3StatusPosition = ImVec2(10.0f, 30.0f); // Below the FPS text

    // Determine the status text and color based on cr3Fixed
    const char* cr3StatusText = globals::CR3Fixed ? "Mystic" : "Mystic";
    ImColor cr3StatusColor = globals::CR3Fixed ? ImColor(0, 255, 0, 255) : ImColor(255, 0, 0, 255); // Green for fixed, red for not fixed

    // Draw the CR3 status text
    ImGui::GetBackgroundDrawList()->AddText(cr3StatusPosition, cr3StatusColor, cr3StatusText);

}



void GuiLoop()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"FluxDMA", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Flux DMA", WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr);

    ::SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ::SetWindowPos(hwnd, nullptr, 0, 0, screenWidth, screenHeight, SWP_NOZORDER | SWP_FRAMECHANGED);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\verdanab.ttf", 13.0f);
    io.Fonts->AddFontFromMemoryTTF(fontFortnite, sizeof(fontFortnite), 14.0f);

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.00f);
    SetupImGuiStyle();
    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        bool menu = true;
        if (menu) ShowMenuWithSidebar();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(0, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}