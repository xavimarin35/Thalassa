#include "j1Coin.h"
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

j1Coin::j1Coin(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::COIN)
{
	idleAnim.LoadAnimations("coinIdle");
	pickAnim.LoadAnimations("coinPick");

	Properties();
}

j1Coin::~j1Coin() {}

bool j1Coin::Start()
{
	sprites = App->tex->Load("textures/Particles/COIN.png");

	picked = false;

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y + 1, 11,14 }, COLLIDER_ITEM, App->entity_manager);

	return true;
}

bool j1Coin::Update(float dt)
{
	if (!picked)
		animation = &idleAnim;

	else animation = &pickAnim;

	BlitEntity(animation->GetCurrentFrame(dt));

	return true;
}

bool j1Coin::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr)
	{
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1Coin::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_ITEM)
	{
		App->audio->PlayFx(App->audio->coinPick);
		picked = true;
		c2->to_delete = true;
		animation = &pickAnim;
		App->entity_manager->player->score += points_given;
	}
}

void j1Coin::Properties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeCoin;
	nodeCoin = config.child("player");

	points_given = nodeCoin.child("coin_points").attribute("value").as_int();
}