/* An aimbot for Assault Cube, a type of hack that automatically aims at enemy players.

It works by iterating over the enemy list and picking the closest enemy through euclidean distance. 
The yaw and pitch required to aim at that enemy are then calculate using arctangents.

This must be injected into the Assault Cube process to work. One way to do this is to use a DLL injector. 
Another way is to enable AppInit_DLLs in the registry.

The offsets and method to discover them are discussed in the article at: https://gamehacking.academy/lesson/27
*/

#include <Windows.h>
#include <math.h>

// The atan2f function produces a radian. To convert it to degrees, we need the value of pi
#define M_PI 3.14159265358979323846

// The player structure for every player in the game
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

// Our player
Player *player = NULL;

// Function to calculate the euclidean distance between two points
float euclidean_distance(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

// This thread contains all of our aimbot code
void injected_thread() {

	while (true) {
		// First, grab the current position and view angles of our player
		DWORD* player_offset = (DWORD*)(0x509B74);
		player = (Player*)(*player_offset);

		// Then, get the current number of players in the game
		int* current_players = (int*)(0x50F500);
		
		// These variables will be used to hold the closest enemy to us
		float closest_player = -1.0f;
		float closest_yaw = 0.0f;
		float closest_pitch = 0.0f;

		// Iterate through all active enemies
		for (int i = 0; i < *current_players; i++) {
			DWORD* enemy_list = (DWORD*)(0x50F4F8);
			DWORD* enemy_offset = (DWORD*)(*enemy_list + (i*4));
			Player* enemy = (Player*)(*enemy_offset);

			// Make sure the enemy is valid and alive
			if (player != NULL && enemy != NULL && !enemy->dead) {

				// Calculate the absolute position of the enemy away from us to ensure that our future calculations are correct and based
				// around the origin
				float abspos_x = enemy->x - player->x;
				float abspos_y = enemy->y - player->y;
				float abspos_z = enemy->z - player->z;

				// Calculate our distance from the enemy
				float temp_distance = euclidean_distance(abspos_x, abspos_y);
				// If this is the closest enemy so far, calculate the yaw and pitch to aim at them
				if (closest_player == -1.0f || temp_distance < closest_player) {
					closest_player = temp_distance;

					// Calculate the yaw
					float azimuth_xy = atan2f(abspos_y, abspos_x);
					float yaw = (float)(azimuth_xy * (180.0 / M_PI));
					closest_yaw = yaw + 90;

					// Calculate the pitch
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

		// When our loop ends, set our yaw and pitch to the closst values
		player->yaw = closest_yaw;
		player->pitch = closest_pitch;

		// So our thread doesn't constantly run, we have it pause execution for a millisecond.
		// This allows the processor to schedule other tasks.
		Sleep(1);
	}
}

// When our DLL is loaded, create a thread in the process that will handle the aimbot code
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injected_thread, NULL, 0, NULL);
	}

	return true;
}
