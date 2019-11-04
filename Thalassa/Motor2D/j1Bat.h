#ifndef __j1BAT_H__
#define __j1BAT_H__

#include "j1Entity.h"
#include "p2Point.h"

struct SDL_Texture;

enum ENTITY_TYPE;

class j1Bat : public j1Entity {

public:

	j1Bat(int x, int y, ENTITY_TYPE type);
	virtual ~j1Bat();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void Move(float x, float y);

public:

	fPoint bat_position;
	fPoint speed = { 0,0 };

	float inertia = 0;

	Animation idle;
};

#endif