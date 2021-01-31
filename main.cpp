#include <Windows.h>
#include <math.h>

#define M_PI 3.14159265358979323846

struct Player {
	char unknown1[4];
	float x;
	float y;
	float z;
	char unknown2[0x30];
	float yaw;
	float pitch;
	char unknown3[0x2f0];
	int dead;
};

Player *player = NULL;

float euclidean_distance(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

void injected_thread() {

	while (true) {
		DWORD* player_offset = (DWORD*)(0x509B74);
		player = (Player*)(*player_offset);

		int* current_players = (int*)(0x50F500);
		float closest_player = -1.0f;
		float closest_yaw = 0.0f;
		float closest_pitch = 0.0f;

		for (int i = 0; i < *current_players; i++) {
			DWORD* enemy_list = (DWORD*)(0x50F4F8);
			DWORD* enemy_offset = (DWORD*)(*enemy_list + (i*4));
			Player* enemy = (Player*)(*enemy_offset);

			if (player != NULL && enemy != NULL && !enemy->dead) {

				float abspos_x = enemy->x - player->x;
				float abspos_y = enemy->y - player->y;
				float abspos_z = enemy->z - player->z;

				float temp_distance = euclidean_distance(abspos_x, abspos_y);
				if (closest_player == -1.0f || temp_distance < closest_player) {
					closest_player = temp_distance;

					float azimuth_xy = atan2f(abspos_y, abspos_x);
					float yaw = (float)(azimuth_xy * (180.0 / M_PI));
					closest_yaw = yaw + 90;

					if (abspos_y < 0) {
						abspos_y *= -1;
					}
					if (abspos_y < 5) {
						if (abspos_x < 0) {
							abspos_x *= -1;
						}
						abspos_y = abspos_x;
					}
					float azimuth_z = atan2f(abspos_z, abspos_y);
					closest_pitch = (float)(azimuth_z * (180.0 / M_PI));
				}
			}
		}

		player->yaw = closest_yaw;
		player->pitch = closest_pitch;

		Sleep(1);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injected_thread, NULL, 0, NULL);
	}

	return true;
}
