#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1BatEnemy.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Map.h"

#include "Brofiler/Brofiler.h"

j1BatEnemy::j1BatEnemy(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::BAT_E)
{
	animation = NULL;

	LoadProperties();

	idleAnim.LoadAnimations("enemyBatIdle");
	pathAnim.LoadAnimations("enemyBatRun");
	dyingAnim.LoadAnimations("enemyBatDie");
	deathAnim.LoadAnimations("enemyBatDead");
}

j1BatEnemy::~j1BatEnemy() {}

bool j1BatEnemy::Start()
{
	sprites = App->tex->Load("textures/Enemies/Enemy_Bat.png");
	
	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, hitbox.x, hitbox.y }, COLLIDER_ENEMY, App->entity_manager);

	return true;
}

bool j1BatEnemy::Update(float dt)
{
	BROFILER_CATEGORY("BatEnemy_Update", Profiler::Color::Beige)

	if (collider != nullptr)
		collider->SetPos(position.x + adjust, position.y);

	if (dead)
	{
		speed.y = 0;
		animation = &deathAnim;
	}

	else if (fall)
	{
		if (!playedFx)
			App->audio->PlayFx(App->audio->batDieFx);
		playedFx = true;

		animation = &dyingAnim;

		if (!ColDown) {
			position.y += speed.y * dt;
			speed.y += gravity * dt;
		}
	}

	else if (!dead && !fall)
	{
		if (App->entity_manager->player->collider != nullptr) {
			if ((App->entity_manager->player->position.x - position.x) <= DETECTION_RANGE
				&& (App->entity_manager->player->position.x - position.x) >= -DETECTION_RANGE
				&& App->entity_manager->player->collider->type == COLLIDER_PLAYER)
			{
				iPoint origin = { App->map->WorldToMap((int)position.x + adjustPath.x, (int)position.y + adjustPath.y) };
				iPoint destination;

				if (position.x < App->entity_manager->player->position.x)
					destination = { App->map->WorldToMap((int)(App->entity_manager->player->position.x + App->entity_manager->player->hitbox.x), (int)(App->entity_manager->player->position.y + App->entity_manager->player->hitbox.y / 2)) };
				else
					destination = { App->map->WorldToMap((int)(App->entity_manager->player->position.x), (int)(App->entity_manager->player->position.y + App->entity_manager->player->hitbox.y)) };

				path = App->pathfinding->CreatePath(origin, destination);
				Move(*path, dt);
				path_created = true;
			}

			else if (path_created)
			{
				path->Clear();
				path_created = false;
				animation = &idleAnim;
			}
		}
	}

	BlitEntity(animation->GetCurrentFrame(dt), flip);

	return true;
}

bool j1BatEnemy::PostUpdate()
{
	ColDown = ColLeft = ColRight = ColUp = false;

	return true;
}

bool j1BatEnemy::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1BatEnemy::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeBat;
	nodeBat = config.child("batEnemy");

	hitbox = { nodeBat.child("hitbox").attribute("x").as_int(), nodeBat.child("hitbox").attribute("y").as_int() };
	speed = { nodeBat.child("speed").attribute("x").as_float(), nodeBat.child("speed").attribute("y").as_float() };
	gravity = nodeBat.child("gravity").attribute("value").as_float();
	adjust = nodeBat.child("adjust").attribute("value").as_int();
	adjustPath = { nodeBat.child("adjustPath").attribute("x").as_int(), nodeBat.child("adjustPath").attribute("y").as_int() };
	adjustCollider = nodeBat.child("adjustCollider").attribute("value").as_int();
}

void j1BatEnemy::Move(p2DynArray<iPoint>& path, float dt)
{
	direction = App->pathfinding->CheckDirection(path);

	switch (direction) 
	{
	case PATH_MOVEMENT::UP_LEFT:
		animation = &pathAnim;
		position.x -= speed.x * dt;
		position.y -= speed.y * dt;
		flip = true;

		break;

	case PATH_MOVEMENT::UP_RIGHT:
		animation = &pathAnim;
		position.x += speed.x * dt;
		position.y -= speed.y * dt;
		flip = false;

		break;

	case PATH_MOVEMENT::DOWN_LEFT:
		animation = &pathAnim;
		position.x -= speed.x * dt;
		position.y += speed.y * dt;
		flip = true;

		break;

	case PATH_MOVEMENT::DOWN_RIGHT:
		animation = &pathAnim;
		position.x += speed.x * dt;
		position.y += speed.y * dt;
		flip = false;

		break;

	case PATH_MOVEMENT::UP:
		animation = &pathAnim;
		position.y -= speed.y * dt;

		break;

	case PATH_MOVEMENT::DOWN:
		animation = &pathAnim;
		position.y += speed.y * dt;

		break;

	case PATH_MOVEMENT::LEFT:
		animation = &pathAnim;
		position.x -= speed.x * dt;
		flip = true;

		break;

	case PATH_MOVEMENT::RIGHT:
		animation = &pathAnim;
		position.x += speed.x * dt;
		flip = false;

		break;
	}
}

void j1BatEnemy::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_ENEMY) {

		if (c2->type == COLLIDER_SHOT) 
		{
			fall = true;
		}

		else if (c2->type == COLLIDER_WALL) 
		{
			if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y < c2->rect.y)
			{
				if (fall) 
				{
					fall = false;
					dead = true;

					if (collider != nullptr)
						collider->to_delete = true;
					collider = nullptr;

					position.y = c2->rect.y + c2->rect.h - adjustCollider;

					ColUp = false;
					ColDown = true;
				}
			}
		}
	}
}