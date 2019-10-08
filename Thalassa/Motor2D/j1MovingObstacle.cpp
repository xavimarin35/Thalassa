#include "j1App.h"
#include "j1MovingObstacle.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1EntityManager.h"

j1MovingObstacle::j1MovingObstacle(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::OBSTACLE)
{
	animation = NULL;

	idle.loop = false;
	idle.speed = 1.0f;
	idle.PushBack({ 140,133,36,36 });
}

j1MovingObstacle::~j1MovingObstacle() {}

bool j1MovingObstacle::Start() {

	sprites = App->tex->Load("textures/spikes.png");

	position = { 200,130 };
	speed = { 1, 2.5f };

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 30, 30 }, COLLIDER_DEATH, App->entity_manager);

	return true;
}

bool j1MovingObstacle::Update(float dt) {

	if (position.y <= 130) 
	{
		movingDown = true;
		movingUp = false;
	}
		
	else if (position.y >= 200)
	{
		movingUp = true;
		movingDown = false;
	}
		

	if (movingDown)
		position.y += speed.y;
	else if (movingUp)
		position.y -= speed.y;


	if (collider != nullptr)
		collider->SetPos(position.x, position.y);
	
	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE, position.x, position.y);

	return true;
}

bool j1MovingObstacle::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}