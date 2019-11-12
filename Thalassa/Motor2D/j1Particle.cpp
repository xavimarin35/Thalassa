#include "j1App.h"
#include "j1Particle.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Player.h"
#include "p2Animation.h"
#include "j1Scene1.h"
#include "j1Entity.h"
#include "j1Audio.h"


j1Particle::j1Particle()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;

	//basic
	basicShoot.anim.PushBack({ 65,75,8,6 });
	basicShoot.anim.PushBack({ 77,74,8,7 });
	basicShoot.anim.speed = 0.1f;
	basicShoot.life = 3000;
	basicShoot.type = BASIC_SHOOT;

	//explosion
	ShootDestroyed.anim.PushBack({ 88,73,9,9 });
	ShootDestroyed.anim.PushBack({ 100,73,12,9 });
	ShootDestroyed.anim.PushBack({ 113,73,12,11 });
	ShootDestroyed.anim.PushBack({ 129,71,14,12 });
	ShootDestroyed.anim.speed = 0.15f;
	ShootDestroyed.life = 200;

	//remote
	remoteShoot.anim.PushBack({ 52,25,11,6 });
	remoteShoot.anim.PushBack({ 70,24,9,7 });
	remoteShoot.anim.speed = 0.1f;
	remoteShoot.life = 3000;
	remoteShoot.type = REMOTE_SHOOT;

	//demon
	demonShoot.anim.PushBack({ 117,184,9,3 });
	demonShoot.anim.PushBack({ 153,184,5,3 });
	demonShoot.anim.speed = 0.2f;
	demonShoot.life = 4000;
	demonShoot.type = DEMON_SHOOT;

	// demon explosion
	demonShootDestroyed.anim.PushBack({ 87,62,8,7 });
	demonShootDestroyed.anim.PushBack({ 96,61,9,9 });
	demonShootDestroyed.anim.PushBack({ 105,61,10,9 });
	demonShootDestroyed.anim.PushBack({ 117,61,10,10 });
	demonShootDestroyed.anim.speed = 0.15f;
	demonShootDestroyed.life = 200;


}

j1Particle::~j1Particle()
{}

// Load assets
bool j1Particle::Start()
{
	LOG("Loading particles");
	shots_tex = App->tex->Load("textures/Particles/shots.png");
	demonShot_tex = App->tex->Load("textures/Enemies/Demon_Spritesheet.png");

	basicShoot.tex = shots_tex;
	ShootDestroyed.tex = shots_tex;
	remoteShoot.tex = shots_tex;
	remoteShootDestroyed.tex = shots_tex;
	demonShoot.tex = demonShot_tex;
	demonShootDestroyed.tex = shots_tex;


	return true;
}

bool j1Particle::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(shots_tex);
	App->tex->UnLoad(demonShot_tex);

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			if (active[i]->type == BASIC_SHOOT || active[i]->type == REMOTE_SHOOT)
			{
				App->tex->UnLoad(shots_tex);
				App->tex->UnLoad(demonShot_tex);
				delete active[i];
				active[i] = nullptr;
			}
		}
	}
	return true;
}

bool j1Particle::Update(float dt)
{
		for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		{
			Particle* p = active[i];

			if (p == nullptr)
				continue;

			if (p->Update(dt) == false)
			{
				delete p;
				active[i] = nullptr;
			}
			else if (SDL_GetTicks() >= p->born)
			{
				App->render->Blit(p->tex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame(dt)), SDL_FLIP_NONE, 1.0F, p->rotation);

				if (p->fx_played == false)
				{
					p->fx_played = true;
				}
			}
		}

	return true;
}

void j1Particle::AddParticle(const Particle& particle, int x, int y, float dt, COLLIDER_TYPE collider_type, Uint32 delay, double rotation, PARTICLE_TYPE ptype)
{

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			p->rotation = rotation;
			p->type = ptype;
			p->anim.Reset();
			if (collider_type == COLLIDER_SHOT) {

				switch (particle.type)
				{
				case PARTICLE_TYPE::BASIC_SHOOT:
					p->collider = App->collisions->AddCollider({ p->anim.GetCurrentFrame(dt).x, p->anim.GetCurrentFrame(dt).y, 5, 5 }, collider_type, this);
					break;
				case PARTICLE_TYPE::REMOTE_SHOOT:
					p->collider = App->collisions->AddCollider({ p->anim.GetCurrentFrame(dt).x, p->anim.GetCurrentFrame(dt).y, 5, 5 }, collider_type, this);
					break;
				}
				
			}

			if (collider_type == COLLIDER_ENEMY_SHOT) {
				
				switch (particle.type)
				{
				case PARTICLE_TYPE::DEMON_SHOOT:
					p->collider = App->collisions->AddCollider({ p->anim.GetCurrentFrame(dt).x, p->anim.GetCurrentFrame(dt).y, 5, 5 }, collider_type, this);
				}
			}

			Collider* test = p->collider;
			active[i] = p;
			break;
		}
	}
}

void j1Particle::OnCollision(Collider* c1, Collider* c2)
{
	bool particleDestroyed = false;
		if (c1->type == COLLIDER_SHOT && (c2->type == COLLIDER_ENEMY || c2->type == COLLIDER_WALL)) 
		{
			int ret = true;
			for (uint i = 0; i < MAX_ACTIVE_PARTICLES && ret; ++i)
			{
				if (active[i] != nullptr) {
					if (active[i]->collider == c1) {
						switch (active[i]->type) {
						case BASIC_SHOOT:
							AddParticle(ShootDestroyed, active[i]->position.x, active[i]->position.y, 0);
							particleDestroyed = true;
							break;
						case REMOTE_SHOOT:
							AddParticle(ShootDestroyed, active[i]->position.x, active[i]->position.y, 0);
							particleDestroyed = true;
							break;
						}
						ret = false;
					}
				}
			}
		}

		else if (c1->type == COLLIDER_ENEMY_SHOT && (c2->type == COLLIDER_PLAYER || c2->type == COLLIDER_WALL))
		{
			int ret = true;
			for (uint i = 0; i < MAX_ACTIVE_PARTICLES && ret; ++i)
			{
				if (active[i] != nullptr) {
					if (active[i]->collider == c1) {
						switch (active[i]->type) {
						case DEMON_SHOOT:
							AddParticle(demonShootDestroyed, active[i]->position.x, active[i]->position.y, 0);
							particleDestroyed = true;
						}
						ret = false;
					}
				}
			}
		}
		
// destroy the particle collided
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr && active[i]->collider == c1 && particleDestroyed)
		{
			delete active[i];
			active[i] = nullptr;
			break;
		}

	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------
Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), type(p.type), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life), tex(p.tex)
{}

Particle::~Particle()
{
	if (collider != nullptr)
		collider->to_delete = true;
}

bool Particle::Update(float dt)
{
	bool ret = true;
	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life) {
			ret = false;	
		}
	}

	position.x += speed.x * dt;
	position.y += speed.y * dt;
	
	if (collider != nullptr) 
	{
		collider->SetPos(position.x, position.y);
	}

	return ret;
}