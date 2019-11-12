#ifndef __j1BATENEMY_H__
#define __j1BATENEMY_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Particle.h"
#include "j1Pathfinding.h"

#define DETECTION_RANGE 150

struct SDL_Texture;
struct Collider;

enum ENTITY_TYPE;

class j1BatEnemy : public j1Entity {

public:

	j1BatEnemy(int x, int y, ENTITY_TYPE type);
	virtual ~j1BatEnemy();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	void Move(p2DynArray<iPoint>& path, float dt);

public:

	fPoint speed = { 35.0f,35.0f };

	bool ColLeft = false;
	bool ColRight = false;
	bool ColDown = false;
	bool ColUp = false;

	bool fall = false;
	bool dead = false;

private:

	PATH_MOVEMENT direction;

	Animation idleAnim;
	Animation pathAnim;
	Animation dyingAnim;
	Animation deathAnim;

	bool flip = false;

	bool path_created = false;

	p2DynArray<iPoint>* path;

	float gravity = 150.0f;
};

#endif