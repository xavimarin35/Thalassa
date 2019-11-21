#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Demon.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Scene1.h"

#include "Brofiler/Brofiler.h"

j1Demon::j1Demon(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::DEMON)
{
	animation = NULL;

	idleAnim.LoadAnimations("demonIdle");
	runAnim.LoadAnimations("demonRun");
	attackAnim.LoadAnimations("demonAttack");
	hurtAnim.LoadAnimations("demonHurt");
	deathAnim.LoadAnimations("demonDie");
}

j1Demon::~j1Demon() {}

bool j1Demon::Start()
{
	sprites = App->tex->Load("textures/Enemies/Demon_Spritesheet.png");

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 10, 13 }, COLLIDER_ENEMY, App->entity_manager);

	timerShot.Start();

	return true;
}

bool j1Demon::Update(float dt)
{
	if (!dead)
	{
		if (App->entity_manager->player->collider != nullptr) {

			if ((App->entity_manager->player->position.x - position.x) <= DETECTION_RANGE
				&& (App->entity_manager->player->position.x - position.x) >= -DETECTION_RANGE
				&& App->entity_manager->player->collider->type == COLLIDER_PLAYER)
			{
				PathFind(dt);

				if (timerShot.Read() >= lastShot + cooldownShot)
				{
					Shoot(dt);
				}
			}

			else if (path_created)
			{
				path->Clear();
				path_created = false;
				animation = &idleAnim;
			}
		}

		if (ColLeft || ColRight)
		{
			move_back = true;

			if (flip) back_pos = position.x + 10;
			else if (!flip) back_pos = position.x - 20;
		}

		if (move_back)
		{
			ColLeft = ColRight = false;
			speed.y = 0;
			MoveBack(dt);
		}

		if (jumping)
		{
			Jump(dt);
		}

		if (ColDown)
			speed.y = 0;

		else if (!ColDown && !jumping && App->scene1->scene_timer.Read() > 1000)
		{
			position.y += speed.y * dt;
			speed.y += gravity * dt;
			animation = &idleAnim;
		}

		if (collider != nullptr)
		{
			if (flip)
				collider->SetPos(position.x, position.y);
			else
				collider->SetPos(position.x + 3, position.y);
		}
	}

	else if (dead)
	{
		animation = &deathAnim;

		if (collider != nullptr)
			collider->to_delete = true;
		collider = nullptr;
	}

	BlitEntity(animation->GetCurrentFrame(dt), flip);

	return true;
}

bool j1Demon::PostUpdate()
{
	ColDown = ColLeft = ColRight = ColUp = false;

	return true;
}

bool j1Demon::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1Demon::Move(p2DynArray<iPoint>& path, float dt)
{
	if (!move_back) 
	{
		direction = App->pathfinding->CheckDirectionGround(path);

		if (direction == PATH_MOVEMENT::LEFT)
		{
			animation = &runAnim;
			position.x -= speed.x * dt;
			flip = true;
		}
		else if (direction == PATH_MOVEMENT::RIGHT)
		{
			animation = &runAnim;
			position.x += speed.x * dt;
			flip = false;
		}
	}
}

void j1Demon::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_ENEMY)
	{
		if (c2->type == COLLIDER_WALL)
		{
			// Right & Left
			if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h + 5 >= c2->rect.y)
			{
				// Right
				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x <= c2->rect.x)
				{
					ColRight = true;
					ColLeft = false;
					move_back = true;
				}
				// Left
				else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w >= c2->rect.x + c2->rect.w)
				{
					ColRight = false;
					ColLeft = true;
					move_back = true;
				}
			}

			// Up & Down
			if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x <= c2->rect.x + c2->rect.w)
			{
				// Down
				if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y < c2->rect.y)
				{
					position.y = c2->rect.y - c1->rect.h + 1;

					speed.y = 0.0f;
					jumping = false;
					ColDown = true;
					ColUp = false;
				}
				// Up
				else if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y > c2->rect.y)
				{
					position.y = c2->rect.y + c2->rect.h;
					
					ColUp = true;
					ColDown = false;
				}
			}
		}

		if (c2->type == COLLIDER_SHOT)
		{
			dead = true;
		}

		if (c2->type == COLLIDER_DEATH)
		{
			dead = true;
		}
	}
}

void j1Demon::PathFind(float dt)
{
	iPoint origin = { App->map->WorldToMap((int)position.x + 7, (int)position.y + 6) };
	iPoint destination;

	if (position.x < App->entity_manager->player->position.x)
		destination = { App->map->WorldToMap((int)(App->entity_manager->player->position.x + App->entity_manager->player->hitbox.x), (int)(App->entity_manager->player->position.y + App->entity_manager->player->hitbox.y / 2)) };
	else
		destination = { App->map->WorldToMap((int)(App->entity_manager->player->position.x), (int)(App->entity_manager->player->position.y + App->entity_manager->player->hitbox.y)) };

	if (App->pathfinding->IsWalkable(destination) && App->pathfinding->IsWalkable(origin))
	{
		path = App->pathfinding->CreatePath(origin, destination);
		Move(*path, dt);
		path_created = true;
	}
}

void j1Demon::MoveBack(float dt)
{
	jump_force = 70.0f;

	if (flip)
	{
		if (position.x < back_pos)
		{
			ColDown = true;
			position.x += 40.0 * dt;
		}
		else if (position.x >= back_pos)
		{
			move_back = false;
			ColDown = false;
			jumping = true;
		}
	}

	if (!flip)
	{
		if (position.x > back_pos)
		{
			ColDown = true;
			position.x -= 40.0 * dt;
		}
		else if (position.x <= back_pos)
		{
			move_back = false;
			ColDown = false;
			jumping = true;
		}
	}
}

void j1Demon::Jump(float dt)
{
	if (!ColDown)
	{
		position.y -= jump_force * dt;
		jump_force -= gravity * dt;
	}

	else
	{
		ColDown = true;
		jumping = false;
	}

}

void j1Demon::Shoot(float dt)
{
	runAnim.Finished();

	fPoint margin;
	margin.x = 8;
	margin.y = 8;

	fPoint edge;
	edge.x = App->entity_manager->player->position.x - position.x;
	edge.y = position.y - App->entity_manager->player->position.y;

	double angle = -(atan2(edge.y, edge.x));

	fPoint speed_particle;
	fPoint p_speed = { 4000, 4000 };

	speed_particle.x = p_speed.x * cos(angle);
	speed_particle.y = p_speed.y * sin(angle);
	App->particles->demonShoot.speed = { speed_particle.x * dt, speed_particle.y * dt };

	double angleInDeg = angle * 180 / PI;

	animation = &attackAnim;
	App->particles->AddParticle(App->particles->demonShoot, position.x + margin.x, position.y + margin.y, 0, COLLIDER_ENEMY_SHOT, dt, angleInDeg, DEMON_SHOOT);

	lastShot = timerShot.Read();
}