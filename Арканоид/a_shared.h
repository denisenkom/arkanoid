// shared by all modules

#define NET_MAXMESSAGE	1024*4
#define NET_MAXNAME	64
#define MAX_PATHNAME	64

#define GAME_X	0
#define GAME_Y	((SCN_HEIGHT-GAME_HEIGHT)/2)
#define BACK_WIDTH	256
#define BACK_HEIGHT	256
#define SPRITES_WIDTH	300
#define SPRITES_HEIGHT	230

#define GAME_WIDTH	400
#define GAME_HEIGHT	400

#define MOVE_LEFT		1
#define MOVE_RIGHT	2
#define NO_MOVE		3

#define SCN_WIDTH	640
#define SCN_HEIGHT	480
#define SCN_BPP	16
#define SCN_REFRESH	75

#define COLOR_KEY		(31|31<<11)

#define MAX_IMAGE_CACHE	5

#ifndef _DEBUG
#define ASSERT(x, msg)
#endif

enum sv_cmd {SV_NOP, SV_SEND_INFO, SV_SYNC_ENT, SV_SOUND, /*SV_SPAWN_BOARD, */
	/*SV_SPAWN_BALL, SV_SPAWN_BLOCK,*/ SV_UPDATE_PLAYERS_LIST, SV_PRINT, SV_START_GAME};

#define UPDATE_POS	0x01
#define UPDATE_VIS	0x02
#define UPDATE_SPR	0x04

enum sound_enum {BALL_HIT=0, RICOCHET, BALL_HIT_EDGE, BALL_HIT_BLOCK, BALL_HIT_BOARD};

enum sprites_enum {BLOCK_RED100=0, BLOCK_RED75, BLOCK_RED50, BLOCK_RED25,
	BLOCK_YELLOW100, BLOCK_YELLOW75, BLOCK_YELLOW50, BLOCK_YELLOW25,
	BLOCK_CYAN100, BLOCK_CYAN75, BLOCK_CYAN50, BLOCK_CYAN25,
	BLOCK_GREEN100, BLOCK_GREEN75, BLOCK_GREEN50, BLOCK_GREEN25,
	BLOCK_SILVER100, BLOCK_SILVER75, BLOCK_SILVER50, BLOCK_SILVER25,
	BLOCK_FIRE1, BLOCK_FIRE2, BLOCK_FIRE3, BLOCK_FIRE4,
	BALL_RED, BALL_YELLOW, BALL_CYAN, BALL_GREEN, BALL_SILVER,
	BALL_REDS, BALL_YELLOWS, BALL_CYANS, BALL_GREENS, BALL_SILVERS,
	BOARDH_RED, BOARDH_YELLOW, BOARDH_CYAN, BOARDH_GREEN,
	BOARDV_RED, BOARDV_YELLOW, BOARDV_CYAN, BOARDV_GREEN,
	SCORE_PANEL
};

enum color_enum {RED=0, YELLOW, CYAN, GREEN, SILVER};

struct rect_s {
	int left, top, width, height;
};

#define KEYS_NUM	6

#define KEY_UP	0
#define KEY_DOWN	1
#define KEY_LEFT	2
#define KEY_RIGHT	3
#define KEY_ESC	4
#define KEY_OTHER	5

class SizeBuffer {
public:
	bool overflowed;
	bool allowOverflow;
	int cursize;
	int maxsize;
	char* data;

	SizeBuffer(int startsize);
	~SizeBuffer();

	void Alloc(int startsize);
	void Free();
	void Clear();
	void Write(const char* data, int length);
	void Print(const char* str);
};

enum cl_cmd {CL_NOP, CL_PRINT, CL_INFO, CL_MOVE, CL_DROP};

class Message {
private:
	const char* msg;
	char* readpos;
public:
	SizeBuffer buffer;

	Message();

	void writeFloat(float x);
	void writeByte(char x);
	void writeLong(long x);
	void writeStr(const char* str);

	void beginRead();
	float readFloat();
	char readByte();
	long readLong();
	char* readStr();
	bool eom();
};

#define MAX_ENTITIES	256
#define MAX_PLAYERS	4

struct player {
	int active;
	char name[256];
	int pts;
	color_enum color;
};

template<typename Type>
class vec2 {
public:
	Type x;
	Type y;


	vec2(Type x, Type y) : x(x), y(y) {}

	Type length() const { return sqrt(x * x + y * y); }
	void normalize() { Type len = length(); if( len == Type(1.0) ) return; x /= len; y /= len; }
	void mirror(const vec2<Type>& normal);
	Type dotProduct(const vec2<Type>& right) const;
	vec2& operator *=(Type factor) { x *= factor; y *= factor; return *this; }
	vec2& operator +=(const vec2<Type>& right) { x += right.x; y += right.y; return *this; }
	vec2 operator +(const vec2<Type>& right) { vec2<Type> result(*this); return result += right; }
	vec2& rotate90cw() { Type saveY = y; y = -x; x = saveY; return *this; }
	vec2& rotate90ccw() { Type saveY = y; y = x; x = -saveY; return *this; }
};

template<typename Type>
Type vec2<Type>::dotProduct(const vec2<Type>& right) const {
	return x * right.x + y * right.y;
}

template<typename Type>
void vec2<Type>::mirror(const vec2<Type>& normal) {
	vec2<Type> normalComponent(normal);
	normalComponent *= normal.dotProduct(*this);

	vec2<Type> normal90ccw(normal);
	normal90ccw.rotate90ccw();

	vec2<Type> normal90ccwComponent(normal90ccw);
	normal90ccwComponent *= normal90ccw.dotProduct(*this);

	*this = normalComponent + normal90ccwComponent;
}
