#include "j1App.h"
#include "j1Bat.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"

#include "Brofiler/Brofiler.h"

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

	bat_position = { App->entity_manager->player->position.x - 30, App->entity_manager->player->position.y - 30 };

	animation = &idle;

	return true;
}

bool j1Bat::Update(float dt)
{
	BROFILER_CATEGORY("Bat_Update", Profiler::Color::YellowGreen)

	animation = &idle;

	MoveHorizontal(App->entity_manager->player->position.x);
	MoveVertical(App->entity_manager->player->position.y);

	BlitEntity(animation->GetCurrentFrame(dt), SDL_FLIP_NONE, bat_position.x, bat_position.y);

	return true;
}

bool j1Bat::CleanUp()
{
	App->tex->UnLoad(sprites);

	return true;
}

void j1Bat::MoveHorizontal(float x)
{
	BROFILER_CATEGORY("BatMoveHorizontal", Profiler::Color::LightPink)

	// Bat position, behind the player
	float horizontal_pos;
	fPoint horizontal_limit;

	animation = &idle;

	if (App->entity_manager->player->flip)
	{
		// Bat at the left of the character
		horizontal_pos = x - 30;
		horizontal_limit = { horizontal_pos + 60, horizontal_pos - 10 };
		
		if (bat_position.x < horizontal_pos) ArriveToPlayer(horizontal_pos, true);

		// If the bat is too far away from the character, he returns to his initial position
		if (redirect_horizontal) 
		{
			if (bat_position.x > horizontal_limit.y)
			{
				inertia -= 25.0f * App->GetDT();
			}
			else redirect_horizontal = false;
		}

		// If the player moves forward, the bat follows him at higher speed than the player
		else if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (bat_position.x < horizontal_limit.x))
			inertia += 100.0f * App->GetDT();

		else if (bat_position.x >= horizontal_limit.x)
			redirect_horizontal = true;
	}
	
	// We do the same as before but if the player is flipped and looking to the left
	if (!App->entity_manager->player->flip)
	{
		horizontal_pos = x + 30;
		horizontal_limit = { horizontal_pos - 60, horizontal_pos + 10 };

		if (bat_position.x > horizontal_pos) ArriveToPlayer(horizontal_pos, false);

		if (redirect_horizontal)
		{
			if (bat_position.x < horizontal_limit.y)
			{
				inertia += 25.0f * App->GetDT();
			}
			else redirect_horizontal = false;
		}

		else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && (bat_position.x > horizontal_limit.x))
			inertia -= 100.0f * App->GetDT();

		else if (bat_position.x <= horizontal_pos)
			redirect_horizontal = true;
	}

	bat_position.x = inertia;

}

void j1Bat::MoveVertical(float y)
{
	BROFILER_CATEGORY("BatMoveVertical", Profiler::Color::AliceBlue)

	float vertical_pos = y - 30;

	animation = &idle;

	if (bat_position.y > vertical_pos + 200)
		vertical_speed -= 100.0f * App->GetDT();

	else if (bat_position.y > vertical_pos + 100)
		vertical_speed -= 50.0f * App->GetDT();

	else if(bat_position.y > vertical_pos)
		vertical_speed -= 30.0f * App->GetDT();


	if (bat_position.y < vertical_pos - 200)
		vertical_speed += 100.0f * App->GetDT();

	else if (bat_position.y < vertical_pos - 100)
		vertical_speed += 50.0f * App->GetDT();

	else if (bat_position.y < vertical_pos - 5)
		vertical_speed += 30.0f * App->GetDT();

	bat_position.y = vertical_speed;

}

void j1Bat::ArriveToPlayer(float pos_to_be, bool flip)
{
	if (flip)
	{
		if (bat_position.x < pos_to_be - 150)
			inertia += 300.0f * App->GetDT();

		else if (bat_position.x < pos_to_be - 100)
			inertia += 160.0f * App->GetDT();

		else if (bat_position.x < pos_to_be - 50)
			inertia += 75.0f * App->GetDT();

		else if (bat_position.x < pos_to_be)
			inertia += 25.0f * App->GetDT();
	}

	else
	{
		if (bat_position.x > pos_to_be + 150)
			inertia -= 300.0f * App->GetDT();

		else if (bat_position.x > pos_to_be + 100)
			inertia -= 160.0f * App->GetDT();

		else if (bat_position.x > pos_to_be + 50)
			inertia -= 75.0f * App->GetDT();

		else if (bat_position.x > pos_to_be)
			inertia -= 25.0f * App->GetDT();
	}
}