#ifndef  __j1COIN_H__
#define __j1COIN_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Particle.h"

struct SDL_Texture;
struct Collider;

enum ENTITY_TYPE;

class j1Coin : public j1Entity
{
public:

	j1Coin(int x, int y, ENTITY_TYPE type);
	virtual ~j1Coin();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	void Properties();

public:
	
	bool picked = false;
	int points_given;

private:
	
	Animation idleAnim;
	Animation pickAnim;
};

#endif // ! __j1COIN_H__
