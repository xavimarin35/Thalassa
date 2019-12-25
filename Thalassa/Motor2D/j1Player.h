#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "j1Particle.h"
#include "j1Timer.h"
#include "j1Hud.h"

struct Collider;
struct SDL_Texture;
struct Particle;

enum ENTITY_TYPE;
enum PARTICLE_TYPE;

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
	void PlayerMovement(float dt);
	void Jumping();
	void JetPack();
	void Die();
	void LoadInfo();
	void BasicShooting(float x, float y, float dt);
	void RemoteShooting(float x, float y, float dt);
	void ChangeWeapon();
	void GodMode(float dt);

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

	j1Hud* hud = nullptr;


public:
	fPoint position;	
	fPoint speed;

	PARTICLE_TYPE currentType = BASIC_SHOOT;

	float jumpForce;
	float jetForce;
	float godSpeed;
	float speed_y;

	int doubleJump = 2;
	float gravity;
	int lifePoints;
	float jetPackLife;

	std::string current_points;
	uint score;

	bool playerCanMove = false;
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

	bool flip = true;
	iPoint hitbox;

private:
	bool playedFx = false;
	bool playedFx2 = false;

	fPoint margin_particles;
	fPoint speed_particles;

	float jumpForce_xml;
	float jetForce_xml;

	float death_fall;
	int timer_app;

	float jetCharge;
	int jetSpend;
	int jetMax;
};

#endif // __j1PLAYER_H__
