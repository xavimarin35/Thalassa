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
	float vertical_pos = y - 30;
	float horitzontal_pos = x - 30;
	
	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (bat_position.x < horitzontal_pos + 60))
	{
		inertia += 0.1f;
	}

	else if (bat_position.x > horitzontal_pos)
		inertia -= 0.2f;

	horitzontal_pos += inertia;

	bat_position = {horitzontal_pos, vertical_pos};

}