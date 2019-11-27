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

	LoadProperties();

	idle.LoadAnimations("batIdle");
}

j1Bat::~j1Bat() {}

bool j1Bat::Start()
{
	sprites = App->tex->Load("textures/bat.png");

	bat_position = { App->entity_manager->player->position.x - pos_to_player.x, App->entity_manager->player->position.y - pos_to_player.y };

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
		horizontal_pos = x - pos_to_player.x;
		horizontal_limit = { horizontal_pos + limit_pos.x, horizontal_pos - limit_pos.y };
		
		if (bat_position.x < horizontal_pos) ArriveToPlayer(horizontal_pos, true);

		// If the bat is too far away from the character, he returns to his initial position
		if (redirect_horizontal) 
		{
			if (bat_position.x > horizontal_limit.y)
			{
				inertia -= redirectInertia * App->GetDT();
			}
			else redirect_horizontal = false;
		}

		// If the player moves forward, the bat follows him at higher speed than the player
		else if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (bat_position.x < horizontal_limit.x))
			inertia += inertia_speed * App->GetDT();

		else if (bat_position.x >= horizontal_limit.x)
			redirect_horizontal = true;
	}
	
	// We do the same as before but if the player is flipped and looking to the left
	if (!App->entity_manager->player->flip)
	{
		horizontal_pos = x + pos_to_player.x;
		horizontal_limit = { horizontal_pos - limit_pos.x, horizontal_pos + limit_pos.y };

		if (bat_position.x > horizontal_pos) ArriveToPlayer(horizontal_pos, false);

		if (redirect_horizontal)
		{
			if (bat_position.x < horizontal_limit.y)
			{
				inertia += redirectInertia * App->GetDT();
			}
			else redirect_horizontal = false;
		}

		else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && (bat_position.x > horizontal_limit.x))
			inertia -= inertia_speed * App->GetDT();

		else if (bat_position.x <= horizontal_pos)
			redirect_horizontal = true;
	}

	bat_position.x = inertia;

}

void j1Bat::MoveVertical(float y)
{
	BROFILER_CATEGORY("BatMoveVertical", Profiler::Color::AliceBlue)

	float vertical_pos = y - pos_to_player.y;

	animation = &idle;

	if (bat_position.y > vertical_pos + distance.x)
		vertical_speed -= speed1 * App->GetDT();

	else if (bat_position.y > vertical_pos + distance.y)
		vertical_speed -= speed2 * App->GetDT();

	else if(bat_position.y > vertical_pos)
		vertical_speed -= speed3 * App->GetDT();


	if (bat_position.y < vertical_pos - distance.x)
		vertical_speed += speed1 * App->GetDT();

	else if (bat_position.y < vertical_pos - distance.y)
		vertical_speed += speed2 * App->GetDT();

	else if (bat_position.y < vertical_pos - minDistance)
		vertical_speed += speed3 * App->GetDT();

	bat_position.y = vertical_speed;

}

void j1Bat::ArriveToPlayer(float pos_to_be, bool flip)
{
	if (flip)
	{
		if (bat_position.x < pos_to_be - farDistance.x)
			inertia += farSpeed1 * App->GetDT();

		else if (bat_position.x < pos_to_be - farDistance.y)
			inertia += farSpeed2 * App->GetDT();

		else if (bat_position.x < pos_to_be - minDistanceFinal)
			inertia += farSpeed3 * App->GetDT();

		else if (bat_position.x < pos_to_be)
			inertia += closeSpeed * App->GetDT();
	}

	else
	{
		if (bat_position.x > pos_to_be + farDistance.x)
			inertia -= farSpeed1 * App->GetDT();

		else if (bat_position.x > pos_to_be + farDistance.y)
			inertia -= farSpeed2 * App->GetDT();

		else if (bat_position.x > pos_to_be + minDistanceFinal)
			inertia -= farSpeed3 * App->GetDT();

		else if (bat_position.x > pos_to_be)
			inertia -= closeSpeed * App->GetDT();
	}
}

void j1Bat::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeBat;
	nodeBat = config.child("bat");

	pos_to_player = { nodeBat.child("pos_to_player").attribute("x").as_int(),nodeBat.child("pos_to_player").attribute("y").as_int() };
	limit_pos = { nodeBat.child("limit").attribute("x").as_int(),nodeBat.child("limit").attribute("y").as_int() };
	redirectInertia = nodeBat.child("redirectInertia").attribute("value").as_float();
	inertia_speed = nodeBat.child("inertia").attribute("value").as_float();
	distance = { nodeBat.child("distances").attribute("x").as_int(),nodeBat.child("distances").attribute("y").as_int() };
	farDistance = { nodeBat.child("farDistance").attribute("x").as_int(),nodeBat.child("farDistance").attribute("y").as_int() };
	speed1 = nodeBat.child("speed1").attribute("value").as_float();
	speed2 = nodeBat.child("speed2").attribute("value").as_float();
	speed3 = nodeBat.child("speed3").attribute("value").as_float();
	farSpeed1 = nodeBat.child("farSpeed1").attribute("value").as_float();
	farSpeed2 = nodeBat.child("farSpeed2").attribute("value").as_float();
	farSpeed3 = nodeBat.child("farSpeed3").attribute("value").as_float();
	closeSpeed = nodeBat.child("closeSpeed").attribute("value").as_float();
	minDistance = nodeBat.child("minDistance").attribute("value").as_int();
	minDistanceFinal = nodeBat.child("minDistanceFinal").attribute("value").as_int();
}