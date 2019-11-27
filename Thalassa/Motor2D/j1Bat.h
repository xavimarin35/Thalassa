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
	
	void LoadProperties();

	void ArriveToPlayer(float pos_to_be, bool flip);

	void MoveHorizontal(float x);
	void MoveVertical(float y);

public:

	fPoint bat_position;

private:

	float inertia = 0;
	float vertical_speed = 0;

	bool redirect_horizontal = false;

	bool moving_vertical = false;
	bool movement = false;

	Animation idle;

	//XML Variables
	iPoint pos_to_player;
	iPoint limit_pos;

	float redirectInertia;
	float inertia_speed;

	iPoint distance;
	int minDistance, minDistanceFinal;
	iPoint farDistance;

	float farSpeed1, farSpeed2, farSpeed3;
	float speed1, speed2, speed3, closeSpeed;

};

#endif