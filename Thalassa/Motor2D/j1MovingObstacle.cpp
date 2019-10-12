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

	Properties();

	topHeight = y - variation;
	lowHeight = y + variation;

	idle.LoadAnimations("movingObstacle");
}

j1MovingObstacle::~j1MovingObstacle() {}

bool j1MovingObstacle::Start() {

	sprites = App->tex->Load("textures/spikes.png");

	speed = { movement.x, movement.y };

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, hitbox, hitbox }, COLLIDER_DEATH, App->entity_manager);

	return true;
}

bool j1MovingObstacle::Update(float dt) {

	if (position.y <= topHeight) 
	{
		movingDown = true;
		movingUp = false;
	}
		
	else if (position.y >= lowHeight)
	{
		movingUp = true;
		movingDown = false;
	}
		

	if (movingDown)
		position.y += speed.y;
	else if (movingUp)
		position.y -= speed.y;


	if (collider != nullptr)
		collider->SetPos(position.x + adjust, position.y + adjust);
	
	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE);

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

void j1MovingObstacle::Properties() 
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeObstacle;
	nodeObstacle = config.child("entities");

	variation = nodeObstacle.child("obstacleVariation").attribute("value").as_int();
	movement = { nodeObstacle.child("obstacleSpeed").attribute("x").as_float(), nodeObstacle.child("obstacleSpeed").attribute("y").as_float() };
	hitbox = nodeObstacle.child("obstacleCollider").attribute("value").as_int();
	adjust = nodeObstacle.child("obstacleAdjust").attribute("value").as_int();
}