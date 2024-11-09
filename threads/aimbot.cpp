#include "threads.h"

int customRound(float value) {
	if (value == 0) return 0;
	if (value > 0) {
		return static_cast<int>(value + 0.5f);
	}
	else {
		return static_cast<int>(value - 0.5f);
	}
}

//void addScaled(Vector3 target, const Vector3& v, float scale)
//{
//	Vector3 newPos = target;
//
//	newPos.x += v.x * scale;
//	newPos.y += v.y * scale;
//	newPos.z += v.z * scale;
//}

Vector3 godPrediction(Vector3 target, Vector3 velocity, float projectile_speed, float gravity_scale, float distance)
{
	//std::cout << projectile_speed << std::endl;
	//std::cout << gravity_scale << std::endl;
	//std::cout << target.z << std::endl;
	Vector3 newPos = target;
	float time = distance / (projectile_speed / 100);
	newPos.x += velocity.x * time;
	newPos.y += velocity.y * time;
	newPos.z += velocity.z * time;

	float gravity = std::fabs(-980.0f * gravity_scale) * 0.5f * time * time;
	newPos.z += gravity;
	//newPos.z += 1000;
	//std::cout << target.z << std::endl;

	return newPos;
}

void fullReset() {
	globals::readGlobals = true;
	closestPlayer::closest_distance_from_center = FLT_MAX;
	closestPlayer::closest_player_distance = FLT_MAX;
	closestPlayer::closest_player_private = 0;
	closestPlayer::closest_player_mesh = 0;
	closestPlayer::index = 0;
}

void aimbot() {
	if (!enableWebSocketAim) return;
	ConnectAim("tcp://192.168.50.242:12345"); // hypr
	//ConnectAim("tcp://192.168.1.178:12345"); // zerky
	//ConnectAim("tcp://192.168.1.244:12345"); // mage
	//ConnectAim("tcp://192.168.0.89:12345"); //loxy

	while (true) {
		if (!settings::aimbot) {
			fullReset();
			continue;
		}
		if (Keyboard.IsKeyDown(settings::aimkeybind)) {
			if (closestPlayer::closest_player_distance != FLT_MAX && closestPlayer::closest_player_private != 0 && closestPlayer::closest_player_mesh != 0) {
				if (settings::VisCheck && !is_entity_visible(closestPlayer::index)) {
					fullReset();
					continue;
				}
				globals::readGlobals = false;
				Vector3 roothead = GetBoneWithRotation(closestPlayer::closest_player_mesh, 67);
				
				roothead.z += settings::offset;
				get_view_point();
				Vector3 output;
				if (cache::bulletSpeed == 0 || cache::bulletGravity == 0) {
					output = roothead;
				}
				else {
					Vector3 velocity = mem.Read<Vector3>(closestPlayer::root_component + offsets::Velocity);

					output = godPrediction(roothead, velocity, cache::bulletSpeed, cache::bulletGravity, closestPlayer::closest_player_distance);

				}

				Vector3 rootHead2D = ProjectWorldToScreen(output);
				int dx = customRound((rootHead2D.x - 960) / static_cast<float>(settings::smoothing));
				int dy = customRound((rootHead2D.y - 540) / static_cast<float>(settings::smoothing));
				float dist = sqrtf(dx * dx + dy * dy);

				if (dx > 1000 || dx < -1000 || dy > 1000 || dy < -1000) continue;
				//consoleLog("X: %d", dx);
				//consoleLog("Y: %d", dy);

				std::string xString = std::to_string(int(dx));
				std::string yString = std::to_string(int(dy));
				std::string aimAt = xString + "," + yString;
				SendAim(aimAt);
			}

		}
		else {
			fullReset();
		}
		auto start = std::chrono::high_resolution_clock::now();

		while (true) {
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			if (duration.count() >= settings::delay) {
				break;
			}
		}
	}
}