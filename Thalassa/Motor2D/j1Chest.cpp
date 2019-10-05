#include "j1App.h"
#include "j1Chest.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

j1Chest::j1Chest(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::CHEST)
{
	chest_position = { (float)x,(float)y };

	animation = NULL;

	idle.loop = false;
	idle.speed = 1.0f;
	idle.PushBack({ 0,0,15,35 });
	idle.PushBack({ 0,0,15,35 });

	openingAnim.loop = false;
	openingAnim.speed = 0.1f;
	openingAnim.PushBack({ 0,0,15,35 });
	openingAnim.PushBack({ 16,0,15,35 });
	openingAnim.PushBack({ 32,0,15,35 });
	openingAnim.PushBack({ 48,0,15,35 });
	openingAnim.PushBack({ 64,0,15,35 });
	openingAnim.PushBack({ 80,0,15,35 });
	openingAnim.PushBack({ 96,0,15,35 });
	openingAnim.PushBack({ 112,0,15,35 });
	openingAnim.PushBack({ 128,0,15,35 });

	openedAnim.PushBack({ 144,0,15,35 });
}

j1Chest::~j1Chest() {}

bool j1Chest::Start() {

	sprites = App->tex->Load("textures/Chest_Spritesheet.png");

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)chest_position.x, (int)chest_position.y, 15, 35 }, COLLIDER_CHEST, App->entity_manager);

	return true;
}

bool j1Chest::Update(float dt) {

	if (App->entity_manager->player->itemPicked)
		animation = &openedAnim;

	else if (App->entity_manager->player->openingChest) {
		if (!playedFx) {
			App->audio->PlayFx(App->audio->openChestFx);
			playedFx = true;
		}
		animation = &openingAnim;
	}
	else animation = &idle;

	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE, chest_position.x, chest_position.y);

	return true;
}

bool j1Chest::CleanUp() 
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete;
		collider = nullptr;
	}

	return true;
}