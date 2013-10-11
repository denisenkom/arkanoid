#define BOARD_SPEED	300
#define BOARD_THICKNESS		20
#define BOARD_WIDTH			100

enum ball_enum {RED_BALL, YELLOW_BALL, CYAN_BALL, GREEN_BALL, SILVER_BALL, FIREBALL};
enum block_enum {RED_BLOCK, YELLOW_BLOCK, CYAN_BLOCK, GREEN_BLOCK, SILVER_BLOCK, FIRE_BLOCK};
enum bound_enum {LEFT=2, TOP=1, RIGHT=3, BOTTOM=0};

class RectEnt;
class Ball;
class Board;
class Block;

void BallHitBounds(Ball &ball, bound_enum bound);
void BallHitBlock(Ball &ball, Block &block);
void BallHitBoard(Ball &ball, Board &board);
void BallHitBall(Ball &ball1, Ball &ball2);
bool IsIntersection(Ball&, Ball&);
bool IsIntersection(Ball&, RectEnt&);
bool IsIntersection(RectEnt&, RectEnt&);

class Entity
{
public:
	int id;
	int clip;
	int visible;
	float x, y;
	float width, height;
	float vx, vy;
	sprites_enum sprite;

	Entity(int id=0, int clip=true, int visible=true,
		float x=0, float y=0, float width=10, float height=10,
		float vx=0, float vy=0, sprites_enum sprite=BALL_RED);
	virtual void Hit(Entity& ent) {};
	virtual void HitBy(Ball& ball) = 0;
	virtual void HitBy(Board& board) {};
	virtual void HitBounds(bound_enum bound) {};
	virtual bool IsIntersectsWith(Entity&) = 0;
	virtual bool IsIntersectsWith(RectEnt &ent) = 0;
	virtual bool IsIntersectsWith(Ball &ent) = 0;
	virtual Entity* Clone() const = 0;
};

class RectEnt : public Entity
{
public:
	RectEnt(int id=0, int clip=true, int visible=true,
		float x=0, float y=0, float width=10, float height=10,
		float vx=0, float vy=0, sprites_enum sprite=BALL_RED)
		: Entity(id, clip, visible, x, y, width, height, vx, vy, sprite)
	{
	}
	virtual bool IsIntersectsWith(Entity &ent) {return ent.IsIntersectsWith(*this);};
	virtual bool IsIntersectsWith(RectEnt &rect) {return IsIntersection(rect, *this);};
	virtual bool IsIntersectsWith(Ball &ball) {return IsIntersection(ball, *this);};
};

class Block : public RectEnt
{
public:
	int animation;
	int health;
	int base_health;
	int base_sprite;

	Block(float x, float y, block_enum block);
	virtual void HitBy(Ball& ball) {BallHitBlock(ball, *this);};
	virtual Entity* Clone() const {return new Block(*this);};
};

class Board : public RectEnt
{
public:
	Board(float pos, color_enum color, bound_enum bound);
	virtual void HitBy(Ball& ball) {BallHitBoard(ball, *this);};
	virtual Entity* Clone() const {return new Board(*this);};
};

class Ball : public Entity
{
public:
	int damage;
	int owner;

	Ball(int owner, float x, float y, float vx, float vy, color_enum color);
	virtual void Hit(Entity& ent) {ent.HitBy(*this);};
	virtual void HitBy(Ball& ball) {BallHitBall(ball, *this);};
	//virtual void HitBy(Board& board) {BallHitBoard(*this, board);};
	virtual void HitBounds(bound_enum bound) {BallHitBounds(*this, bound);};
	void SetColor(color_enum color) {sprite = (sprites_enum)(BALL_RED + (int)color);};
	virtual Entity* Clone() const {return new Ball(*this);};
	virtual bool IsIntersectsWith(Entity &ent) {return ent.IsIntersectsWith(*this);};
	virtual bool IsIntersectsWith(RectEnt &ent) {return IsIntersection(*this, ent);};
	virtual bool IsIntersectsWith(Ball &ball) {return IsIntersection(ball, *this);};
};

namespace Game
{
// интерфейс для server
	extern Entity* entities[MAX_ENTITIES];
	extern player players[MAX_PLAYERS];
	extern bool paused;

	void Phisics();

	void MakeScene();
	void LoadMap(const char game_map[][20]);
	void SpawnBoard(int pl_id);
	void SpawnEntity(const Entity &entity);
};