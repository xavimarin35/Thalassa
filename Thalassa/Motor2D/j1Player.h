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

	void OnCollision(Collider* c1, Collider* c2);
	void Jumping();
	void JetPack();
	void Die();
	void LoadInfo();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	Animation idle;
	Animation run;
	Animation jetpack;
	Animation jump;
	Animation godAnim;
	Animation deathAnim;


public:
	fPoint position;	
	fPoint speed;

	float jumpForce;
	float jetForce;
	float godSpeed;

	int doubleJump = 2;
	int lifes = 3;
	float gravity;

	bool playerCanMove = false;
	bool openingChest = false;
	bool itemPicked = false;
	bool doorOpened = false;
	bool godMode = false;
	bool isDead = false;
	bool playerCreated = false;
	bool onFloor = false;
	bool isJumping = false;
	bool jetpackActive = false;
	bool changedFloor = false;
	bool touchingWin = false;

	bool ColRight = false;
	bool ColLeft = false;
	bool ColUp = false;
	bool ColDown = false;

private:
	bool flip = true;
	bool playedFx = false;
	bool playedFx2 = false;

	float jumpForce_xml;
	float jetForce_xml;

	iPoint hitbox;
};

#endif // __j1PLAYER_H__
