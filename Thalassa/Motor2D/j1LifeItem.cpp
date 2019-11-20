#include "j1LifeItem.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
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

j1LifeItem::j1LifeItem(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::LIFE_ITEM)
{
	idleAnim.LoadAnimations("bubbleIdle");
	destroyAnim.LoadAnimations("bubbleDestroy");
}

j1LifeItem::~j1LifeItem()
{
}

bool j1LifeItem::Start()
{
	sprites = App->tex->Load("textures/Particles/bubble.png");

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x + 2, (int)position.y + 2, 12, 12 }, COLLIDER_ITEM, App->entity_manager);

	return true;
}

bool j1LifeItem::Update(float dt)
{

	BlitEntity(animation->GetCurrentFrame(dt), false);

	return true;
}

bool j1LifeItem::PostUpdate()
{
	return true;
}

bool j1LifeItem::CleanUp()
{
	return true;
}

void j1LifeItem::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_PLAYER) {
		if (c2->type == COLLIDER_ITEM) 
		{
			animation = &destroyAnim;
			c2->to_delete = true;
		}
	}
}
