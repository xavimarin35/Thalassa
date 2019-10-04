#include "j1App.h"
#include "j1MovingObstacle.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

j1MovingObstacle::j1MovingObstacle(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::OBSTACLE)
{
	animation = NULL;

	idle.loop = false;
	idle.speed = 1.0f;
	idle.PushBack({ 128,50,32,14 });
}

j1MovingObstacle::~j1MovingObstacle() {}

bool j1MovingObstacle::Start() {

	sprites = App->tex->Load("textures/tiles_spike.png");

	position = { 200,130 };
	speed = { 4, 2 };

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 32, 14 }, COLLIDER_DEATH, App->entity_manager);

	return true;
}

bool j1MovingObstacle::Update(float dt) {

	position.y += speed.y;

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);
	
	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE, position.x, position.y);

	return true;
}

bool j1MovingObstacle::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete;
		collider = nullptr;
	}

	return true;
}