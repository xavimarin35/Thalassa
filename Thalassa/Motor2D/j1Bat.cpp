#include "j1App.h"
#include "j1Bat.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

j1Bat::j1Bat(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::BAT)
{
	bat_position = { (float)x, (float)y };

	animation = NULL;

	idle.LoadAnimations("batIdle");
}

j1Bat::~j1Bat() {}

bool j1Bat::Start()
{
	sprites = App->tex->Load("textures/bat.png");

	animation = &idle;

	return true;
}

bool j1Bat::Update(float dt)
{
	Move(App->entity_manager->player->position.x, App->entity_manager->player->position.y);

	BlitEntity(animation->GetCurrentFrame(), SDL_FLIP_NONE, bat_position.x, bat_position.y);

	return true;
}

bool j1Bat::CleanUp()
{
	App->tex->UnLoad(sprites);

	return true;
}

void j1Bat::Move(float x, float y)
{
	float vertical_pos = y - 20;
	float horitzontal_pos;


	if (App->entity_manager->player->flip)
		horitzontal_pos = x - 30;
	else horitzontal_pos = x + 30;

	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (bat_position.x < horitzontal_pos + 60))
	{
		inertia += 1.5f;
	}

	else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && (bat_position.x < horitzontal_pos - 60))
	{
		inertia -= 1.5f;
	}

	else if (bat_position.x > horitzontal_pos)
		inertia -= 1.5f;

	else if (bat_position.x < horitzontal_pos)
	{
		inertia += 1.5f;
	}

	if (App->entity_manager->player->isJumping || !App->entity_manager->player->onFloor) 
	{
		if (vertical_pos < bat_position.y)
			vertical_speed -= 0.5f;

		if (vertical_pos > bat_position.y)
			vertical_speed += 0.5f;
	}

	else if (bat_position.y < vertical_pos - 5)
	{
		vertical_speed += 1.0f;
	}

	else if (bat_position.y > vertical_pos + 5)
	{
		vertical_speed -= 1.0f;
	}

	horitzontal_pos += inertia;
	vertical_pos += vertical_speed;

	bat_position = {inertia, vertical_speed};

}