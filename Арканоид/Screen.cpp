#include <exception>
#include <stdlib.h>

#include "a_shared.h"
#include "Screen.h"
#include "Graphics.h"
#include "Client.h"
#include "log.h"
#include "Realtime.h"

/* local variables */
static rect_s sprites_rects[] = {
	{0, 0, 20, 20},	// BLOCK_RED100
	{0, 20, 20, 20},	// BLOCK_RED75
	{0, 40, 20, 20},	// BLOCK_RED50
	{0, 60, 20, 20},	// BLOCK_RED25

	{20, 0, 20, 20},	// BLOCK_YELLOW100
	{20, 20, 20, 20},	// BLOCK_YELLOW75
	{20, 40, 20, 20},	// BLOCK_YELLOW50
	{20, 60, 20, 20},	// BLOCK_YELLOW25

	{40, 0, 20, 20},	// BLOCK_CYAN100
	{40, 20, 20, 20},	// BLOCK_CYAN75
	{40, 40, 20, 20},	// BLOCK_CYAN50
	{40, 60, 20, 20},	// BLOCK_CYAN25

	{60, 0, 20, 20},	// BLOCK_GREEN100
	{60, 20, 20, 20},	// BLOCK_GREEN75
	{60, 40, 20, 20},	// BLOCK_GREEN50
	{60, 60, 20, 20},	// BLOCK_GREEN25

	{80, 0, 20, 20},	// BLOCK_SILVER100
	{80, 20, 20, 40},	// BLOCK_SILVER75
	{80, 40, 20, 60},	// BLOCK_SILVER50
	{80, 60, 20, 80},	// BLOCK_SILVER25

	{100, 0, 20, 20},	// BLOCK_FIRE1
	{100, 20, 20, 20},	// BLOCK_FIRE2
	{100, 40, 20, 20},	// BLOCK_FIRE3
	{100, 60, 20, 20},	// BLOCK_FIRE4

	{0, 80, 20, 20},	// BALL_RED
	{20, 80, 20, 20},	// BALL_YELLOW
	{40, 80, 20, 20},	// BALL_CYAN
	{60, 80, 20, 20},	// BALL_GREEN
	{80, 80, 20, 20},	// BALL_SILVER

	{0, 100, 10, 10},	// BALL_RED
	{10, 100, 10, 10},	// BALL_YELLOW
	{20, 100, 10, 10},	// BALL_CYAN
	{30, 100, 10, 10},	// BALL_GREEN
	{40, 100, 10, 10},	// BALL_SILVER

	{120, 0, 100, 20},	// BOARDH_RED
	{120, 20, 100, 20},	// BOARDH_YELLOW
	{120, 40, 100, 20},	// BOARDH_CYAN
	{120, 60, 100, 20},	// BOARDH_GREEN

	{220, 0, 20, 100},	// BOARDV_RED
	{240, 0, 20, 100},	// BOARDV_YELLOW
	{260, 0, 20, 100},	// BOARDV_CYAN
	{280, 0, 20, 100},	// BOARDV_GREEN

	{0, 110, 240, 120}	// SCORE_SPRITE
};

static int sprites_img = 0;
static int backgr_img = 0;

/* local routines */

static void DrawSprite(int scx, int scy, sprites_enum sprite)
{
	if (sprite == BLOCK_FIRE1 || sprite == BLOCK_FIRE2 || sprite == BLOCK_FIRE3 || sprite == BLOCK_FIRE4)
	{
		int msec = RealTime::getTime()*1000;
		sprite = (sprites_enum)(BLOCK_FIRE1+(msec/200)%4);
	}
	rect_s rect = sprites_rects[sprite];
	if (scx < 0)
	{
		rect.left -= scx;
		scx = 0;
	}
	//if (sprite == BALL_RED)
	//	Graphics::DrawTransparentImg(sprites_img, scx, scy, rect);
	//else
		Graphics::DrawImg(sprites_img, scx, scy, rect);
}

/* Implementation */

void Screen::Init()
{
	Log_Print("Loading sprites...\n");
	sprites_img = Graphics::LoadImg("gfx/sprites.bmp");
	if (sprites_img == -1)
		throw std::exception("Failed to load sprites.");

	Log_Print("Loading background...\n");
	backgr_img = Graphics::LoadImg("gfx/back4.bmp");
	if (backgr_img == -1)
		throw std::exception("Failed to load background.");
}

void Screen::Update()
{
	static double time1, time2, time3, time4;
	time1 = RealTime::getTime();
	Graphics::Clear();

	DrawSprite(0, GAME_Y-20, BOARDH_YELLOW);
	DrawSprite(100, GAME_Y-20, BOARDH_YELLOW);
	DrawSprite(200, GAME_Y-20, BOARDH_YELLOW);
	DrawSprite(300, GAME_Y-20, BOARDH_YELLOW);

	DrawSprite(0, GAME_Y+GAME_HEIGHT, BOARDH_RED);
	DrawSprite(100, GAME_Y+GAME_HEIGHT, BOARDH_RED);
	DrawSprite(200, GAME_Y+GAME_HEIGHT, BOARDH_RED);
	DrawSprite(300, GAME_Y+GAME_HEIGHT, BOARDH_RED);

	time2 = RealTime::getTime();

	rect_s rect = {0, 0, BACK_WIDTH, BACK_HEIGHT};

	Graphics::DrawImg(backgr_img, GAME_X, GAME_Y, rect);

	rect.width = GAME_WIDTH-BACK_WIDTH-rect.left;
	Graphics::DrawImg(backgr_img, GAME_X+BACK_WIDTH, GAME_Y, rect);

	rect.width = GAME_WIDTH-BACK_WIDTH-rect.left;
	rect.height = GAME_HEIGHT-BACK_HEIGHT-rect.top;
	Graphics::DrawImg(backgr_img, GAME_X+BACK_WIDTH, GAME_Y+BACK_HEIGHT, rect);

	rect.width = BACK_WIDTH-rect.left;
	rect.height = GAME_HEIGHT-BACK_HEIGHT-rect.top;
	Graphics::DrawImg(backgr_img, GAME_X, GAME_Y+BACK_HEIGHT, rect);

	{for (int i = 0; i < MAX_ENTITIES; i++) {
		if (Client::entities[i].visible)
			DrawSprite(GAME_X + (int)Client::entities[i].x, GAME_Y + GAME_HEIGHT - (int)(Client::entities[i].y + sprites_rects[Client::entities[i].sprite].height), Client::entities[i].sprite);
	}}

	//char stats[1024];
	//sprintf(stats, "SLEEP: %i", Arkanoid::sleep_counter);
	//Graphics::DrawText(0, 40, stats);

	for (int i = 0; i < 4; i++)
	{
		DrawSprite(SCN_WIDTH-240, i*120, SCORE_PANEL);
		Graphics::DrawTxt(SCN_WIDTH-240+110, i*120+35, Client::players[i].name);
		char _pts[256];
		sprintf(_pts, "%i", Client::players[i].pts);
		Graphics::DrawTxt(SCN_WIDTH-240+110, i*120+70, _pts);
	}

	time3 = RealTime::getTime();

	//if (Game::paused)
	//	Graphics::DrawTxt(SCN_WIDTH/2-20, SCN_HEIGHT/2, "PAUSED");

	char str[30];
	float alltime = time3 - time4;
	float dt1 = time2 - time1;
	float dt2 = time3 - time2;

	sprintf(str, "fps: %d,\t%%1:%d,\t%%2:%d", int(1.0/alltime), int(dt1/alltime*100), int(dt2/alltime*100));
	Graphics::DrawTxt(5, 10, str);

	Graphics::Flush();

	time4 = RealTime::getTime();

	//Log_Print("background %f : ent %f : flush %f\n", time2-time1, time3-time2, time4-time3);
}
