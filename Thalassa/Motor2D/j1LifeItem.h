#ifndef __j1LIFEITEM_H__
#define __j1LIFEITEM_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Particle.h"
#include "j1Pathfinding.h"

struct SDL_Texture;
struct Collider;

enum ENTITY_TYPE;

class j1LifeItem : public j1Entity {

public:

	j1LifeItem(int x, int y, ENTITY_TYPE type);
	virtual ~j1LifeItem();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);


public:

	bool ColLeft = false;
	bool ColRight = false;
	bool ColDown = false;
	bool ColUp = false;

	bool fall = false;
	bool dead = false;

private:

	Animation idleAnim;
	Animation destroyAnim;

	bool flip = false;

};

#endif