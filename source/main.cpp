#include "AIE.h"
#include <iostream>
#include <ctime>


//prototypes
void readyPlayers();
void readyBall();
void readyScore();
void readyGame();
void mainMenu();
void optionsMenu();
void endGame();
void Gameplay(float in_DeltaTime);

//constants
const int SCREEN_MAX_X = 1000, SCREEN_MAX_Y = 700;
	//lose strings
const char* titleText0 = "Pong";
const char* titleText1 = "Press Space to begin";
const char* titleText2 = "Press O to go to options";

const char* winText0 = "Press escape to reset";
const char* winText1 = "Player 1 Wins!";
const char* winText2 = "Player 2 Wins!";
	//player settings
const float playerWidth = 30.f;
const float playerStartY = SCREEN_MAX_Y * .5f;
const float player1X = SCREEN_MAX_X * .10f;
const float player2X = SCREEN_MAX_X * .90f;
	//ball settings
const float ballWidth = 40.f;
const float ballHeight = 40.f;
const float ballStartXVector = 450; //((std::rand() % 45) + 20) * 10
const float ballStartYVector = 300;
const float ballStartX = SCREEN_MAX_X * .5f;
const float ballStartY = SCREEN_MAX_Y * .5f;
	//Score positions
const float player1ScoreX = SCREEN_MAX_X * .15f;
const float player1ScoreY = SCREEN_MAX_Y * .95f;
const float player2ScoreX = SCREEN_MAX_X * .835f;
const float player2ScoreY = SCREEN_MAX_Y * .95f;
	//default keybinds
const unsigned int player1UpKey = 'W';
const unsigned int player1DownKey = 'S';
const unsigned int player2UpKey = 265; //up arrow key
const unsigned int player2DownKey = 264; //down arrow key
	//textures
const char* paddelSrc = "./images/crate_sideup.png";
const char* ballSrc = "./images/invaders/invaders_1_00.png";

//game vars
unsigned int player1Score;
unsigned int player2Score;

//Option values
enum OPT_PADDEL_SIZE {
	PSz_verySmall = 50,
	PSz_small = 100,
	PSz_Normal = 200,
	PSz_large = 250,
	PSz_veryLarge = 300
};

enum OPT_PADDEL_SPEED {
	PSp_verySlow = 300,
	PSp_slow = 450,
	PSp_Normal = 600,
	PSp_fast = 750,
	PSp_veryFast = 900
};

enum OPT_BALL_SPEED {
	BSp_verySlow = 8,
	BSp_slow = 9,
	BSp_Normal = 10,
	BSp_fast = 11,
	BSp_veryFast = 12
};

const unsigned int OPT_SCORE_CAP_MAX = 15;
const unsigned int OPT_SCORE_CAP_MIN = 1;


//Option container
OPT_PADDEL_SIZE set_paddelSize = PSz_Normal;
OPT_PADDEL_SPEED set_paddelSpeed = PSp_Normal;
OPT_BALL_SPEED set_ballSpeed = BSp_Normal;
unsigned int set_scoreCap = 7;

enum MENU_CURRENT {
	scoreCap,
	paddelSize,
	paddelSpeed,
	ballSpeed
};

//Gamestate
enum GAMESTATE {
	title,
	options,
	gameplay,
	end
};

GAMESTATE currentState;

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

			player2Score++;
			reset();

		} else if (x > SCREEN_MAX_X - (width * .5f)) {
			x = SCREEN_MAX_X - (width * .5f);
			xVector *= -1;

			player1Score++;
			reset();
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
		x = ballStartX;
		y = ballStartY;

		srand(time(NULL));

		if (std::rand() % 2 == 0) {
			xVector = ballStartXVector;
		} else {
			xVector = -ballStartXVector;
		}

		if (std::rand() % 2 == 0) {
			yVector = ((std::rand() % 30) + 10) * set_ballSpeed;
		} else {
			yVector = -(((std::rand() % 30) + 10) * set_ballSpeed);
		}

	}

	bool collide(Player other) {
		bool hasColided = false;

		if (abs(x - other.x) < (width * .5f) + (other.width * .5f) && abs(y - other.y) < (height * .5f) + (other.height * .5f)) {

			if (x - other.x > 0) {
				x = other.x + ((width * .5f) + (other.width * .5f));
			} else {
				x = other.x - ((width * .5f) + (other.width * .5f));
			}

			xVector *= -1;

			hasColided = true;
		}

		MoveSprite(spriteID, x, y);
		return hasColided;
	}
};

//entities
Player player1;
Player player2;
Ball ball;

int main( int argc, char* argv[] )
{	

	Initialise(SCREEN_MAX_X, SCREEN_MAX_Y, false, "Pong");
	SetBackgroundColour(SColour(0, 0, 0, 255));

	currentState = title;


    //Game Loop
    do
	{
		ClearScreen();

		switch (currentState) 
		{
		case title:
			mainMenu();
			break;

		case options:
			optionsMenu();
			break;

		case gameplay:
			Gameplay(GetDeltaTime());
			break;

		case end:
			endGame();
			break;

		default:
			std::cout << "State value error";
			return -1;
			break;
		}

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}

void readyPlayers() {
	//setup player 1
	player1.width = playerWidth;
	player1.height = set_paddelSize;
	player1.x = player1X;
	player1.y = playerStartY;
	player1.speed = set_paddelSpeed;
	player1.keyUp = player1UpKey;
	player1.keyDown = player1DownKey;
	player1.spriteID = CreateSprite(paddelSrc, player1.width, player1.height, true);
	MoveSprite(player1.spriteID, player1.x, player1.y);
	//setup player 2
	player2.width = playerWidth;
	player2.height = set_paddelSize;
	player2.x = player2X;
	player2.y = playerStartY;
	player2.speed = set_paddelSpeed;
	player2.keyUp = player2UpKey;
	player2.keyDown = player2DownKey;
	player2.spriteID = CreateSprite(paddelSrc, player2.width, player2.height, true);
	MoveSprite(player2.spriteID, player2.x, player2.y);
}

void readyBall() {
	ball.width = ballWidth;
	ball.height = ballHeight;
	ball.reset();
	ball.spriteID = CreateSprite(ballSrc, ball.width, ball.height, true);
	MoveSprite(ball.spriteID, ball.x, ball.y);
}

void readyScore() {
	player1Score = 0;
	player2Score = 0;
}

void readyGame() {
	readyPlayers();
	readyBall();
	readyScore();
	currentState = gameplay;
}

void mainMenu() {

	if (!IsKeyDown(256)) {
		if (IsKeyDown(' ')) {
			readyGame();
		}

		if (IsKeyDown('O')) {
			currentState = options;
		}
	}

	DrawString(titleText0, SCREEN_MAX_X * .47f, SCREEN_MAX_Y * .5f);
	DrawString(titleText1, SCREEN_MAX_X * .37f, SCREEN_MAX_Y * .2f);
	DrawString(titleText2, SCREEN_MAX_X * .35f, SCREEN_MAX_Y * .15f);

}

void optionsMenu() {
	if (IsKeyDown(256)) {
		currentState = title;
	}
}

void Gameplay(float in_DeltaTime) {

	if (IsKeyDown(256)) {
		currentState = title;
	}

	//movement
	player1.move(in_DeltaTime);
	player2.move(in_DeltaTime);
	ball.move(in_DeltaTime);
	ball.collide(player1);
	ball.collide(player2);

	//draw sprites
	DrawSprite(player1.spriteID);
	DrawSprite(player2.spriteID);
	DrawSprite(ball.spriteID);

	char str[10];
	DrawString(itoa(player1Score, str, 10), player1ScoreX, player1ScoreY);
	DrawString(itoa(player2Score, str, 10), player2ScoreX, player2ScoreY);

	if (player1Score >= set_scoreCap || player2Score >= set_scoreCap) {
		currentState = end;
	}
}

void endGame() {
	if (IsKeyDown(256)) {
		currentState = title;
	}

	if (player1Score > player2Score) {
		DrawString(winText1, player1ScoreX, player1ScoreY);
	} else {
		DrawString(winText2, player1ScoreX, player1ScoreY);
	}

	DrawString(winText0, SCREEN_MAX_X * .37f, SCREEN_MAX_Y * .2f);
}