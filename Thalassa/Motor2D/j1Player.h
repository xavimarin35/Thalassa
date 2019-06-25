#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Entity.h"

struct Collider;
struct SDL_Texture;

enum ENTITY_TYPE;

class j1Player : public j1Entity
{
public:

	j1Player(int x, int y, ENTITY_TYPE type);
	virtual ~j1Player();

	bool Start();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	Animation idle;
	Animation run;
	Animation jetpack;
	Animation jump;


public:
	iPoint position;	
	bool dead = false;
	bool playerCreated = false;
	float speed;
};

#endif // __j1PLAYER_H__
