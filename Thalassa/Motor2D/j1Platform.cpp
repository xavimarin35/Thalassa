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

	if (vertical)
		initial_position = (float)y;
	else initial_position = (float)x;

	if (typePlatform == 1)
		idle.LoadAnimations("platformIdle1");
	else if (typePlatform == 2)
		idle.LoadAnimations("platformIdle2");
	else
		idle.LoadAnimations("platformIdle3");
}

j1Platform::~j1Platform() {}

bool j1Platform::Start()
{
	sprites = App->tex->Load("textures/platforms.png");

	animation = &idle;

	speed = { 30.0f, 30.0f };

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 80, 8 }, COLLIDER_WALL, App->entity_manager);

	return true;
}

bool j1Platform::Update(float dt)
{
	BROFILER_CATEGORY("Platform_Update", Profiler::Color::Bisque)


	animation = &idle;
	
	Move(limits, dt);

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

void j1Platform::Move(iPoint limit, float dt)
{
	float limit_left = initial_position - (float)limit.x;
	float limit_right = initial_position + (float)limit.y;

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

void j1Platform::Properties()
{

}