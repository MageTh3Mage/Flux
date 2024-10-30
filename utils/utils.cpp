#include "utils.h"
#include "structs.h"
#include "offsets.h"
#include "globals.h"
#include "Memory.h"

void GetCameraInfo()
{
	CameraInfo camera;
	VMMDLL_SCATTER_HANDLE scatterHandle2 = mem.CreateScatterHandle();
	mem.AddScatterReadRequest(scatterHandle2, globals::rotation_pointer, &rotationInfo.pitch, sizeof(double));
	mem.AddScatterReadRequest(scatterHandle2, globals::rotation_pointer + 0x20, &rotationInfo.yaw, sizeof(double));
	mem.AddScatterReadRequest(scatterHandle2, globals::rotation_pointer + 0x1d0, &rotationInfo.roll, sizeof(double));
	mem.AddScatterReadRequest(scatterHandle2, globals::location_pointer, &camera.location, sizeof(Vector3));
	mem.AddScatterReadRequest(scatterHandle2, globals::PlayerController + 0x394, &camera.fov, sizeof(float));
	mem.ExecuteReadScatter(scatterHandle2);
	camera.fov = camera.fov * 90.f;


	camera.rotation.x = asin(rotationInfo.roll) * (180.0 / M_PI);
	camera.rotation.y = ((atan2(rotationInfo.pitch * -1, rotationInfo.yaw) * (180.0 / M_PI)) * -1) * -1;

	viewInfo = camera;
}

Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{

	D3DMATRIX tempMatrix = Matrix(viewInfo.rotation);
	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	Vector3 vDelta = WorldLocation - viewInfo.location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((1920 / 2.0f) + vTransformed.x * (((1920 / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, (1080 / 2.0f) - vTransformed.y * (((1920 / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, 0);
}


Vector3 GetBoneWithRotation(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = 0;
	FTransform component_to_world;
	VMMDLL_SCATTER_HANDLE scatterHandle = mem.CreateScatterHandle();
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::BoneArray, &bone_array, sizeof(uintptr_t));
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::ComponetToWorld, &component_to_world, sizeof(FTransform));
	mem.ExecuteReadScatter(scatterHandle);
	if (bone_array == NULL) bone_array = mem.Read<uintptr_t>(mesh + offsets::BoneArray + 0x10);
	FTransform bone = mem.Read<FTransform>(bone_array + (bone_id * 0x60));
	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

bool IsVisible(uintptr_t mesh)
{
	float LastSumbitTime = 0;
	float LastRenderTimeOnScreen = 0;
	VMMDLL_SCATTER_HANDLE scatterHandle = mem.CreateScatterHandle();
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::LastSubmitTime, &LastSumbitTime, sizeof(float));
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::LastRenderTimeOnScreen, &LastRenderTimeOnScreen, sizeof(float));
	mem.ExecuteReadScatter(scatterHandle);
	bool Visible = LastRenderTimeOnScreen - LastSumbitTime <= 0.3;
	return Visible;
}