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

	Properties();

	idle.LoadAnimations("chestIdle");
	openingAnim.LoadAnimations("chestOpening");
	openedAnim.LoadAnimations("chestOpened");
}

j1Chest::~j1Chest() {}

bool j1Chest::Start() {

	sprites = App->tex->Load("textures/Chest_Spritesheet.png");

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)chest_position.x, (int)chest_position.y, hitbox.x, hitbox.y }, COLLIDER_CHEST, App->entity_manager);

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
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1Chest::Properties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeChest;
	nodeChest = config.child("entities");

	hitbox = { nodeChest.child("chestCollider").attribute("x").as_int(), nodeChest.child("chestCollider").attribute("y").as_int() };
}