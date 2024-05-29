#include "chip8.hpp"
#include "raylib.h"
#include "common.hpp"

int main()
{
	const int scale = 20;
	const int sw {64 * scale};
	const int sh {32 * scale};

	InitWindow(sw, sh, "c8");

	SetTargetFPS(60);

	chip8 cpu;

	while (!WindowShouldClose()) {
		
		BeginDrawing();

		ClearBackground(BACKGROUND);


		EndDrawing();
	}

	CloseWindow();

	return 0;

}
