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

	basicShoot.anim.PushBack({ 65,75,8,6 });
	basicShoot.anim.PushBack({ 77,74,8,7 });
	basicShoot.anim.speed = 0.1f;
	basicShoot.life = 2000;
	basicShoot.type = BASIC_SHOOT;

	/*remoteShoot.anim.PushBack({ 119,76,11,5 });
	remoteShoot.anim.PushBack({ 396,8,14,12 });
	remoteShoot.anim.PushBack({ 413,8,14,12 });
	remoteShoot.anim.PushBack({ 396,8,14,12 });
	remoteShoot.anim.PushBack({ 379,8,14,12 });
	remoteShoot.life = 500;
	remoteShoot.type = REMOTE_SHOOT;*/

	remoteShoot.anim.PushBack({ 52,25,11,6 });
	remoteShoot.anim.PushBack({ 70,24,9,7 });
	//remoteShoot.anim.PushBack({ 0,20,52,12 });
	//remoteShoot.anim.PushBack({ 0,30,58,12 });
	//remoteShoot.anim.PushBack({ 0,40,55,12 });
	remoteShoot.anim.speed = 0.1f;
	remoteShoot.life = 2000;
	remoteShoot.type = REMOTE_SHOOT;

}

j1Particle::~j1Particle()
{}

// Load assets
bool j1Particle::Start()
{
	LOG("Loading particles");
	/*part_tex = App->tex->Load("textures/Particles/particles.png");*/
	part2_tex = App->tex->Load("textures/Particles/shots.png");

	basicShoot.tex = part2_tex;
	remoteShoot.tex = part2_tex;

	return true;
}

bool j1Particle::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(part_tex);

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
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
				App->render->Blit(p->tex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()), SDL_FLIP_NONE, 1.0F, p->rotation);

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
			p->state = 0;
			p->anim.Reset();
			if (collider_type == COLLIDER_SHOOT) {

				switch (particle.type)
				{
				case PARTICLE_TYPE::BASIC_SHOOT:
					p->collider = App->collisions->AddCollider({ p->anim.GetCurrentFrame().x, p->anim.GetCurrentFrame().y, 10, 10 }, collider_type, this);
					break;
				case PARTICLE_TYPE::REMOTE_SHOOT:
					p->collider = App->collisions->AddCollider({ p->anim.GetCurrentFrame().x, p->anim.GetCurrentFrame().y, 10, 10 }, collider_type, this);
					break;
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
	
		int ret = true;
		for (uint i = 0; i < MAX_ACTIVE_PARTICLES && ret; ++i)
		{
			if (active[i] != nullptr) {
				if (active[i]->collider == c1) {
					switch (active[i]->type) {
					case BASIC_SHOOT:
						break;
					case REMOTE_SHOOT:
						break;
					}
					ret = false;
				}
			}
		}
	

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1 && particleDestroyed)
		{
			//AddParticle(...) ---> If we want to print an explosion when hitting an enemy
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

	position.x += speed.x/* * dt*/;
	position.y += speed.y/* * dt*/;
	
	if (collider != nullptr) 
	{
		collider->SetPos(position.x, position.y);
	}

	return ret;
}