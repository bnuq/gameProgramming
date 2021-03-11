// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include <vector>

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};


// 2개의 벡터를 포함하는 Ball 구조체
struct Ball
{
	Vector2 pos; // Ball 의 위치
	Vector2 vel; // Ball 의 속도
};

// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	
	// Pong specific
	// Direction of paddle
	int mPaddleDir;
	// Position of paddle
	Vector2 mPaddlePos;


	// 멀티볼
	std::vector<Ball> balls;
	// Ball 의 갯수
	int numbers;
};
