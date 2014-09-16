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

const char* optText0 = "Score Cap:";
const char* optText1 = "Paddel Size:";
const char* optText2 = "Paddel Speed:";
const char* optText3 = "Ball Speed:";
const char* optText4 = "Press escape to return to the main menu";
const char* optText5 = "Use the arrow keys to select and change options";
const char* optText6 = "Press space to start game";

const char* setText0 = "Normal";
const char* setText1 = "Very Small";
const char* setText2 = "Small";
const char* setText3 = "Large";
const char* setText4 = "Very Large";
const char* setText5 = "Very Slow";
const char* setText6 = "Slow";
const char* setText7 = "Fast";
const char* setText8 = "Very Fast";
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
const char* pongSrc = "./images/pong_texture.png";

//game vars
unsigned int player1Score;
unsigned int player2Score;

//Option presets
enum OPT_PADDEL_SIZE {
	PSz_verySmall = 50,
	PSz_small = 100,
	PSz_Normal = 200,
	PSz_large = 250,
	PSz_veryLarge = 300
};

enum OPT_PADDEL_SPEED {
	PSp_verySlow = 100,
	PSp_slow = 300,
	PSp_Normal = 600,
	PSp_fast = 900,
	PSp_veryFast = 1200
};

enum OPT_BALL_SPEED {
	BSp_verySlow = 150,
	BSp_slow = 250,
	BSp_Normal = 450,
	BSp_fast = 650,
	BSp_veryFast = 850
};

const unsigned int OPT_SCORE_CAP_MAX = 15;
const unsigned int OPT_SCORE_CAP_MIN = 1;


//Option containers
OPT_PADDEL_SIZE set_paddelSize = PSz_Normal;
OPT_PADDEL_SPEED set_paddelSpeed = PSp_Normal;
OPT_BALL_SPEED set_ballSpeed = BSp_Normal;
unsigned int set_scoreCap = 7;

//options menu current state
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
MENU_CURRENT currentSelection;

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

		if (IsKeyDown(keyUp)) {//detect keypress

			y += speed * in_DeltaTime;//add movement (deltaTime compinsates for any change in framerate) 
			if (y > SCREEN_MAX_Y - (height * .5f)){//detect if the paddel has gone off of the screen
				y = SCREEN_MAX_Y - (height * .5f);//return paddel to screen
			}
		}

		if (IsKeyDown(keyDown)) {
			y -= speed * in_DeltaTime;
			if (y < height * .5f) {
				y = height * .5f;
			}
		}

		MoveSprite(spriteID, x, y);//commit movement
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

		x += xVector * in_DeltaTime;//move in x direction
		if (x < width * .5f) {// detect screen edge
			x = width * .5f;//return to screen (irrelivent)
			xVector *= -1;//reverse x direction bounce (irrelevent)

			player2Score++;//increment score for the goal
			reset();//recenter ball and give a new vector

		} else if (x > SCREEN_MAX_X - (width * .5f)) {
			x = SCREEN_MAX_X - (width * .5f);
			xVector *= -1;

			player1Score++;
			reset();
		}

		y += yVector * in_DeltaTime;//move in y direction
		if (y < height * .5f) {// check if ball has left screen
			y = height * .5f;//bring bell back to screen
			yVector *= -1;//invert direction to make it "Bounce"
		}
		else if (y > SCREEN_MAX_Y - (height * .5f)) {
			y = SCREEN_MAX_Y - (height * .5f);
			yVector *= -1;
		}

		MoveSprite(spriteID, x, y);//finalize movement
	}

	void reset() {
		x = ballStartX;//reset ball position
		y = ballStartY;

		srand(time(NULL));//seed RNG

		if (std::rand() % 2 == 0) {//generate wether the ball is going left or right
			xVector = set_ballSpeed;//set ball's speed to what the current option allows
		} else {
			xVector = -set_ballSpeed;
		}

		if (std::rand() % 2 == 0) {//generate wether the ball is going in a more upward or downward direction
			yVector = ((std::rand() % 30) + 10) * 10;//sets ball to a random speed up or down (random 10 - 39) * 10
		} else {
			yVector = -(((std::rand() % 30) + 10) * 10);
		}

	}

	bool collide(Player other) {
		bool hasColided = false;//this will be set to true if the ball has colided with the given object

		if (abs(x - other.x) < (width * .5f) + (other.width * .5f) && abs(y - other.y) < (height * .5f) + (other.height * .5f)) {//bounce if the ball is within the given paddel's (other) area and is touching on the y direction as well

			if (x - other.x > 0) {//determine wich side the ball needs to be placed on
				x = other.x + ((width * .5f) + (other.width * .5f));//set ball on the correct side of the paddel
			} else {
				x = other.x - ((width * .5f) + (other.width * .5f));
			}

			xVector *= -1;//bounce

			hasColided = true;//confirm that the ball has bounced
		}

		MoveSprite(spriteID, x, y);//commit movement
		return hasColided;//retur weater or not te ball has bounced
	}
};

//entities
Player player1;
Player player2;
Ball ball;

bool keyDown = false;//track weather or not the keys are pressed so that one cane properly navigate the options menu

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
	player1.spriteID = CreateSprite(pongSrc, player1.width, player1.height, true);
	MoveSprite(player1.spriteID, player1.x, player1.y);
	//setup player 2
	player2.width = playerWidth;
	player2.height = set_paddelSize;
	player2.x = player2X;
	player2.y = playerStartY;
	player2.speed = set_paddelSpeed;
	player2.keyUp = player2UpKey;
	player2.keyDown = player2DownKey;
	player2.spriteID = CreateSprite(pongSrc, player2.width, player2.height, true);
	MoveSprite(player2.spriteID, player2.x, player2.y);
}

void readyBall() {
	ball.width = ballWidth;
	ball.height = ballHeight;
	ball.reset();
	ball.spriteID = CreateSprite(pongSrc, ball.width, ball.height, true);
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
			currentSelection = scoreCap;
		}
	}

	DrawString(titleText0, SCREEN_MAX_X * .47f, SCREEN_MAX_Y * .5f);
	DrawString(titleText1, SCREEN_MAX_X * .37f, SCREEN_MAX_Y * .2f);
	DrawString(titleText2, SCREEN_MAX_X * .35f, SCREEN_MAX_Y * .15f);

}

void optionsMenu() {
	DrawString(optText5, SCREEN_MAX_X * .2f, SCREEN_MAX_Y *.9f);
	if (IsKeyDown(256)) {
		currentState = title;
	}
	DrawString(optText4, SCREEN_MAX_X * .25f, SCREEN_MAX_Y *.2f);
	if (IsKeyDown(' ')) {
		readyGame();
	}
	DrawString(optText6, SCREEN_MAX_X * .35f, SCREEN_MAX_Y *.15f);

	switch (currentSelection) {
	case scoreCap:

		DrawString(optText0, player1ScoreX, player1ScoreY);

		char str[10];
		DrawString(itoa(set_scoreCap, str, 10), player2ScoreX, player2ScoreY);

		//change value
		if (IsKeyDown(263) && !keyDown) {//arrow left
			set_scoreCap--;
			if (set_scoreCap < OPT_SCORE_CAP_MIN) {
				set_scoreCap = OPT_SCORE_CAP_MIN;
			}
			keyDown = true;
		}
		if (IsKeyDown(262) && !keyDown) {//arrow right
			set_scoreCap++;
			if (set_scoreCap > OPT_SCORE_CAP_MAX) {
				set_scoreCap = OPT_SCORE_CAP_MAX;
			}
			keyDown = true;
		}

		//change state
		if (IsKeyDown(265) && !keyDown) {//arrow up
			currentSelection = ballSpeed;
			keyDown = true;
		}
		if (IsKeyDown(264) && !keyDown) {//arrow down
			currentSelection = paddelSize;
			keyDown = true;
		}

		break;

	case paddelSize:

		DrawString(optText1, player1ScoreX, player1ScoreY);

		switch (set_paddelSize) {
		case PSz_verySmall:
			DrawString(setText1, player2ScoreX, player2ScoreY);
			break;
		case PSz_small:
			DrawString(setText2, player2ScoreX, player2ScoreY);
			break;
		case PSz_Normal:
			DrawString(setText0, player2ScoreX, player2ScoreY);
			break;
		case PSz_large:
			DrawString(setText3, player2ScoreX, player2ScoreY);
			break;
		case PSz_veryLarge:
			DrawString(setText4, player2ScoreX, player2ScoreY);
			break;
		default:
			std::cout << "setting value error reseting to \"normal\"";
			set_paddelSize = PSz_Normal;
			break;
		}

		//change value
		if (IsKeyDown(263) && !keyDown) {//arrow left
			switch (set_paddelSize) {
			case PSz_verySmall:
				set_paddelSize = PSz_veryLarge;
				break;
			case PSz_small:
				set_paddelSize = PSz_verySmall;
				break;
			case PSz_Normal:
				set_paddelSize = PSz_small;
				break;
			case PSz_large:
				set_paddelSize = PSz_Normal;
				break;
			case PSz_veryLarge:
				set_paddelSize = PSz_large;
				break;
			}
			keyDown = true;
		}
		if (IsKeyDown(262) && !keyDown) {//arrow right
			switch (set_paddelSize) {
			case PSz_verySmall:
				set_paddelSize = PSz_small;
				break;
			case PSz_small:
				set_paddelSize = PSz_Normal;
				break;
			case PSz_Normal:
				set_paddelSize = PSz_large;
				break;
			case PSz_large:
				set_paddelSize = PSz_veryLarge;
				break;
			case PSz_veryLarge:
				set_paddelSize = PSz_verySmall;
				break;
			}
			keyDown = true;
		}

		//change state
		if (IsKeyDown(265) && !keyDown) {//arrow up
			currentSelection = scoreCap;
			keyDown = true;
		}
		if (IsKeyDown(264) && !keyDown) {//arrow down
			currentSelection = paddelSpeed;
			keyDown = true;
		}

		break;
	case paddelSpeed:

		DrawString(optText2, player1ScoreX, player1ScoreY);

		switch (set_paddelSpeed) {
		case PSp_verySlow:
			DrawString(setText5, player2ScoreX, player2ScoreY);
			break;
		case PSp_slow:
			DrawString(setText6, player2ScoreX, player2ScoreY);
			break;
		case PSp_Normal:
			DrawString(setText0, player2ScoreX, player2ScoreY);
			break;
		case PSp_fast:
			DrawString(setText7, player2ScoreX, player2ScoreY);
			break;
		case PSp_veryFast:
			DrawString(setText8, player2ScoreX, player2ScoreY);
			break;
		default:
			std::cout << "setting value error reseting to \"normal\"";
			set_paddelSpeed = PSp_Normal;
			break;
		}

		//change value
		if (IsKeyDown(263) && !keyDown) {//arrow left
			switch (set_paddelSpeed) {
			case PSp_verySlow:
				set_paddelSpeed = PSp_veryFast;
				break;
			case PSp_slow:
				set_paddelSpeed = PSp_verySlow;
				break;
			case PSp_Normal:
				set_paddelSpeed = PSp_slow;
				break;
			case PSp_fast:
				set_paddelSpeed = PSp_Normal;
				break;
			case PSp_veryFast:
				set_paddelSpeed = PSp_fast;
				break;
			}
			keyDown = true;
		}
		if (IsKeyDown(262) && !keyDown) {//arrow right
			switch (set_paddelSpeed) {
			case PSp_verySlow:
				set_paddelSpeed = PSp_slow;
				break;
			case PSp_slow:
				set_paddelSpeed = PSp_Normal;
				break;
			case PSp_Normal:
				set_paddelSpeed = PSp_fast;
				break;
			case PSp_fast:
				set_paddelSpeed = PSp_veryFast;
				break;
			case PSp_veryFast:
				set_paddelSpeed = PSp_verySlow;
				break;
			}
			keyDown = true;
		}

		//change state
		if (IsKeyDown(265) && !keyDown) {//arrow up
			currentSelection = paddelSize;
			keyDown = true;
		}
		if (IsKeyDown(264) && !keyDown) {//arrow down
			currentSelection = ballSpeed;
			keyDown = true;
		}

		break;
	case ballSpeed:

		DrawString(optText3, player1ScoreX, player1ScoreY);

		switch (set_ballSpeed) {
		case BSp_verySlow:
			DrawString(setText5, player2ScoreX, player2ScoreY);
			break;
		case BSp_slow:
			DrawString(setText6, player2ScoreX, player2ScoreY);
			break;
		case BSp_Normal:
			DrawString(setText0, player2ScoreX, player2ScoreY);
			break;
		case BSp_fast:
			DrawString(setText7, player2ScoreX, player2ScoreY);
			break;
		case BSp_veryFast:
			DrawString(setText8, player2ScoreX, player2ScoreY);
			break;
		default:
			std::cout << "setting value error reseting to \"normal\"";
			set_ballSpeed = BSp_Normal;
			break;
		}

		//change value
		if (IsKeyDown(263) && !keyDown) {//arrow left
			switch (set_ballSpeed) {
			case BSp_verySlow:
				set_ballSpeed = BSp_veryFast;
				break;
			case BSp_slow:
				set_ballSpeed = BSp_verySlow;
				break;
			case BSp_Normal:
				set_ballSpeed = BSp_slow;
				break;
			case BSp_fast:
				set_ballSpeed = BSp_Normal;
				break;
			case BSp_veryFast:
				set_ballSpeed = BSp_fast;
				break;
			}
			keyDown = true;
		}
		if (IsKeyDown(262) && !keyDown) {//arrow right
			switch (set_ballSpeed) {
			case BSp_verySlow:
				set_ballSpeed = BSp_slow;
				break;
			case BSp_slow:
				set_ballSpeed = BSp_Normal;
				break;
			case BSp_Normal:
				set_ballSpeed = BSp_fast;
				break;
			case BSp_fast:
				set_ballSpeed = BSp_veryFast;
				break;
			case BSp_veryFast:
				set_ballSpeed = BSp_verySlow;
				break;
			}
			keyDown = true;
		}

		//change state
		if (IsKeyDown(265) && !keyDown) {//arrow up
			currentSelection = paddelSpeed;
			keyDown = true;
		}
		if (IsKeyDown(264) && !keyDown){//arrow down
			currentSelection = scoreCap;
			keyDown = true;
		}
		break;

	default:
		std::cout << "current selection value error \n resetting to scoreCap\n";
		currentSelection = scoreCap;
		break;
	}

	if (!IsKeyDown(262) && !IsKeyDown(263) && !IsKeyDown(264) && !IsKeyDown(265)) {
		keyDown = false;
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