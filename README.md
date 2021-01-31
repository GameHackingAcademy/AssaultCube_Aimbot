# AssaultCube Aimbot
An aimbot for Assault Cube, a type of hack that automatically aims at enemy players.

It works by iterating over the enemy list and picking the closest enemy through euclidean distance. The yaw and pitch required to aim at that enemy are then calculate using arctangents.

This must be injected into the Assault Cube process to work. One way to do this is to use a DLL injector. Another way is to enable AppInit_DLLs in the registry.

The offsets and method to discover them are discussed in the article at: https://gamehacking.academy/lesson/27
