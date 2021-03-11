// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;
// 볼의 갯수를 상수로 정한다.
const int number_of_balls = 10;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir(0)
,numbers(number_of_balls) // Ball 갯수 초기화
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Chapter 1, Exercise 2", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	

	// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}


	//
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;


	// Ball 여러 개의 위치와 속도를 초기화하고 vector 에 넣는다.
	for (int i = 0; i < numbers; i++) {
		float inc = 30.0f;
		
		Ball temp;
		temp.pos.x = 1024.0f / 2.0f + inc * i;
		temp.pos.y = 768.0f / 2.0f + inc * i;
		temp.vel.x = 200.0f - inc * i;
		temp.vel.y = 235.0f + inc * i;
		
		balls.push_back(temp);

	}

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}
	

	// 여러 Ball 들의 속도와 위치를 갱신한다.
	for (int i = 0; i < numbers; i++) {
		// 속도에 맞춰서 각 볼의 위치 갱신
		balls[i].pos.x += balls[i].vel.x * deltaTime;
		balls[i].pos.y += balls[i].vel.y * deltaTime;

		float diff = mPaddlePos.y - balls[i].pos.y;
		diff = (diff > 0.0f) ? diff : -diff;

		// 공이 패들이 부딪히는 경우
		if (
			diff <= paddleH / 2.0f &&
			balls[i].pos.x <= 25.0f && balls[i].pos.x >= 20.0f &&
			balls[i].vel.x < 0.0f)
		{
			balls[i].vel.x *= -1.0f;
		}
		// 공이 스크린 밖으로 나가는 경우
		else if (balls[i].pos.x <= 0.0f)
		{
			mIsRunning = false;
		}
		// Did the ball collide with the right wall?
		else if (balls[i].pos.x >= (1024.0f - thickness) && balls[i].vel.x > 0.0f)
		{
			balls[i].vel.x *= -1.0f;
		}

		// Did the ball collide with the top wall?
		if (balls[i].pos.y <= thickness && balls[i].vel.y < 0.0f)
		{
			balls[i].vel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (balls[i].pos.y >= (768 - thickness) &&
			balls[i].vel.y > 0.0f)
		{
			balls[i].vel.y *= -1;
		}
	}
	
	
	
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	

	// 여러 개의 공들을 그린다
	for (auto i : balls) {
		SDL_Rect ball{
		static_cast<int>(i.pos.x - thickness / 2),
		static_cast<int>(i.pos.y - thickness / 2),
		thickness,
		thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);
	}
	
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
