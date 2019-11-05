#ifndef __j1DEMON_H__
#define __j1DEMON_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Particle.h"

struct SDL_Texture;

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

	void Move();

public:

	fPoint demon_position;
	fPoint speed = {0.0f, 0.7f};

	bool ColLeft = false;
	bool ColRight = false;
	bool ColDown = false;
	bool ColUp = false;

private:

	Animation idleAnim;
	Animation runAnim;
	Animation attackAnim;
	Animation deathAnim;
	Animation hurtAnim;

	bool flip = false;

	float gravity = 0.15f;
};

#endif