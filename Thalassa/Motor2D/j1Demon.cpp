#include "j1App.h"
#include "j1Demon.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

j1Demon::j1Demon(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::DEMON)
{
	demon_position = { (float)x, (float)y };

	animation = NULL;

	idleAnim.LoadAnimations("demonIdle");
	runAnim.LoadAnimations("demonRun");
	attackAnim.LoadAnimations("demonAttack");
	hurtAnim.LoadAnimations("demonHurt");
	deathAnim.LoadAnimations("demonDeath");
}

j1Demon::~j1Demon() {}

bool j1Demon::Start()
{
	sprites = App->tex->Load("textures/Enemies/Demon_Spritesheet.png");

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 15, 13 }, COLLIDER_ENEMY, App->entity_manager);

	return true;
}

bool j1Demon::Update(float dt)
{
	Move();

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

	BlitEntity(animation->GetCurrentFrame(), flip, position.x, position.y);

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

	return true;
}

void j1Demon::Move()
{
	
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