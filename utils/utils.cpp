#include "utils.h"
#include "structs.h"
#include "offsets.h"
#include "globals.h"
#include "Memory.h"
#include "cache.h"
struct FPlane : Vector3
{
	double W;

	FPlane() : W(0) { }
	FPlane(double W) : W(W) { }
};

class FMatrix
{
public:
	double m[4][4];
	FPlane XPlane, YPlane, ZPlane, WPlane;

	FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() { }
	FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane)
		: XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) { }
};

inline double RadiansToDegrees(double dRadians)
{
	return dRadians * (180.0 / M_PI);
}

template< typename t >
class TArray
{
public:

	TArray() : tData(), iCount(), iMaxCount() { }
	TArray(t* data, int count, int max_count) :
		tData(tData), iCount(iCount), iMaxCount(iMaxCount) { }

public:

	auto Get(int idx) -> t
	{
		return mem.Read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
	}

	auto Size() -> std::uint32_t
	{
		return this->iCount;
	}

	bool IsValid()
	{
		return this->iCount != 0;
	}

	t* tData;
	int iCount;
	int iMaxCount;
};

auto GetViewState() -> uintptr_t
{
	TArray<uintptr_t> ViewState = mem.Read<TArray<uintptr_t>>(globals::Localplayer + 0xD0);
	//std::cout << ViewState.Get(1) << std::endl;
	return ViewState.Get(1);
}

void get_view_point()
{
	uintptr_t ViewState = GetViewState();
	CameraInfo view_point;
	auto mProjection = mem.Read<FMatrix>(ViewState + 0x900);
	view_point.rotation.x = RadiansToDegrees(std::asin(mProjection.ZPlane.W));
	view_point.rotation.y = RadiansToDegrees(std::atan2(mProjection.YPlane.W, mProjection.XPlane.W));
	view_point.rotation.z = 0.0;

	view_point.location.x = mProjection.m[3][0];
	view_point.location.y = mProjection.m[3][1];
	view_point.location.z = mProjection.m[3][2];
	float FieldOfView = atanf(1 / mem.Read<double>(ViewState + 0x700)) * 2;
	view_point.fov = (FieldOfView) * (180.f / M_PI); 
	/*std::cout << "X: " << view_point.rotation.x << std::endl;
	std::cout << "Y: " << view_point.rotation.y << std::endl;
	std::cout << "Z: " << view_point.rotation.z << std::endl;*/
	viewInfo = view_point;
}

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

bool is_entity_visible(int i)
{
	/*float last_render_time_on_screen = mem.Read<float>(cache::meshes[i] + offsets::LastSubmitTime);*/
	return cache::isVis[i];
	
}

void visCheckLoop() {
	while (true) {
		VMMDLL_SCATTER_HANDLE arrayHandle = mem.CreateScatterHandle();
		for (int i = 0; i < globals::Num; i++) {
			if (cache::meshes[i] == (uint64_t)nullptr) continue;
			mem.AddScatterReadRequest(arrayHandle, (cache::meshes[i] + offsets::LastSubmitTime), &cache::last_submit_time_on_screen[i], sizeof(float));
		}
		mem.ExecuteReadScatter(arrayHandle);
		VMMDLL_Scatter_CloseHandle(arrayHandle);
		Sleep(10);
		VMMDLL_SCATTER_HANDLE arrayHandle2 = mem.CreateScatterHandle();
		for (int i = 0; i < globals::Num; i++) {
			if (cache::meshes[i] == (uint64_t)nullptr) continue;
			mem.AddScatterReadRequest(arrayHandle2, (cache::meshes[i] + offsets::LastSubmitTime), &cache::last_submit_time_on_screen2[i], sizeof(float));
		}
		mem.ExecuteReadScatter(arrayHandle2);
		VMMDLL_Scatter_CloseHandle(arrayHandle2);
		for (int i = 0; i < globals::Num; i++) {

			if (cache::last_submit_time_on_screen2[i] > cache::last_submit_time_on_screen[i])
			{
				cache::last_submit_time_on_screen[i] = cache::last_submit_time_on_screen2[i];
				cache::isVis[i] = true;
			}
			else
			{
				cache::isVis[i] = false;
			}
		}
	}
}

bool IsVisible(uintptr_t mesh)
{
	float LastSumbitTime = 0;
	float LastRenderTimeOnScreen = 0;
	VMMDLL_SCATTER_HANDLE scatterHandle = mem.CreateScatterHandle();
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::LastSubmitTime, &LastSumbitTime, sizeof(float));
	mem.AddScatterReadRequest(scatterHandle, mesh + offsets::LastSubmitTime, &LastRenderTimeOnScreen, sizeof(float));
	mem.ExecuteReadScatter(scatterHandle);
	bool Visible = LastRenderTimeOnScreen - LastSumbitTime <= 0.3;
	return Visible;
}