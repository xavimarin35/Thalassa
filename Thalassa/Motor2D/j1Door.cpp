#include "j1App.h"
#include "j1Door.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

j1Door::j1Door(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::DOOR) 
{
	door_position = { (float)x,(float)y };

	animation = NULL;

	idle.loop = false;
	idle.PushBack({ 40,0,8,15 });

	openingAnim.loop = false;
	openingAnim.speed = 1.0f;
	openingAnim.PushBack({ 32,0,8,15 });
	openingAnim.PushBack({ 24,0,8,15 });
	openingAnim.PushBack({ 16,0,8,15 });
	openingAnim.PushBack({ 8,0,8,15 });
	openingAnim.PushBack({ 0,0,8,15 });
}

j1Door::~j1Door() {}

bool j1Door::Start() 
{
	sprites = App->tex->Load("textures/Door_Spritesheet.png");

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)door_position.x, (int)door_position.y, 8,15 }, COLLIDER_DOOR, App->entity_manager);

	return true;
}

bool j1Door::Update(float dt) {

	if (App->entity_manager->player->doorOpened) {
		animation = &openingAnim;
		if (!playedFx) {
			//audio
			playedFx = true;
		}
	}

	else animation = &idle;

	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE, door_position.x, door_position.y);

	return true;
}

bool j1Door::CleanUp() {

	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete;
		collider = nullptr;
	}

	return true;
}