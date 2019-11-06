#ifndef __j1PARTICLE_H__
#define __j1PARTICLE_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"
#include "j1Collisions.h"
#include "p2Log.h"
#include "p2Defs.h"

#define MAX_ACTIVE_PARTICLES 300

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

enum PARTICLE_TYPE {
	BASIC_SHOOT, 
	REMOTE_SHOOT
};

struct Particle
{
	SDL_Texture * tex = nullptr;
	/*int type_particle;*/
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0;
	fPoint position;
	fPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	uint32 state = 0;
	int rotation = 0;
	bool fx_played = false;
	Particle();
	Particle(const Particle& p);
	virtual ~Particle();
	bool Update(float dt);
	iPoint margin;
	PARTICLE_TYPE type;
};

class j1Particle : public j1Module
{
public:
	j1Particle();
	virtual ~j1Particle();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);
	void AddParticle(const Particle& particle, int x, int y, float dt, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0, double rotation = 0, PARTICLE_TYPE ptype = BASIC_SHOOT);
	Particle* active[MAX_ACTIVE_PARTICLES];
private:

	SDL_Texture* part_tex = nullptr;
	SDL_Texture* ice_tex = nullptr;

public:
	uint width, height;
	fPoint particle_speed;

	Particle basicShoot;
	Particle remoteShoot;
};
#endif // __j1PARTICLE_H__ 
