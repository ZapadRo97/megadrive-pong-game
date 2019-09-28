#include <genesis.h>
#include "res/resources.h"

enum stateEnum {FIRST_START, RUNNING, PAUSED, LOST};
enum stateEnum gameState = FIRST_START;
bool updateNeeded = FALSE;

const int LEFT_EDGE = 0;
const int TOP_EDGE = 0;
const int RIGHT_EDGE = 320;
const int BOTTOM_EDGE = 224;

Sprite* ball;
Sprite* player;

Sprite* lives[3];
int num_lives = 3;
int total_score = 0;

int ball_pos_x = 156;
int ball_pos_y = 108;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

int player_pos_x = 144;
int player_pos_y = 200;
int player_vel_x = 0;
int player_width = 32;
int player_height = 8;

void checkCollision()
{
	if(ball_pos_y + ball_height >= player_pos_y 
	&& ball_pos_x + (ball_width >> 1) >= player_pos_x 
	&& ball_pos_x - (ball_width >> 1) <= player_pos_x + player_width) 
	{
		//we have collision
		//ball_vel_x = -ball_vel_x;
		ball_vel_y = -ball_vel_y;

		total_score++;

	}
}

void initLives()
{
	num_lives = 3;
	lives[0] = SPR_addSprite(&imgheart, 258, 8, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	lives[1] = SPR_addSprite(&imgheart, 276, 8, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	lives[2] = SPR_addSprite(&imgheart, 294, 8, TILE_ATTR(PAL1, 0, FALSE, FALSE));
}

void resetBall() 
{
	ball_pos_x = 156;
	ball_pos_y = 108;
	ball_vel_x = 1;
	ball_vel_y = 1;

	SPR_releaseSprite(lives[num_lives-1]);
	num_lives--;

	if(num_lives == 0) {
		gameState = LOST;
		updateNeeded = TRUE;
	}
		
}

void resetScore() 
{
	initLives();
	total_score = 0;
}

void moveBall()
{

	if(ball_pos_x < LEFT_EDGE) 
	{
		ball_pos_x = LEFT_EDGE;
		ball_vel_x = -ball_vel_x;
	} else if(ball_pos_x + ball_width > RIGHT_EDGE) {
		ball_pos_x = RIGHT_EDGE - ball_width;
		ball_vel_x = -ball_vel_x;
	}

	if(ball_pos_y < TOP_EDGE) 
	{
		ball_pos_y = TOP_EDGE;
		ball_vel_y = -ball_vel_y;
	} else if(ball_pos_y > BOTTOM_EDGE) {
		//ball_pos_y = BOTTOM_EDGE - ball_height;
		//ball_vel_y = -ball_vel_y;
		resetBall();

	}

	ball_pos_x += ball_vel_x;
	ball_pos_y += ball_vel_y;
	SPR_setPosition(ball, ball_pos_x, ball_pos_y);
}

void positionPlayer()
{
	player_pos_x += player_vel_x;
	if(player_pos_x < LEFT_EDGE)
		player_pos_x = LEFT_EDGE;
	
	if(player_pos_x + player_width > RIGHT_EDGE)
		player_pos_x = RIGHT_EDGE - player_width;

	SPR_setPosition(player, player_pos_x, player_pos_y);
}

void mainJoyHandler(u16 joy, u16 changed, u16 state) {
	if(joy == JOY_1)
	{
		if(state & BUTTON_START) {
			if(gameState == FIRST_START || gameState == PAUSED) {
				gameState = RUNNING;
			} else
			if(gameState == RUNNING) {
				gameState = PAUSED;
			} else
			if(gameState == LOST) {
				gameState = RUNNING;
				resetScore();
			}

			updateNeeded = TRUE;
		}
		

		if(state & BUTTON_RIGHT)
		{
			player_vel_x = 3;
		} else if(state & BUTTON_LEFT)
		{
			player_vel_x = -3;
		} else 
		{
			if( (changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT) )
			{
				player_vel_x = 0;
			}
		}
	}
}

char* startText = "PRESS START TO PLAY";
char* loseText = "GAME OVER";
char* pauseText = "PAUSED";
short startTextOffset = 11;
short loseTextOffset = 15;
short pauseTextOffset = 17;

const short infoLine = 15;

int main()
{
	//Do cool stuff!
	VDP_loadTileSet(bgtile.tileset, 1, DMA);
	//we have all the color data inside this image
	VDP_setPalette(PAL1, bgtile.palette->data);
	VDP_fillTileMapRect(PLAN_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 0, 40, 30);

	//init sprite engine
	SPR_init(0,0,0);
	ball = SPR_addSprite(&imgball, ball_pos_x, ball_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	player = SPR_addSprite(&imgpaddle, player_pos_x, player_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	initLives();

	VDP_drawText("SCORE", 1, 1);
	VDP_drawText(startText,startTextOffset ,infoLine);

	//init input
	JOY_init();
	JOY_setEventHandler(&mainJoyHandler);

	char *scoreText[16];

	while(1)
	{
		if(gameState == RUNNING) {
			checkCollision();
			moveBall();
			positionPlayer();
		}

		if(updateNeeded) {
			VDP_clearTextLine(infoLine);

			if(gameState == PAUSED)
				VDP_drawText(pauseText, pauseTextOffset, infoLine);

			if(gameState == LOST)
				VDP_drawText(loseText, loseTextOffset, infoLine);

			updateNeeded = FALSE;
		}
		

		sprintf(scoreText, "%d", total_score);
		VDP_drawText(scoreText, 1, 2);

		SPR_update();
		VDP_waitVSync();
	}

	//Stop doing cool stuff :(
	return(0);
}

 