#include <stdexcept>
#include <math.h>
#include <assert.h>
#include <geometry\geometry.h>

#include "a_shared.h"
#include "Game.h"
#include "Server.h"
#include "Host.h"

/* local routines */

static bool ptInRect(double x, double y, double rx1, double ry1, double rx2, double ry2)
{
	return (x >= rx1 && x <= rx2 && y >= ry1 && y <= ry2);
}

static bool isIntersects(double x1, double y1, double width1, double height1, double x2, double y2, double width2, double height2)
{
	double r1x1 = x1;
	double r1y1 = y1;
	double r1x2 = x1 + width1;
	double r1y2 = y1 + height1;

	double r2x1 = x2;
	double r2y1 = y2;
	double r2x2 = x2 + width2;
	double r2y2 = y2 + height2;

	return (ptInRect(r1x1, r1y1, r2x1, r2y1, r2x2, r2y2) ||
		ptInRect(r1x2, r1y1, r2x1, r2y1, r2x2, r2y2) ||
		ptInRect(r1x2, r1y2, r2x1, r2y1, r2x2, r2y2) ||
		ptInRect(r1x1, r1y2, r2x1, r2y1, r2x2, r2y2) ||

		ptInRect(r2x1, r2y1, r1x1, r1y1, r1x2, r1y2) ||
		ptInRect(r2x2, r2y1, r1x1, r1y1, r1x2, r1y2) ||
		ptInRect(r2x2, r2y2, r1x1, r1y1, r1x2, r1y2) ||
		ptInRect(r2x1, r2y2, r1x1, r1y1, r1x2, r1y2));
}

#define CHECK_TOP		1
#define CHECK_BOTTOM	2
#define CHECK_LEFT	4
#define CHECK_RIGHT	8

static bool _RepairIntersection(Entity& ent1, Entity& ent2, double b1, unsigned check)
{
	if (ent1.vx > 0 && (check & CHECK_RIGHT))
	{
		double y1 = (ent1.vy*ent2.x - b1)/ent1.vx;
		if (y1 >= ent2.y && y1 <= ent2.y + ent2.height)
		{ // hit from right
			ent1.x -= 2*(ent1.x + ent1.width - ent2.x);
			ent1.vx *= -1;
			return true;
		}
	}
	else if (ent1.vx < 0 && (check & CHECK_LEFT))
	{
		double y1 = (ent1.vy*(ent2.x + ent2.width) - b1)/ent1.vx;
		if (y1 >= ent2.y && y1 <= ent2.y + ent2.height)
		{ // hit from right
			ent1.x -= 2*(ent1.x - ent2.x - ent2.width);
			ent1.vx *= -1;
			return true;
		}
	}
	if (ent1.vy > 0 && (check & CHECK_BOTTOM))
	{
		double x2 = (ent1.vx*ent2.y + b1)/ent1.vy;
		if (x2 >= ent2.x && x2 <= ent2.x + ent2.width)
		{// hit from bottom
			ent1.y -= 2*(ent1.y + ent1.height - ent2.y);
			ent1.vy *= -1;
			return true;
		}
	}
	else if (ent1.vy < 0 && (check & CHECK_TOP))
	{
		double x2 = (ent1.vx*(ent2.y + ent2.height) + b1)/ent1.vy;
		if (x2 >= ent2.x && x2 <= ent2.x + ent2.width)
		{// hit from bottom
			ent1.y -= 2*(ent1.y - ent2.y - ent2.height);
			ent1.vy *= -1;
			return true;
		}
	}
	return false;
}

static void RepairIntersection(Entity& ent1, Entity& ent2)
{
	if (ent1.vx >= 0 || ent1.vy >= 0)
	{
		// верхняя левая точка
		double b1 = ent1.vy*(ent1.x + ent1.width) - ent1.vx*(ent1.y + ent1.height);
		if (_RepairIntersection(ent1, ent2, b1, CHECK_BOTTOM | CHECK_RIGHT))
			return;
	}
	if (ent1.vx >= 0 || ent1.vy <= 0)
	{
		// нижняя левая точка
		double b1 = ent1.vy*(ent1.x + ent1.width) - ent1.vx*ent1.y;
		if (_RepairIntersection(ent1, ent2, b1, CHECK_RIGHT | CHECK_TOP))
			return;
	}
	if (ent1.vx <= 0 || ent1.vy <= 0)
	{
		// нижняя правая точка
		double b1 = ent1.vy*ent1.x - ent1.vx*ent1.y;
		if (_RepairIntersection(ent1, ent2, b1, CHECK_TOP | CHECK_LEFT))
			return;
	}
	if (ent1.vx <= 0 || ent1.vy >= 0)
	{
		// верхняя правая точка
		double b1 = ent1.vy*ent1.x - ent1.vx*(ent1.y + ent1.height);
		if (_RepairIntersection(ent1, ent2, b1, CHECK_BOTTOM | CHECK_LEFT))
			return;
	}
}

/* Implementation */

bool Game::paused = true;
Entity* Game::entities[MAX_ENTITIES] = {0};
player Game::players[4] = {0};

void ReflectBall(Ball &ball, float nx, float ny, float rel_spd)
{
	using MyGeometryTools::vec2;
	using MyGeometryTools::matrix2;
	/*float sinb, cosb;
	sinb = -ny;
	cosb = nx;
	vec2<float> newv = matrix2<float>(cosb, sinb,  -sinb, cosb)*matrix2<float>(-cosb, -sinb,  -sinb, cosb)*vec2<float>(ball.vx, ball.vy);*/
	vec2<float> newv(ball.vx, ball.vy);
	newv.mirror(vec2<float>(nx, ny));
	newv *= rel_spd;
	ball.vx = newv.x;
	ball.vy = newv.y;
}

bool IsIntersection(Ball &ball1, Ball &ball2)
{
	float rad1 = ball1.width/2, rad2 = ball2.width/2;
	float cx1 = ball1.x + rad1, cy1 = ball1.y + rad1,
		cx2 = ball2.x + rad2, cy2 = ball2.y + rad2;

	float range = sqrtf((cx1 - cx2)*(cx1 - cx2) + (cy1 - cy2)*(cy1 - cy2));

	if (range <= rad1 + rad2)
	{
		// модули скоростей шаров
		float speed1 = sqrtf(ball1.vx*ball1.vx + ball1.vy*ball1.vy),
			speed2 = sqrtf(ball2.vx*ball2.vx + ball2.vy*ball2.vy);
		/*ball1.x += ball1.vx*correct/speed1;
		ball1.y += ball1.vy*correct/speed1;
		ball2.x += ball2.vx*correct/speed2;
		ball2.y += ball2.vy*correct/speed2;*/
		// новая скорость шаров есть половина суммы бывших скоростей, из закона сохранения
		// импульса и т.к. массы шаров одинаковы
		float newspeed = (speed1 + speed2)/2;
		using MyGeometryTools::vec2;
		vec2<float> normal(cx2 - cx1, cy2 - cy1);
		normal.normalize();
		ReflectBall(ball1, normal.x, normal.y, newspeed/speed1);
		ReflectBall(ball2, -normal.x, -normal.y, newspeed/speed2);
		float correct = (range - rad1 + rad2)/2/newspeed;
		ball1.x += ball1.vx*correct;
		ball1.y += ball1.vy*correct;
		ball2.x += ball2.vx*correct;
		ball2.y += ball2.vy*correct;
		Server::Sound(BALL_HIT, 1, 6000);
		{
	float rad1 = ball1.width/2, rad2 = ball2.width/2;
	float cx1 = ball1.x + rad1, cy1 = ball1.y + rad1,
		cx2 = ball2.x + rad2, cy2 = ball2.y + rad2;

	float range = sqrtf((cx1 - cx2)*(cx1 - cx2) + (cy1 - cy2)*(cy1 - cy2));

	assert(range > rad1 + rad2);
		}
	}
	return false;
}

bool IsIntersection(Ball &ball, RectEnt &rect)
{
	/*float rad = ball.width/2;
	float cx = ball.x + rad, cy = ball.y + rad,
	return false;*/
	return isIntersects(ball.x, ball.y, ball.width, ball.height,
		rect.x, rect.y, rect.width, rect.height);
}

bool IsIntersection(RectEnt &rect1, RectEnt &rect2)
{
	return isIntersects(rect1.x, rect1.y, rect1.width, rect1.height,
		rect2.x, rect2.y, rect2.width, rect2.height);
}

void BallHitBlock(Ball &ball, Block &block)
{	
	block.health -= ball.damage;
	Game::players[ball.owner].pts += ball.damage;
	block.sprite = (sprites_enum)(block.base_sprite + 4 - block.health*4/block.base_health);
	if (block.health <= 0)
	{
		block.clip = false;
		block.visible = false;
	}
	Server::UpdatePlayersList();
	Server::Sound(BALL_HIT_BLOCK, 1, 6000);
	Server::datagramm.writeByte(SV_SYNC_ENT);
	Server::datagramm.writeLong(block.id);
	Server::datagramm.writeByte(UPDATE_VIS | UPDATE_SPR);
	Server::datagramm.writeByte(block.visible);
	Server::datagramm.writeLong(block.sprite);
}

void BallHitBounds(Ball &ball, bound_enum bound)
{
	Server::Sound(BALL_HIT, 1, 6000);
}

void BallHitBall(Ball &ball1, Ball &ball2)
{
	Server::Sound(BALL_HIT, 1, 6000);
}

void BallHitBoard(Ball &ball, Board &board)
{
	float delta = ball.x + ball.width/2.0f - board.x - board.width/2.0f;
	if (delta >= -30.0f && delta <= 30.0f)
		Server::Sound(BALL_HIT_BOARD, 1, 8000);
	else
		Server::Sound(BALL_HIT_EDGE, 1, 6000);
	ball.vx = delta*4.0f /*+ board.vx/10.0f*/;
	if (ball.owner != board.id)
	{
		ball.owner = board.id;
		ball.SetColor(Game::players[board.id].color);
		Server::datagramm.writeByte(SV_SYNC_ENT);
		Server::datagramm.writeLong(ball.id);
		Server::datagramm.writeByte(UPDATE_SPR);
		Server::datagramm.writeLong(ball.sprite);
	}
}

void Game::Phisics()
{
	if (paused || !Server::active)
		return;

	for (int id = 0; id < sizeof(entities)/sizeof(entities[0]); id++)
	{
		Entity* ent = Game::entities[id];
		if (!ent)
			continue;

		if (ent->vx == 0 && ent->vy == 0)
			continue;
	
		ent->x += ent->vx*Host::frametime;
		ent->y += ent->vy*Host::frametime;

		if (!ent->clip)
			continue;

		// clip to game field
		//bool still_intersect = false;
		//do {
			if (ent->x < 0)
			{
				ent->x = -ent->x;
				ent->vx *= -1;
				//still_intersect = true;
				ent->HitBounds(LEFT);
			}
			if (ent->y < 0)
			{
				ent->y = -ent->y;
				ent->vy *= -1;
				//still_intersect = true;
				ent->HitBounds(BOTTOM);
			}
			if (ent->x > GAME_WIDTH - ent->width)
			{
				ent->x = GAME_WIDTH - ent->width - 0.1f;
				ent->vx *= -1;
				//still_intersect = true;
				ent->HitBounds(RIGHT);
			}
			if (ent->y > GAME_HEIGHT - ent->height)
			{
				ent->y = GAME_HEIGHT - ent->height - 0.1f;
				ent->vy *= -1;
				//still_intersect = true;
				ent->HitBounds(TOP);
			}

			// проверка пересечений с другими элементами
			for (int i = 0; i < MAX_ENTITIES; i++)
			{
				if (i != id
					&& Game::entities[i] != 0
					&& Game::entities[i]->clip
					&& ent->IsIntersectsWith(*Game::entities[i])
					/*isIntersects(ent->x, ent->y, ent->width, ent->height,
						Game::entities[i]->x, Game::entities[i]->y, Game::entities[i]->width, Game::entities[i]->height)*/
				)
				{	
					RepairIntersection(*ent, *Game::entities[i]);
					//still_intersect = true;
					ent->Hit(*Game::entities[i]);
				}
			}
			//if (still_intersect)
			//	Server::Sound(BALL_HIT, 1);
		//}
		//while(still_intersect);

		Server::datagramm.writeByte(SV_SYNC_ENT);
		Server::datagramm.writeLong(id);
		Server::datagramm.writeByte(UPDATE_POS);
		Server::datagramm.writeFloat((float)ent->x);
		Server::datagramm.writeFloat((float)ent->y);
	}
}

static void FullSyncEnt(Entity &ent)
{
	Server::datagramm.writeByte(SV_SYNC_ENT);
	Server::datagramm.writeLong(ent.id);
	Server::datagramm.writeByte(UPDATE_POS | UPDATE_VIS | UPDATE_SPR);
	Server::datagramm.writeFloat(ent.x);
	Server::datagramm.writeFloat(ent.y);
	Server::datagramm.writeByte(ent.visible);
	Server::datagramm.writeLong(ent.sprite);
}

void Game::SpawnBoard(int pl_id)
{
	assert(pl_id >= 0 && pl_id < 4);
	Board* board = new Board(GAME_WIDTH/2-BOARD_WIDTH/2, players[pl_id].color, (bound_enum)pl_id);

	if (entities[pl_id] != 0)
		delete entities[pl_id];

	entities[pl_id] = board;
	board->id = pl_id;

	FullSyncEnt(*board);
}

void Game::SpawnEntity(const Entity &entity)
{
	unsigned i;
	for (i = 4; i < MAX_ENTITIES; i++)
		if (Game::entities[i] == 0)
			break;
	if (i == MAX_ENTITIES)
	{
		assert(false);
		//Log_Print("Нельзя создать шарик, нет свободных слотов.");
		return;
	}
	entities[i] = entity.Clone();
	entities[i]->id = i;
	FullSyncEnt(*entities[i]);
}

void Game::LoadMap(const char game_map[][20])
{
	block_enum block_enum = RED_BLOCK;

	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++)
			if (game_map[i][j] != 0) {
				switch (game_map[i][j]) {
				case 1:
					block_enum = RED_BLOCK;
					break;
				case 2:
					block_enum = YELLOW_BLOCK;
					break;
				case 3:
					block_enum = CYAN_BLOCK;
					break;
				case 4:
					block_enum = SILVER_BLOCK;
					break;
				case 5:
					block_enum = FIRE_BLOCK;
					break;
				}
				Block block((float)j*20, (float)i*20, block_enum);
				SpawnEntity(block);
			}
}
