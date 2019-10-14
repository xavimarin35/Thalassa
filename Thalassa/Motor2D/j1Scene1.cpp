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
#include "p2Animation.h"
#include "SDL_mixer/include/SDL_mixer.h"

#define SDL_TICKS_PASSED(A, B)  ((Sint32)((B) - (A)) <= 0)

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
	LoadSceneInfo();

	

	// TUTORIAL
	if (tutorial_active) {
		App->map->Load("Map1_Tutorial.tmx");

		keys = App->tex->Load("textures/keys.png");
		keyA.loop = false;
		keyA.speed = 0.0F;
		keyA.PushBack({ 48, 49, 16, 15 });
		keyA.PushBack({ 48, 147, 16, 13 });

		keyD.loop = false;
		keyD.speed = 0.0F;
		keyD.PushBack({ 80, 49, 16, 15 });
		keyD.PushBack({ 80, 147, 16, 13 });

		App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);

		App->entity_manager->CreateEntity(PLAYER);

		App->audio->PlayMusic("audio/music/loading.ogg");
	}

	// LEVEL 1
	else if (level1_active) {
		
		App->map->Load("Map2_Level1.tmx");
		
		if (App->entity_manager->player != nullptr)
			App->entity_manager->player->doorOpened = false;

		Mix_FadeOutMusic(2000);

		uint32 timeout = SDL_GetTicks() + 2000;
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
			App->audio->PlayMusic("audio/music/Music_Level1.ogg");
		}

		App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);

		App->entity_manager->AddEnemy(obstacle2.x, obstacle2.y, OBSTACLE);

		App->entity_manager->CreateEntity(DOOR, doorPosition.x, doorPosition.y);

		App->entity_manager->CreateEntity(PLAYER);
	}

	// MID-LEVEL
	else if (midlevel_active) {
		App->map->Load("Map3_MidLevel.tmx");

		if (App->entity_manager->player != nullptr)
			App->entity_manager->player->doorOpened = false;

		Mix_FadeOutMusic(2000);

		uint32 timeout = SDL_GetTicks() + 2000;
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
			App->audio->PlayMusic("audio/music/Music_MidLevel.ogg");
		}

		App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);

		App->entity_manager->AddEnemy(obstacle2.x, obstacle2.y, OBSTACLE);

		App->entity_manager->CreateEntity(DOOR, doorPosition.x, doorPosition.y);

		App->entity_manager->CreateEntity(PLAYER);
	}
	
	cameraLimitX = cameraLimit.x;
	cameraLimitY = cameraLimit.y;
		
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
		App->LoadGame();
	}

	


	if (death) {
		App->transitions->FadingToColor(Black, 0.5f);
	}

	// If player arrives to the end of a level
	if (App->entity_manager->player->touchingWin) {

		// Wins from tutorial & goes to level 1
		if (tutorial_active) {
			App->entity_manager->player->touchingWin = false;
			
			midlevel_completed = false;

			tutorial_active = false;
			level1_active = true;
			midlevel_active = false;
		}
		
		else if (level1_active) {
			// Crosses the door & goes to mid level
			if (!midlevel_completed) {
				App->entity_manager->player->touchingWin = false;

				tutorial_active = false;
				level1_active = false;
				midlevel_active = true;
			}

			// Wins the game
			else {
				App->entity_manager->player->touchingWin = false;

				tutorial_active = true;
				level1_active = false;
				midlevel_active = false;
			}
		}

		// Wins from midlevel & returns to level1
		else if (midlevel_active) {
			App->entity_manager->player->touchingWin = false;

			midlevel_completed = true;

			tutorial_active = false;
			level1_active = true;
			midlevel_active = false;
		}

		App->transitions->LinesAppearing();
	}

	// Loads tutorial
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		tutorial_active = true;
		level1_active = false;
		midlevel_active = false;

		App->transitions->LinesAppearing();
	}

	// Loads level 1
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		midlevel_completed = false;

		tutorial_active = false;
		level1_active = true;
		midlevel_active = false;
		
		App->transitions->LinesAppearing();
	}

	// Loads mid-level
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		tutorial_active = false;
		level1_active = false;
		midlevel_active = true;
		
		App->transitions->LinesAppearing();
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

	//keys animations manager
	if (tutorial_active)
	{
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			A = &keyA;
		else
			A = &press_keyA;

	/*SDL_Rect r = { 80, 49, 16, 15 };*/
	App->render->Blit(keys, 100, 100, &A->GetCurrentFrame(), SDL_FLIP_NONE, 1.0F);
	}

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

// Load Game State
bool j1Scene1::Load(pugi::xml_node& data)
{
	tutorial_active = data.child("tutorialActive").attribute("active").as_bool();
	level1_active= data.child("level1Active").attribute("active").as_bool();
	midlevel_active = data.child("midLevelActive").attribute("active").as_bool();
	midlevel_completed = data.child("midLevelCompleted").attribute("active").as_bool();

	LoadNewLevel();

	App->entity_manager->player->position.x = data.child("playerPos").attribute("x").as_int();
	App->entity_manager->player->position.y = data.child("playerPos").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Scene1::Save(pugi::xml_node& data) const
{
	pugi::xml_node playerPos = data.append_child("playerPos");

	playerPos.append_attribute("x") = App->entity_manager->player->position.x;
	playerPos.append_attribute("y") = App->entity_manager->player->position.y;

	pugi::xml_node tutorial = data.append_child("tutorialActive");
	tutorial.append_attribute("active") = tutorial_active;

	pugi::xml_node level1 = data.append_child("level1Active");
	level1.append_attribute("active") = level1_active;

	pugi::xml_node midlevel = data.append_child("midLevelActive");
	midlevel.append_attribute("active") = midlevel_active;

	pugi::xml_node midcompleted = data.append_child("midLevelCompleted");
	midcompleted.append_attribute("active") = midlevel_completed;

	return true;
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
		App->render->camera.x = cameraPositionMoving;
	}
	else {
		cameraMoving = true;
		App->render->camera.y = 0;
	}
}

void j1Scene1::LoadSceneInfo()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeScene;
	nodeScene = config.child("scene");

	cameraPositionMoving = nodeScene.child("cameraPositionMoving").attribute("x").as_int();

	if (tutorial_active) 
	{
		cameraLimit.x = nodeScene.child("cameraLimit1").attribute("x").as_int();
		cameraLimit.y = nodeScene.child("cameraLimit1").attribute("y").as_int();

		obstacle1 = { nodeScene.child("obstacle1").attribute("x").as_int() , nodeScene.child("obstacle1").attribute("y").as_int() };
	}

	else if (level1_active) 
	{
		doorPosition.x = nodeScene.child("doorPosition1").attribute("x").as_int();
		doorPosition.y = nodeScene.child("doorPosition1").attribute("y").as_int();

		if (!midlevel_completed) 
		{
			obstacle1 = { nodeScene.child("obstacle2").attribute("x").as_int() , nodeScene.child("obstacle2").attribute("y").as_int() };
			obstacle2 = { nodeScene.child("obstacle3").attribute("x").as_int() , nodeScene.child("obstacle3").attribute("y").as_int() };

			cameraLimit.x = nodeScene.child("cameraLimit2").attribute("x").as_int();
			cameraLimit.y = nodeScene.child("cameraLimit2").attribute("y").as_int();
		}
		else 
		{
			obstacle1 = { nodeScene.child("obstacle6").attribute("x").as_int() , nodeScene.child("obstacle6").attribute("y").as_int() };
			obstacle2 = { nodeScene.child("obstacle7").attribute("x").as_int() , nodeScene.child("obstacle7").attribute("y").as_int() };

			cameraLimit.x = nodeScene.child("cameraLimit3").attribute("x").as_int();
			cameraLimit.y = nodeScene.child("cameraLimit3").attribute("y").as_int();
		}
	}

	else if (midlevel_active) 
	{
		doorPosition.x = nodeScene.child("doorPosition2").attribute("x").as_int();
		doorPosition.y = nodeScene.child("doorPosition2").attribute("y").as_int();

		obstacle1 = { nodeScene.child("obstacle4").attribute("x").as_int() , nodeScene.child("obstacle4").attribute("y").as_int() };
		obstacle2 = { nodeScene.child("obstacle5").attribute("x").as_int() , nodeScene.child("obstacle5").attribute("y").as_int() };

		cameraLimit.x = nodeScene.child("cameraLimit4").attribute("x").as_int();
		cameraLimit.y = nodeScene.child("cameraLimit4").attribute("y").as_int();
	}

}