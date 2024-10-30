#pragma once
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "cache.h"
#include "Memory.h"
#include "globals.h"
#include "settings.h"
#include "utils.h"
#include "font.h"

extern void GuiLoop();
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

