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


}

j1Particle::~j1Particle()
{}

// Load assets
bool j1Particle::Start()
{
	LOG("Loading particles");


	return true;
}

bool j1Particle::CleanUp()
{
	LOG("Unloading particles");

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
				//App->render->Blit(part_tex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));

				if (p->fx_played == false)
				{
					p->fx_played = true;
				}
			}
		}

	return true;
}

void j1Particle::AddParticle(const Particle& particle, int x, int y, float dt, COLLIDER_TYPE collider_type, Uint32 delay, int rotation)
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
			p->state = 0;
			p->anim.Reset();
			if (collider_type != COLLIDER_NONE) {
				if (particle.type == BASIC_SHOOT) 
					p->collider = App->collisions->AddCollider({p->anim.GetCurrentFrame().x, p->anim.GetCurrentFrame().y, 22, 20 }, collider_type, this);
				else 
					p->collider = App->collisions->AddCollider(p->anim.GetCurrentFrame(), collider_type, this);
			}

			Collider* test = p->collider;
			active[i] = p;
			break;
		}
	}
}

void j1Particle::OnCollision(Collider* c1, Collider* c2)
{
	bool destroy = false;
	if (c2->type == COLLIDER_PLAYER)
	{
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
	}

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1 && destroy)
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