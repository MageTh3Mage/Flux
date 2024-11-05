#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include "offsets.h"
#include "globals.h"
#include "Memory.h"

inline CameraInfo viewInfo = {};
void get_view_point();
Vector3 ProjectWorldToScreen(Vector3 WorldLocation);
Vector3 GetBoneWithRotation(uintptr_t mesh, int bone_id);
bool IsVisible(uintptr_t mesh);

#endif // UTILS_H