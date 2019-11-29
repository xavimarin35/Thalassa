#include "j1App.h"
#include "j1Platform.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

#include "Brofiler/Brofiler.h"

j1Platform::j1Platform(int x, int y, ENTITY_TYPE type, iPoint limit, int typePlatform, bool vertical) : j1Entity(x, y, ENTITY_TYPE::PLATFORM)
{
	animation = NULL;

	isVertical = vertical;
	limits = limit;
	type_platform = typePlatform;

	if (vertical)
		initial_position = (float)y;
	else initial_position = (float)x;

	if (typePlatform == 1)
		idle.LoadAnimations("platformIdle1");
	else if (typePlatform == 2)
		idle.LoadAnimations("platformIdle2");
	else
		idle.LoadAnimations("platformIdle3");

	Properties();
}

j1Platform::~j1Platform() {}

bool j1Platform::Start()
{
	sprites = App->tex->Load("textures/platforms.png");

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, hitbox.x, hitbox.y }, COLLIDER_WALL, App->entity_manager);

	if (!isVertical)
	{
		limit_left = initial_position - limits.x;
		limit_right = initial_position + limits.y;
	}

	else
	{
		limit_up = initial_position - limits.x;
		limit_down = initial_position + limits.y;
	}

	return true;
}

bool j1Platform::Update(float dt)
{
	BROFILER_CATEGORY("Platform_Update", Profiler::Color::Bisque)


	animation = &idle;
	
	if (!isVertical)
		Move(dt);

	else
		MoveVertical(dt);

	if (collider != nullptr)
			collider->SetPos(position.x, position.y);

	BlitEntity(animation->GetCurrentFrame(dt), SDL_FLIP_NONE);

	return true;
}

bool j1Platform::CleanUp()
{
	App->tex->UnLoad(sprites);

	if (collider != nullptr)
	{
		collider->to_delete = true;
		collider = nullptr;
	}
	
	return true;
}

void j1Platform::Move(float dt)
{
	if (!arrived_limitX)
	{
		if (position.x > limit_left)
		{
			position.x -= speed.x * dt;
		}
		else if (position.x <= limit_left)
		{
			arrived_limitX = true;
			arrived_limitY = false;
		}
	}

	else if (!arrived_limitY)
	{
		if (position.x < limit_right)
		{
			position.x += speed.x * dt;
		}
		else if (position.x >= limit_right)
		{
			arrived_limitY = true;
			arrived_limitX = false;
		}
	}
}

void j1Platform::MoveVertical(float dt)
{
	if (!arrived_limitX)
	{
		if (position.y > limit_up)
		{
			position.y -= speed.y * dt;
		}
		else if (position.y <= limit_up)
		{
			arrived_limitX = true;
			arrived_limitY = false;
		}
	}

	else if (!arrived_limitY)
	{
		if (position.y < limit_down)
		{
			position.y += speed.y * dt;
		}
		else if (position.y >= limit_down)
		{
			arrived_limitY = true;
			arrived_limitX = false;
		}
	}
}

void j1Platform::Properties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodePlatform;
	nodePlatform = config.child("platform");
	
	if (type_platform == 1)
		hitbox.x = nodePlatform.child("hitbox1").attribute("value").as_int();

	else if (type_platform == 2)
		hitbox.x = nodePlatform.child("hitbox2").attribute("value").as_int();

	else if (type_platform == 3)
		hitbox.x = nodePlatform.child("hitbox3").attribute("value").as_int();

	hitbox.y = nodePlatform.child("hitbox").attribute("value").as_int();

	speed.x = nodePlatform.child("speed").attribute("x").as_float();
	speed.y = nodePlatform.child("speed").attribute("y").as_float();
}