#ifndef __j1PLATFORM_H__
#define __j1PLATFORM_H__

#include "j1Entity.h"
#include "p2Point.h"

struct Collider;
struct SDL_Texture;

enum ENTITY_TYPE;

class j1Platform : public j1Entity {

public:

	j1Platform(int x, int y, ENTITY_TYPE type, iPoint limit, int typePlatform, bool vertical);
	virtual ~j1Platform();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void Move(float dt);
	void MoveVertical(float dt);

	void Properties();

public:
	
	Animation idle;

	bool arrived_limitX = false;
	bool arrived_limitY = false;

private:

	bool isVertical = false;
	iPoint limits;
	fPoint speed;

	float initial_position;
	int type_platform;

	float limit_left, limit_right, limit_up, limit_down;

	iPoint hitbox;
};

#endif