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

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)

,leftPaddleDir(0)
,rightPaddleDir(0)
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
		"Chapter 1's Practice 1", // Window title
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
	

	//// Create SDL renderer
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
	// left paddle 은 그냥 왼쪽에서 10.0f 만큼 떨어져 있는 거야
	leftPaddlePos.x = 10.0f;
	leftPaddlePos.y = 768.0f/2.0f;

	// right paddle 도 오른쪽에서 10.0f 만큼 떨어져 있는 상태에서 시작하자. thickness 를 고려해야 한다.
	rightPaddlePos.x = 1024.0f - thickness - 10.0f;
	rightPaddlePos.y = 768.0f / 2.0f;


	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;
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
	
	// Update left paddle direction based on W/S keys
	leftPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		leftPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		leftPaddleDir += 1;
	}

	// Update right paddle direction based on I/K keys
	rightPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		rightPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		rightPaddleDir += 1;
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
	
	// Update left paddle position based on direction
	if (leftPaddleDir != 0)
	{
		leftPaddlePos.y += leftPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (leftPaddlePos.y < (paddleH/2.0f + thickness))
		{
			leftPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (leftPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			leftPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update right paddle position based on direction
	if (rightPaddleDir != 0)
	{
		rightPaddlePos.y += rightPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (rightPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			rightPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (rightPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			rightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	

	// Bounce if needed
	// 
	// left paddle 과의 거리 체크
	float leftDiff = leftPaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	leftDiff = (leftDiff > 0.0f) ? leftDiff : -leftDiff;

	// right paddle 과의 거리 체크
	float rightDiff = rightPaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	rightDiff = (rightDiff > 0.0f) ? rightDiff : -rightDiff;


	// Ball 과 left paddle 간의 충돌 체크
	if (
		// Our y-difference is small enough
		leftDiff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// Ball 과 right paddle 간의 충돌 체크
	else if (
		// Our y-difference is small enough
		rightDiff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x >= 1024.0f-thickness-10.0f && mBallPos.x <= 1024.0f-thickness-5.0f &&
		// The ball is moving to the left
		mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}

	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}
	// 오른쪽에 패들이 있으므로 오른쪽 벽과의 충돌은 확인하지 않는다.
	// 오른쪽 스크린을 벗어나면 게임이 종료된다
	else if (mBallPos.x >= 1024.f)
	{
		mIsRunning = false;
	}

	
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
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
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
	
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
	
	// right paddle 이 존재하므로 right wall 은 그리지 않는다.
	


	// Draw left paddle
	SDL_Rect leftPaddle{
		static_cast<int>(leftPaddlePos.x),
		static_cast<int>(leftPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	// Draw right paddle
	SDL_Rect rightPaddle{
		static_cast<int>(rightPaddlePos.x),
		static_cast<int>(rightPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &rightPaddle);
	
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
