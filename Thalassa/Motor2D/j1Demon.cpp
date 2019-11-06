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

	position = { 200, 150 };

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 15, 13 }, COLLIDER_ENEMY, App->entity_manager);

	return true;
}

bool j1Demon::Update(float dt)
{
	if (!dead)
	{

		if (ColDown)
			speed.y = 0;

		else if (!ColDown)
		{
			position.y += speed.y;
			speed.y += gravity;
			animation = &idleAnim;
		}

		if (collider != nullptr)
			collider->SetPos(position.x, position.y);


		if ((App->entity_manager->player->position.x - position.x) <= DETECTION_RANGE
			&& (App->entity_manager->player->position.x - position.x) >= -DETECTION_RANGE
			&& App->entity_manager->player->collider->type == COLLIDER_PLAYER)
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
		else if (path_created)
		{
			path->Clear();
			path_created = false;
		}

		BlitEntity(animation->GetCurrentFrame(), flip);
	}

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
	direction = App->pathfinding->CheckDirectionGround(path);

	if (direction == PATH_MOVEMENT::LEFT)
	{
		animation = &runAnim;
		position.x -= speed.x;
		flip = true;
	}
	else if (direction == PATH_MOVEMENT::RIGHT)
	{
		animation = &runAnim;
		position.x += speed.x;
		flip = false;
	}

	if (direction == PATH_MOVEMENT::DOWN)
	{
		animation = &runAnim;
		position.y += speed.y;
	}
	else if (direction == PATH_MOVEMENT::UP)
	{
		animation = &runAnim;
		position.y -= speed.y;
	}
}

void j1Demon::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_ENEMY)
	{
		if (c2->type == COLLIDER_WALL)
		{
			// Right & Left
			if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h >= c2->rect.y)
			{
				// Right
				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x <= c2->rect.x)
				{
					ColRight = true;
					ColLeft = false;
				}
				// Left
				else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w >= c2->rect.x + c2->rect.w)
				{
					ColRight = false;
					ColLeft = true;
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
	}
}