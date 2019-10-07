#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1TransitionsManager.h"
#include "j1Collisions.h"

j1Scene1::j1Scene1() : j1Module()
{
	name.create("scene1");
}

// Destructor
j1Scene1::~j1Scene1()
{}

// Called before render is available
bool j1Scene1::Awake()
{
	LOG("Loading Scene1");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene1::Start()
{
	// TUTORIAL
	if (tutorial_active) {
		App->map->Load("Map1_Tutorial.tmx");
		cameraLimitX = -6290;
		cameraLimitY = -360;

		App->entity_manager->CreateEntity(PLAYER);
	}

	// LEVEL 1
	else if (level1_active) {
		App->map->Load("Map2_Level1.tmx");

		if (!midlevel_completed) {
			cameraLimitX = -5135;
			cameraLimitY = -790;
		}
		else {
			cameraLimitX = -10400;
			cameraLimitY = -790;
		}

		App->entity_manager->CreateEntity(PLAYER);
	}

	// MID-LEVEL
	else if (midlevel_active) {
		App->map->Load("Map3_MidLevel.tmx");

		cameraLimitX = -2200;
		cameraLimitY = -400;
		App->entity_manager->CreateEntity(PLAYER);
	}


	/*App->entity_manager->CreateEntity(OBSTACLE);
	App->entity_manager->CreateEntity(CHEST, 15, 100);
	App->entity_manager->CreateEntity(DOOR, 50, 100);*/

//	App->audio->PlayMusic("audio/music/loading.ogg");
		
	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene1::Update(float dt)
{

	//Save & Load
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->SaveGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame("save_game.xml");
	}

	
	if (death) {
		App->transitions->FadingToColor(Black, 0.5f);
	}

	// If player arrives to the end of a level
	if (App->entity_manager->player->touchingWin) {

		// Wins from tutorial & goes to level 1
		if (tutorial_active) {
			midlevel_completed = false;

			tutorial_active = false;
			level1_active = true;
			midlevel_active = false;

			LoadNewLevel();
		}
		
		else if (level1_active) {
			// Crosses the door & goes to mid level
			if (!midlevel_completed) {
				
				tutorial_active = false;
				level1_active = false;
				midlevel_active = true;
				
				LoadNewLevel();
			}

			// Wins the game
			else {
				tutorial_active = true;
				level1_active = false;
				midlevel_active = false;

				LoadNewLevel();
			}
		}

		// Wins from midlevel & returns to level1
		else if (midlevel_active) {
			midlevel_completed = true;

			tutorial_active = false;
			level1_active = true;
			midlevel_active = false;

			LoadNewLevel();
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	// Loads tutorial
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		tutorial_active = true;
		level1_active = false;
		midlevel_active = false;
		LoadNewLevel();
	}

	// Loads level 1
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		midlevel_completed = false;

		tutorial_active = false;
		level1_active = true;
		midlevel_active = false;
		LoadNewLevel();
	}

	// Loads mid-level
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		tutorial_active = false;
		level1_active = false;
		midlevel_active = true;
		LoadNewLevel();
	}


	// CAMERA FOLLOWING PLAYER
	if (App->entity_manager->player != nullptr) 
	{
		if (lateralMove) {
			if (App->render->camera.x >= -App->entity_manager->player->position.x * App->win->GetScale() + App->win->width / 2) {
				App->render->camera.x -= 3.5f;
				App->entity_manager->player->playerCanMove = false;
			}
			else {
				lateralMove = false;
				App->entity_manager->player->playerCanMove = true;
			}
		}
		else
			App->render->camera.x = -App->entity_manager->player->position.x * App->win->GetScale() + App->win->width / 2;
		
		// LIMITING X CAMERA
		if (- App->render->camera.x < 0) 
			App->render->camera.x = 0;
		else if (App->render->camera.x < cameraLimitX) 
			App->render->camera.x = cameraLimitX;
		
		if (cameraMoving)
		{
			if (App->render->camera.y >= -App->entity_manager->player->position.y * App->win->GetScale() + App->win->height / 2) {
				App->render->camera.y -= 2.0f;
				App->entity_manager->player->playerCanMove = false;
			}
			else {
				cameraMoving = false;
				App->entity_manager->player->playerCanMove = true;
			}
		}
		else
			App->render->camera.y = -App->entity_manager->player->position.y * App->win->GetScale() + App->win->height / 2;
		
		// LIMITING Y CAMERA
		if (App->render->camera.y > 0)
			App->render->camera.y = 0;
		else if (App->render->camera.y < cameraLimitY)
			App->render->camera.y = cameraLimitY;
	}

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->MapToWorld(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle("Thalassa");
	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene1");
	App->map->CleanUp();
	App->collisions->CleanUp();
	if (App->entity_manager->player != nullptr)
		App->entity_manager->player->CleanUp();
	App->entity_manager->CleanUp();

	return true;
}

void j1Scene1::LoadNewLevel() {

	CleanUp();

	Start();
	App->collisions->Start();
	App->entity_manager->Start();
	if (App->entity_manager->player != nullptr)
		App->entity_manager->player->Start();


	if (level1_active && midlevel_completed) {
		lateralMove = true;
		App->render->camera.x = -5400;
	}
	else {
		cameraMoving = true;
		App->render->camera.y = 0;
	}
}