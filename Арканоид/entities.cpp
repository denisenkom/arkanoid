#include <stdexcept>
#include <assert.h>

#include "a_shared.h"
#include "Game.h"

Entity::Entity(int id, int clip, int visible, float x, float y, float width, float height, float vx, float vy, sprites_enum sprite) :
	id(id), clip(clip), visible(visible), x(x), y(y), width(width), height(height), vx(vx), vy(vy), sprite(sprite)
{
}

Block::Block(float x, float y, block_enum block)
: RectEnt(0, true, true, x, y, 20, 20, 0, 0), animation(false)
{
	switch (block) {
	case RED_BLOCK:
		base_sprite = sprite = BLOCK_RED100;
		base_health = health = 100;
		break;
	case YELLOW_BLOCK:
		base_sprite = sprite = BLOCK_YELLOW100;
		base_health = health = 100;
		break;
	case CYAN_BLOCK:
		base_sprite = sprite = BLOCK_CYAN100;
		base_health = health = 100;
		break;
	case GREEN_BLOCK:
		base_sprite = sprite = BLOCK_GREEN100;
		base_health = health = 100;
		break;
	case SILVER_BLOCK:
		base_sprite = sprite = BLOCK_SILVER100;
		base_health = health = 5;
		break;
	case FIRE_BLOCK:
		base_sprite = sprite = BLOCK_FIRE1;
		animation = true;
		base_health = health = 5;
		break;
	default:
		throw std::exception("Block::Init() Направильное (или неизвестное) значение параметра block.");
	}
}

Ball::Ball(int owner, float x, float y, float vx, float vy, color_enum color)
:
	Entity(0, true, true, x, y, 20, 20, vx, vy, (sprites_enum)(BALL_RED + (int)color)),
	owner(owner),
	damage(25)
{
}

Board::Board(float pos, color_enum color, bound_enum bound) 
{
	switch (bound)
	{
	case TOP:
		y = GAME_HEIGHT - BOARD_THICKNESS;
	case BOTTOM:
		x = pos;
		width = BOARD_WIDTH;
		height = BOARD_THICKNESS;
		sprite = (sprites_enum)(BOARDH_RED + (int)color);
		break;
	case LEFT:
		x = 0;
	case RIGHT:
		y = pos;
		width = BOARD_THICKNESS;
		height = BOARD_WIDTH;
		sprite = (sprites_enum)(BOARDV_RED + (int)color);
		break;
	default:
		assert(false);
	}

	switch (bound)
	{
	case BOTTOM:
		y = 0;
		break;
	case RIGHT:
		x = GAME_WIDTH - BOARD_THICKNESS;
		break;
	}
}