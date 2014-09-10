#include "AIE.h"
#include <iostream>

//prototypes
void readyPlayers();
void readyBall();
void Gameplay(float in_DeltaTime);

//constants
const int SCREEN_MAX_X = 1000, SCREEN_MAX_Y = 700;
	//player settings
const float playerWidth = 30.f;
const float playerHeight = 250.f;
const float playerSpeed = 600.f;
const float playerStartY = SCREEN_MAX_Y * .5f;
const float player1X = SCREEN_MAX_X * .10f;
const float player2X = SCREEN_MAX_X * .90f;
	//ball settings
const float ballWidth = 40.f;
const float ballHeight = 40.f;
const float ballStartXVector = 450.f;
const float ballStartYVector = 300.f;
const float ballStartX = SCREEN_MAX_X * .5f;
const float ballStartY = SCREEN_MAX_Y * .5f;
	//default keybinds
const unsigned int player1UpKey = 'W';
const unsigned int player1DownKey = 'S';
const unsigned int player2UpKey = 265; //up arrow key
const unsigned int player2DownKey = 264; //down arrow key
	//textures
const char* paddelSrc = "./images/crate_sideup.png";
const char* ballSrc = "./images/invaders/invaders_1_00.png";

struct Player {
	unsigned int spriteID;
	float width;
	float height;
	float x;
	float y;
	float speed;
	unsigned int keyUp;
	unsigned int keyDown;

	void move(float in_DeltaTime) {
		if (IsKeyDown(keyUp)) {
			y += speed * in_DeltaTime;
			if (y > SCREEN_MAX_Y - (height * .5f)){
				y = SCREEN_MAX_Y - (height * .5f);
			}
		}

		if (IsKeyDown(keyDown)) {
			y -= speed * in_DeltaTime;
			if (y < height * .5f) {
				y = height * .5f;
			}
		}
		MoveSprite(spriteID, x, y);
	}

};

struct Ball {
	unsigned int spriteID;
	float width;
	float height;
	float x;
	float y;
	float xVector;
	float yVector;

	void move(float in_DeltaTime) {
		x += xVector * in_DeltaTime;
		if (x < width * .5f) {
			x = width * .5f;
			xVector *= -1;
		} else if (x > SCREEN_MAX_X - (width * .5f)) {
			x = SCREEN_MAX_X - (width * .5f);
			xVector *= -1;
		}

		y += yVector * in_DeltaTime;
		if (y < height * .5f) {
			y = height * .5f;
			yVector *= -1;
		}
		else if (y > SCREEN_MAX_Y - (height * .5f)) {
			y = SCREEN_MAX_Y - (height * .5f);
			yVector *= -1;
		}

		MoveSprite(spriteID, x, y);
	}

	void reset() {
		x = ballStartX
	}
};

Player player1;
Player player2;
Ball ball;

int main( int argc, char* argv[] )
{	

	Initialise(SCREEN_MAX_X, SCREEN_MAX_Y, false, "Pong");
	SetBackgroundColour(SColour(0, 0, 0, 255));

	readyPlayers();
	readyBall();


    //Game Loop
    do
	{
		Gameplay(GetDeltaTime());

        ClearScreen();

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}

void readyPlayers() {
	//setup player 1
	player1.width = playerWidth;
	player1.height = playerHeight;
	player1.x = player1X;
	player1.y = playerStartY;
	player1.speed = playerSpeed;
	player1.keyUp = player1UpKey;
	player1.keyDown = player1DownKey;
	player1.spriteID = CreateSprite(paddelSrc, player1.width, player1.height, true);
	MoveSprite(player1.spriteID, player1.x, player1.y);
	//setup player 2
	player2.width = playerWidth;
	player2.height = playerHeight;
	player2.x = player2X;
	player2.y = playerStartY;
	player2.speed = playerSpeed;
	player2.keyUp = player2UpKey;
	player2.keyDown = player2DownKey;
	player2.spriteID = CreateSprite(paddelSrc, player2.width, player2.height, true);
	MoveSprite(player2.spriteID, player2.x, player2.y);
}

void readyBall() {
	ball.width = ballWidth;
	ball.height = ballHeight;
	ball.x = ballStartX;
	ball.y = ballStartY;
	ball.xVector = ballStartXVector;
	ball.yVector = ballStartYVector;
	ball.spriteID = CreateSprite(ballSrc, ball.width, ball.height, true);
	MoveSprite(ball.spriteID, ball.x, ball.y);
}

void Gameplay(float in_DeltaTime) {

	//movement
	player1.move(in_DeltaTime);
	player2.move(in_DeltaTime);
	ball.move(in_DeltaTime);

	//draw sprites
	DrawSprite(player1.spriteID);
	DrawSprite(player2.spriteID);
	DrawSprite(ball.spriteID);
}