# Assault Cube Aimbot
Referenced in https://gamehacking.academy/lesson/5/6.

An aimbot for Assault Cube 1.2.0.2 that automatically aims at enemy players.

It works by iterating over the enemy list and picking the closest enemy through Euclidean distance. The yaw and pitch required to aim at that enemy are then calculated using arctangents.

This must be injected into the Assault Cube process to work. One way to do this is to use a DLL injector. Another way is to enable AppInit_DLLs in the registry.
