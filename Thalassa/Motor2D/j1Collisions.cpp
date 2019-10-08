#include "j1Collisions.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Map.h"

#include "SDL\include\SDL.h"

j1Collisions::j1Collisions() : j1Module()
{

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		colliders[i] = nullptr;
	}
	name.create("collisions");

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_CHEST] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_OPENCHEST] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_OPENDOOR] = true;

	matrix[COLLIDER_DEATH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_WIN] = false;
	matrix[COLLIDER_DEATH][COLLIDER_GOD] = false;

	matrix[COLLIDER_WIN][COLLIDER_WALL] = false;
	matrix[COLLIDER_WIN][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WIN][COLLIDER_WIN] = false;
	matrix[COLLIDER_WIN][COLLIDER_GOD] = true;

	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL][COLLIDER_WIN] = false;
	matrix[COLLIDER_WALL][COLLIDER_GOD] = false;

	matrix[COLLIDER_GOD][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_GOD][COLLIDER_WALL] = false;
	matrix[COLLIDER_GOD][COLLIDER_DEATH] = false;
	matrix[COLLIDER_GOD][COLLIDER_WIN] = true;
	matrix[COLLIDER_GOD][COLLIDER_GOD] = false;

	matrix[COLLIDER_CHEST][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_CHEST][COLLIDER_WALL] = false;
	matrix[COLLIDER_CHEST][COLLIDER_DEATH] = false;
	matrix[COLLIDER_CHEST][COLLIDER_WIN] = false;
	matrix[COLLIDER_CHEST][COLLIDER_GOD] = false;
	matrix[COLLIDER_CHEST][COLLIDER_CHEST] = false;

	matrix[COLLIDER_OPENCHEST][COLLIDER_WALL] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_DEATH] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_WIN] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_GOD] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_CHEST] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_OPENCHEST] = false;

	matrix[COLLIDER_DOOR][COLLIDER_DOOR] = false;
	matrix[COLLIDER_DOOR][COLLIDER_WALL] = false;
	matrix[COLLIDER_DOOR][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DOOR][COLLIDER_WIN] = false;
	matrix[COLLIDER_DOOR][COLLIDER_GOD] = false;
	matrix[COLLIDER_DOOR][COLLIDER_CHEST] = false;

	matrix[COLLIDER_OPENCHEST][COLLIDER_WALL] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_DEATH] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_WIN] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_GOD] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_CHEST] = false;
	matrix[COLLIDER_OPENCHEST][COLLIDER_OPENCHEST] = false;

	matrix[COLLIDER_OPENDOOR][COLLIDER_WALL] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_DEATH] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_WIN] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_GOD] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_CHEST] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_OPENCHEST] = false;
	matrix[COLLIDER_OPENDOOR][COLLIDER_OPENDOOR] = false;
}


j1Collisions::~j1Collisions() {}

bool j1Collisions::PreUpdate()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			if (colliders[i]->to_delete == true)
			{
				delete colliders[i];
				colliders[i] = nullptr;
			}
		}
	}

	Collider* collider1;
	Collider* collider2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr) 
			continue;

		if (colliders[i]->type == COLLIDER_PLAYER)
		{
			collider1 = colliders[i];

			for (uint j = 0; j < MAX_COLLIDERS; ++j)
			{
				if (colliders[j] == nullptr || i == j) continue;

				collider2 = colliders[j];

				if (collider1->CheckCollision(collider2->rect) == true)
				{
					if (matrix[collider1->type][collider2->type] && collider1->callback)
						collider1->callback->OnCollision(collider1, collider2);

					if (matrix[collider2->type][collider1->type] && collider2->callback)
						collider2->callback->OnCollision(collider2, collider1);
				}
			}
		}
	}

	return true;
}

bool j1Collisions::Update(float dt)
{

	DebugColliders();

	return true;
}

bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

void j1Collisions::DebugColliders()
{

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_WALL:	//blue															
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_WIN:	//green															
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_DEATH:	//red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_PLAYER:		//yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_GOD:
			App->render->DrawQuad(colliders[i]->rect, 125, 125, 0, alpha);
			break;
		case COLLIDER_CHEST:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_OPENCHEST:
			App->render->DrawQuad(colliders[i]->rect, 100, 100, 100, alpha);
			break;
		case COLLIDER_DOOR:
			App->render->DrawQuad(colliders[i]->rect, 200, 255, 20, alpha);
			break;
		case COLLIDER_OPENDOOR:
			App->render->DrawQuad(colliders[i]->rect, 0, 200, 200, alpha);
			break;
		}
	}
}


Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{

		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}
	return ret;
}


bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w && rect.x + rect.w > r.x && rect.y < r.y + r.h && rect.h + rect.y > r.y);

}

