#ifndef __j1DEMON_H__
#define __j1DEMON_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Particle.h"
#include "j1Pathfinding.h"
#include "j1Timer.h"

#define DETECTION_RANGE 200

struct SDL_Texture;
struct Collider;

enum ENTITY_TYPE;

class j1Demon : public j1Entity {

public:

	j1Demon(int x, int y, ENTITY_TYPE type);
	virtual ~j1Demon();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	void Move(p2DynArray<iPoint>& path, float dt);
	
	void PathFind(float dt);
	void Shoot(float dt);
	void MoveBack(float dt);
	void Jump(float dt);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	fPoint demon_position;
	fPoint speed = {30.0f, 30.0f};

	bool ColLeft = false;
	bool ColRight = false;
	bool ColDown = false;
	bool ColUp = false;

	bool dead = false;
	bool jumping = false;
	bool move_back = false;

	int attackDamage = 100;

private:

	j1Timer timerShot;
	uint lastShot = 0;
	uint cooldownShot = 3000;

	float back_pos;

	PATH_MOVEMENT direction;

	float jump_force;

	Animation idleAnim;
	Animation runAnim;
	Animation attackAnim;
	Animation deathAnim;
	Animation hurtAnim;

	bool flip = false;
	bool path_created = false;

	p2DynArray<iPoint>* path;

	float gravity = 150.0f;
};

#endif