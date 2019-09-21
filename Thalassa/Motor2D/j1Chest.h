#ifndef __j1CHEST_H__
#define __j1CHEST_H__

#include "j1Entity.h"
#include "p2Point.h"

struct Collider;
struct SDL_Texture;

enum ENTITY_TYPE;

class j1Chest : public j1Entity {

public:

	j1Chest(int x, int y, ENTITY_TYPE type);
	virtual ~j1Chest();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	Animation idle;
	Animation openingAnim;
	Animation openedAnim;

	bool opened = false;
};

#endif