#ifndef __j1DOOR_H__
#define __j1DOOR_H__

#include "j1Entity.h"
#include "p2Point.h"

struct Collider;
struct SDL_Texture;

enum ENTITY_TYPE;

class j1Door : public j1Entity {

public:

	j1Door(int x, int y, ENTITY_TYPE type);
	virtual ~j1Door();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	fPoint door_position;

	Animation idle;
	Animation openingAnim;

	bool opened = false;
	bool playedFx = false;
};

#endif