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
#include "j1Bat.h"
#include "j1Platform.h"
#include "j1TransitionsManager.h"
#include "j1Collisions.h"
#include "p2Animation.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include <time.h>
#include "j1Pathfinding.h"
#include "j1MainMenu.h"
#include "SDL/include/SDL_mouse.h"
#include "j1Box.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Fonts.h"

#include "Brofiler/Brofiler.h"

#define SDL_TICKS_PASSED(A, B)  ((Sint32)((B) - (A)) <= 0)

j1Scene1::j1Scene1() : j1Module()
{
	name.create("scene1");
	srand(time(NULL));

	mouseBlinking = nullptr;

	blinkAnim.LoadAnimations("blinkAnim");

}

// Destructor
j1Scene1::~j1Scene1()
{}

// Called before render is available
bool j1Scene1::Awake()
{
	if (active)
		App->mainmenu->active = false;
	else
		App->mainmenu->active = true;

	return active;
}

// Called before the first frame
bool j1Scene1::Start()
{
	BROFILER_CATEGORY("Scene_Start", Profiler::Color::PaleGoldenRod)

	LoadSceneInfo();

	debugPath = App->tex->Load("maps/Quad_Ortho.png");
	jetPack_tex = App->tex->Load("textures/jetPack_bar.png");
	cursor_tex = App->tex->Load("textures/cursor.png");
	settings_window_text = App->tex->Load("gui/set_window.png");
	buttons_text = App->tex->Load("gui/settings_buttons.png");
	fontscene = App->font->Load("fonts/Pixeled.ttf", 10);

	cursor = { 0,0,13,13 };

	score_player = score;

	//UI
	SDL_Rect r = { 0,0,191,165 };
	settings_window = App->gui->CreateBox(&scene1Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, r, settings_window_text);
	settings_window->visible = false;

	App->gui->CreateLabel(&scene1Labels, LABEL, 57, 0, fontscene, "SETTINGS", { 0,102,153,255 }, (j1UIElement*)settings_window);

	App->gui->CreateButton(&scene1Buttons, BUTTON, 45, 70, continue_idle, continue_hover, continue_click, buttons_text, CLOSE_SETTINGS, (j1UIElement*)settings_window);
	App->gui->CreateButton(&scene1Buttons, BUTTON, 20, 100, return_idle, return_hover, return_click, buttons_text, SAVE_GAME, (j1UIElement*)settings_window);
	App->gui->CreateButton(&scene1Buttons, BUTTON, 100, 100, save_idle, save_hover, save_click, buttons_text, GO_TO_MENU, (j1UIElement*)settings_window);

	App->gui->CreateLabel(&scene1Labels, LABEL, 60, 68, fontscene, "CONTINUE", { 0,102,153,255 }, (j1UIElement*)settings_window);
	App->gui->CreateLabel(&scene1Labels, LABEL, 35, 99, fontscene, "SAVE", { 0,102,153,255 }, (j1UIElement*)settings_window);
	App->gui->CreateLabel(&scene1Labels, LABEL, 115, 99, fontscene, "BACK", { 0,102,153,255 }, (j1UIElement*)settings_window);

	// ------------------------

	// TUTORIAL
	if (tutorial_active) 
	{
		LoadTutorial();
	}

	// LEVEL 1
	else if (level1_active) 
	{
		LoadLevel1();
	}

	// MID-LEVEL
	else if (midlevel_active) 
	{
		LoadMidLevel();
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
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::Red)


	if (scene1_active)
	{
		if (!settings_window->visible)
			ShowCursor(hide_cursor);

		App->gui->UpdateButtonState(&scene1Buttons);
		App->gui->UpdateWindow(settings_window, &scene1Buttons, &scene1Labels, &scene1Boxes);

		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings)
		{
			App->pause = !App->pause;
			settings_window->visible = !settings_window->visible;

			for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next)
			{
				if (item->data->parent == settings_window)
				{
					item->data->visible = !item->data->visible;
					item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
					item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
				}
			}
			for (p2List_item<j1Label*>* item = scene1Labels.start; item != nullptr; item = item->next) {
				if (item->data->parent == settings_window) {
					item->data->visible = !item->data->visible;
					item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
					item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
				}
			}
			for (p2List_item<j1Box*>* item = scene1Boxes.start; item != nullptr; item = item->next) {
				if (item->data->parent == settings_window) {
					item->data->visible = !item->data->visible;
					item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
					item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

					item->data->minimum = item->data->originalMinimum + settings_window->position.x;
					item->data->maximum = item->data->originalMaximum + settings_window->position.x;

					item->data->distanceCalculated = false;
				}
			}

			if (!settings_window->visible) closeSettings = false;
		}

		/*App->gui->UpdateSliders(&scene1Boxes);*/
		score_player = App->entity_manager->player->score;
		current_points = std::to_string(score_player);

		for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
			switch (item->data->state)
			{
			case IDLE:
				item->data->situation = item->data->idle;
				break;

			case HOVERED:
				item->data->situation = item->data->hovered;
				break;

			case RELEASED:
				item->data->situation = item->data->idle;
				if (item->data->bfunction == GO_TO_MENU) {
					EndRun();
					App->pause = false;
					settings_window->visible = false;
				}
				else if (item->data->bfunction == CLOSE_SETTINGS) {
					closeSettings = true;
				}
				else if (item->data->bfunction == SAVE_GAME) {
					App->SaveGame("save_game.xml");
				}
				else if (item->data->bfunction == EXIT) {
					continueGame = false;
				}
				break;

			case CLICKED:
				item->data->situation = item->data->clicked;
				break;
			}
		}

		//Save & Load
		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			App->SaveGame("save_game.xml");
		}

		if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		{
			App->LoadGame();
		}

		if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		{
			if (cullingView)
				cullingView = false;
			else cullingView = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		{
			App->pause = !App->pause;
		}

		if (death)
		{
			if (!done_anim)
			{
				deathTimer.Start();
				done_anim = true;
			}

			if (deathTimer.Read() > timer)
				App->transitions->FadingToColor(Black, 0.5f);
		}

		// If player arrives to the end of a level
		if (App->entity_manager->player != nullptr && App->entity_manager->player->touchingWin)
		{
			LevelChangeLogic();
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

		// Loads level 1 (second part)
		if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		{
			midlevel_completed = true;

			tutorial_active = false;
			level1_active = true;
			midlevel_active = false;

			App->transitions->LinesAppearing();
		}

		// CAMERA FOLLOWING PLAYER
		if (App->entity_manager->player != nullptr)
		{
			CameraMovement(dt);
		}

		App->map->Draw(-App->render->camera.x);

		//keys animations manager
		if (tutorial_active)
		{
			BlitKeys();
		}

		int x, y;
		App->input->GetMousePosition(x, y);

		settings_window->position.x = (float)(App->render->camera.x * App->map->parallax_speed / App->win->scale + 85);
		settings_window->position.y = (float)(-App->render->camera.y * App->map->parallax_speed / App->win->scale + 20);

		mouse_position.x = (-App->render->camera.x * App->map->parallax_speed / App->win->scale) + x - 6;
		mouse_position.y = (-App->render->camera.y * App->map->parallax_speed / App->win->scale) + y - 6;
		App->render->Blit(cursor_tex, mouse_position.x, mouse_position.y, &cursor);

		if (player_lifes < 0)
			EndRun();
	}
	// App->win->SetTitle("Thalassa");
	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	return continueGame;
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

	App->tex->UnLoad(keys);
	App->tex->UnLoad(mouse);
	App->tex->UnLoad(debugPath);
	App->tex->UnLoad(jetPack_tex);
	App->tex->UnLoad(cursor_tex);
	App->tex->UnLoad(settings_window_text);
	App->tex->UnLoad(buttons_text);

	return true;
}

void j1Scene1::CameraMovement(float dt)
{
	if (lateralMove)
	{
		if (App->render->camera.x >= -App->entity_manager->player->position.x * App->win->GetScale() + App->win->width / 2)
		{
			App->render->camera.x -= camera_speed.x * dt;
			App->entity_manager->player->playerCanMove = false;
		}
		else
		{
			lateralMove = false;
			App->entity_manager->player->playerCanMove = true;
		}
	}
	else
		App->render->camera.x = -App->entity_manager->player->position.x * App->win->GetScale() + App->win->width / 2;

	// LIMITING X CAMERA
	if (-App->render->camera.x < 0)
		App->render->camera.x = 0;
	else if (App->render->camera.x < cameraLimitX)
		App->render->camera.x = cameraLimitX;

	if (cameraMoving)
	{
		if (App->render->camera.y >= -App->entity_manager->player->position.y * App->win->GetScale() + App->win->height / 2)
		{
			App->render->camera.y -= camera_speed.y * dt;
			App->entity_manager->player->playerCanMove = false;
		}
		else
		{
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

void j1Scene1::LoadTutorial()
{
	App->map->Load("Map1_Tutorial.tmx");

	keys = App->tex->Load("textures/keys.png");
	mouse = App->tex->Load("textures/mouseThalassa.png");

	SpawnTutorialEntities();

	App->audio->PlayMusic("audio/music/loading.ogg");
}

void j1Scene1::LoadLevel1()
{
	App->map->Load("Map2_Level1.tmx");

	if (App->entity_manager->player != nullptr)
		App->entity_manager->player->doorOpened = false;

	Mix_FadeOutMusic(2000);

	uint32 timeout = SDL_GetTicks() + 2000;
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
		App->audio->PlayMusic("audio/music/Music_Level1.ogg");
	}

	SpawnLevel1Entities();
}

void j1Scene1::LoadMidLevel()
{
	App->map->Load("Map3_MidLevel.tmx");

	if (App->entity_manager->player != nullptr)
		App->entity_manager->player->doorOpened = false;

	Mix_FadeOutMusic(2000);

	uint32 timeout = SDL_GetTicks() + 2000;
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
		App->audio->PlayMusic("audio/music/Music_MidLevel.ogg");
	}

	SpawnMidLevelEntities();
}

void j1Scene1::LevelChangeLogic()
{
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
			App->transitions->SquaresAppearing(3, Black, 4.0F);
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

	if (!tutorial_active)
		App->transitions->LinesAppearing();
}

void j1Scene1::LoadNewLevel() 
{
	CleanUp();

	score = score_player;

	scene_timer.Start();

	Start();
	App->collisions->Start();
	App->entity_manager->Start();

	if (level1_active && midlevel_completed) 
	{
		lateralMove = true;
		App->render->camera.x = cameraPositionMoving;
	}
	else 
	{
		cameraMoving = true;
		App->render->camera.y = 0;
	}
}

void j1Scene1::EndRun()
{
	CleanUp();
	scene1_active = false;
	App->mainmenu->Start();
	App->map->Start();
}

void j1Scene1::LoadSceneInfo()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeScene;
	nodeScene = config.child("scene");

	pugi::xml_node nodePositions;
	nodePositions = config.child("enemiesPosition");

	pugi::xml_node itemPositions;
	itemPositions = config.child("itemPosition");

	cameraPositionMoving = nodeScene.child("cameraPositionMoving").attribute("x").as_int();
	camera_speed = { nodeScene.child("cameraSpeed").attribute("x").as_int(), nodeScene.child("cameraSpeed").attribute("y").as_int() };
	timer = nodeScene.child("deathTimer").attribute("value").as_int();
	pos_bar = { nodeScene.child("jetPackLife").attribute("x").as_int(), nodeScene.child("jetPackLife").attribute("y").as_int() };
	pos_bar2 = { nodeScene.child("BlitJet").attribute("x").as_int(), nodeScene.child("BlitJet").attribute("y").as_int() };

	if (tutorial_active) 
	{
		cameraLimit.x = nodeScene.child("cameraLimit1").attribute("x").as_int();
		cameraLimit.y = nodeScene.child("cameraLimit1").attribute("y").as_int();

		obstacle1 = { nodeScene.child("obstacle1").attribute("x").as_int() , nodeScene.child("obstacle1").attribute("y").as_int() };

		batPos1 = { nodePositions.child("bat1").attribute("x").as_int(), nodePositions.child("bat1").attribute("y").as_int() };

		item1 = { itemPositions.child("i11").attribute("x").as_int(), itemPositions.child("i11").attribute("y").as_int() };
		item2 = { itemPositions.child("i12").attribute("x").as_int(), itemPositions.child("i12").attribute("y").as_int() };
		item3 = { itemPositions.child("i13").attribute("x").as_int(), itemPositions.child("i13").attribute("y").as_int() };
		item4 = { itemPositions.child("i14").attribute("x").as_int(), itemPositions.child("i14").attribute("y").as_int() };
		item5 = { itemPositions.child("i15").attribute("x").as_int(), itemPositions.child("i15").attribute("y").as_int() };
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

			batPos1 = { nodePositions.child("bat2").attribute("x").as_int(), nodePositions.child("bat2").attribute("y").as_int() };
			batPos2 = { nodePositions.child("bat3").attribute("x").as_int(), nodePositions.child("bat3").attribute("y").as_int() };
			batPos3 = { nodePositions.child("bat4").attribute("x").as_int(), nodePositions.child("bat4").attribute("y").as_int() };
			batPos4 = { nodePositions.child("bat10").attribute("x").as_int(), nodePositions.child("bat10").attribute("y").as_int() };

			demonPos1 = { nodePositions.child("demon1").attribute("x").as_int(), nodePositions.child("demon1").attribute("y").as_int() };

			item1 = { itemPositions.child("i21").attribute("x").as_int(), itemPositions.child("i21").attribute("y").as_int() };
			item2 = { itemPositions.child("i22").attribute("x").as_int(), itemPositions.child("i22").attribute("y").as_int() };
			item3 = { itemPositions.child("i23").attribute("x").as_int(), itemPositions.child("i23").attribute("y").as_int() };
			item4 = { itemPositions.child("i24").attribute("x").as_int(), itemPositions.child("i24").attribute("y").as_int() };
			item5 = { itemPositions.child("i25").attribute("x").as_int(), itemPositions.child("i25").attribute("y").as_int() };
			item6 = { itemPositions.child("i26").attribute("x").as_int(), itemPositions.child("i26").attribute("y").as_int() };
		}
		else 
		{
			obstacle1 = { nodeScene.child("obstacle6").attribute("x").as_int() , nodeScene.child("obstacle6").attribute("y").as_int() };
			obstacle2 = { nodeScene.child("obstacle7").attribute("x").as_int() , nodeScene.child("obstacle7").attribute("y").as_int() };

			cameraLimit.x = nodeScene.child("cameraLimit3").attribute("x").as_int();
			cameraLimit.y = nodeScene.child("cameraLimit3").attribute("y").as_int();

			batPos1 = { nodePositions.child("bat5").attribute("x").as_int(), nodePositions.child("bat5").attribute("y").as_int() };
			batPos2 = { nodePositions.child("bat6").attribute("x").as_int(), nodePositions.child("bat6").attribute("y").as_int() };

			demonPos1 = { nodePositions.child("demon2").attribute("x").as_int(), nodePositions.child("demon2").attribute("y").as_int() };
			demonPos2 = { nodePositions.child("demon3").attribute("x").as_int(), nodePositions.child("demon3").attribute("y").as_int() };

			item1 = { itemPositions.child("i27").attribute("x").as_int(), itemPositions.child("i27").attribute("y").as_int() };
			item2 = { itemPositions.child("i28").attribute("x").as_int(), itemPositions.child("i28").attribute("y").as_int() };
			item3 = { itemPositions.child("i29").attribute("x").as_int(), itemPositions.child("i29").attribute("y").as_int() };
			item4 = { itemPositions.child("i210").attribute("x").as_int(), itemPositions.child("i210").attribute("y").as_int() };
			item5 = { itemPositions.child("i211").attribute("x").as_int(), itemPositions.child("i211").attribute("y").as_int() };
			item6 = { itemPositions.child("i212").attribute("x").as_int(), itemPositions.child("i212").attribute("y").as_int() };
			item7 = { itemPositions.child("i213").attribute("x").as_int(), itemPositions.child("i213").attribute("y").as_int() };
			item8 = { itemPositions.child("i214").attribute("x").as_int(), itemPositions.child("i214").attribute("y").as_int() };
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

		batPos1 = { nodePositions.child("bat7").attribute("x").as_int(), nodePositions.child("bat7").attribute("y").as_int() };
		batPos2 = { nodePositions.child("bat8").attribute("x").as_int(), nodePositions.child("bat8").attribute("y").as_int() };
		batPos3 = { nodePositions.child("bat9").attribute("x").as_int(), nodePositions.child("bat9").attribute("y").as_int() };

		item1 = { itemPositions.child("i31").attribute("x").as_int(), itemPositions.child("i31").attribute("y").as_int() };
	}

	pugi::xml_node nodeKeys;
	nodeKeys = config.child("keys");

	A = { nodeKeys.child("A").attribute("x").as_int(), nodeKeys.child("A").attribute("y").as_int(), nodeKeys.child("A").attribute("w").as_int(), nodeKeys.child("A").attribute("h").as_int() };
	APressed = { nodeKeys.child("APressed").attribute("x").as_int(), nodeKeys.child("APressed").attribute("y").as_int(), nodeKeys.child("APressed").attribute("w").as_int(), nodeKeys.child("APressed").attribute("h").as_int() };
	W = { nodeKeys.child("W").attribute("x").as_int(), nodeKeys.child("W").attribute("y").as_int(), nodeKeys.child("W").attribute("w").as_int(), nodeKeys.child("W").attribute("h").as_int() };
	WPressed = { nodeKeys.child("WPressed").attribute("x").as_int(), nodeKeys.child("WPressed").attribute("y").as_int(), nodeKeys.child("WPressed").attribute("w").as_int(), nodeKeys.child("WPressed").attribute("h").as_int() };
	D = { nodeKeys.child("D").attribute("x").as_int(), nodeKeys.child("D").attribute("y").as_int(), nodeKeys.child("D").attribute("w").as_int(), nodeKeys.child("D").attribute("h").as_int() };
	DPressed = { nodeKeys.child("DPressed").attribute("x").as_int(), nodeKeys.child("DPressed").attribute("y").as_int(), nodeKeys.child("DPressed").attribute("w").as_int(), nodeKeys.child("DPressed").attribute("h").as_int() };
	SPACE = { nodeKeys.child("SPACE").attribute("x").as_int(), nodeKeys.child("SPACE").attribute("y").as_int(), nodeKeys.child("SPACE").attribute("w").as_int(), nodeKeys.child("SPACE").attribute("h").as_int() };
	SPACEPressed = { nodeKeys.child("SPACEPressed").attribute("x").as_int(), nodeKeys.child("SPACEPressed").attribute("y").as_int(), nodeKeys.child("SPACEPressed").attribute("w").as_int(), nodeKeys.child("SPACEPressed").attribute("h").as_int() };
	mouseIdle = { nodeKeys.child("mouseIdle").attribute("x").as_int(), nodeKeys.child("mouseIdle").attribute("y").as_int(), nodeKeys.child("mouseIdle").attribute("w").as_int(), nodeKeys.child("mouseIdle").attribute("h").as_int() };
	mouseLeft = { nodeKeys.child("mouseLeft").attribute("x").as_int(), nodeKeys.child("mouseLeft").attribute("y").as_int(), nodeKeys.child("mouseLeft").attribute("w").as_int(), nodeKeys.child("mouseLeft").attribute("h").as_int() };
	mouseRight = { nodeKeys.child("mouseRight").attribute("x").as_int(), nodeKeys.child("mouseRight").attribute("y").as_int(), nodeKeys.child("mouseRight").attribute("w").as_int(), nodeKeys.child("mouseRight").attribute("h").as_int() };

	differenceY = nodeKeys.child("differenceY").attribute("value").as_int();
	posA = { nodeKeys.child("posA").attribute("x").as_int(), nodeKeys.child("posA").attribute("y").as_int() };
	posD = { nodeKeys.child("posD").attribute("x").as_int(), nodeKeys.child("posD").attribute("y").as_int() };
	posW1 = { nodeKeys.child("posW1").attribute("x").as_int(), nodeKeys.child("posW1").attribute("y").as_int() };
	posW2 = { nodeKeys.child("posW2").attribute("x").as_int(), nodeKeys.child("posW2").attribute("y").as_int() };
	posW3 = { nodeKeys.child("posW3").attribute("x").as_int(), nodeKeys.child("posW3").attribute("y").as_int() };
	posW4 = { nodeKeys.child("posW4").attribute("x").as_int(), nodeKeys.child("posW4").attribute("y").as_int() };
	posSPACE = { nodeKeys.child("posSPACE").attribute("x").as_int(), nodeKeys.child("posSPACE").attribute("y").as_int() };
	posMIdle = { nodeKeys.child("posMIdle").attribute("x").as_int(), nodeKeys.child("posMIdle").attribute("y").as_int() };
	posMLeft = { nodeKeys.child("posMLeft").attribute("x").as_int(), nodeKeys.child("posMLeft").attribute("y").as_int() };
	/*posMRight = { nodeKeys.child("posMRight").attribute("x").as_int(), nodeKeys.child("posMRight").attribute("y").as_int() };*/

	pugi::xml_node nodeButtons;
	nodeButtons = config.child("buttons");

	continue_idle = { nodeButtons.child("continue_idle").attribute("x").as_int(),nodeButtons.child("continue_idle").attribute("y").as_int(), nodeButtons.child("continue_idle").attribute("w").as_int(), nodeButtons.child("continue_idle").attribute("h").as_int() };
	continue_hover = { nodeButtons.child("continue_hover").attribute("x").as_int(),nodeButtons.child("continue_hover").attribute("y").as_int(), nodeButtons.child("continue_hover").attribute("w").as_int(), nodeButtons.child("continue_hover").attribute("h").as_int() };
	continue_click = { nodeButtons.child("continue_click").attribute("x").as_int(),nodeButtons.child("continue_click").attribute("y").as_int(), nodeButtons.child("continue_click").attribute("w").as_int(), nodeButtons.child("continue_click").attribute("h").as_int() };

	save_idle = { nodeButtons.child("save_idle").attribute("x").as_int(),nodeButtons.child("save_idle").attribute("y").as_int(), nodeButtons.child("save_idle").attribute("w").as_int(), nodeButtons.child("save_idle").attribute("h").as_int() };
	save_hover = { nodeButtons.child("save_hover").attribute("x").as_int(),nodeButtons.child("save_hover").attribute("y").as_int(), nodeButtons.child("save_hover").attribute("w").as_int(), nodeButtons.child("save_hover").attribute("h").as_int() };
	save_click = { nodeButtons.child("save_click").attribute("x").as_int(),nodeButtons.child("save_click").attribute("y").as_int(), nodeButtons.child("save_click").attribute("w").as_int(), nodeButtons.child("save_click").attribute("h").as_int() };

	return_idle = { nodeButtons.child("return_idle").attribute("x").as_int(),nodeButtons.child("return_idle").attribute("y").as_int(), nodeButtons.child("return_idle").attribute("w").as_int(), nodeButtons.child("return_idle").attribute("h").as_int() };
	return_hover = { nodeButtons.child("return_hover").attribute("x").as_int(),nodeButtons.child("return_hover").attribute("y").as_int(), nodeButtons.child("return_hover").attribute("w").as_int(), nodeButtons.child("return_hover").attribute("h").as_int() };
	return_click = { nodeButtons.child("return_click").attribute("x").as_int(),nodeButtons.child("return_click").attribute("y").as_int(), nodeButtons.child("return_click").attribute("w").as_int(), nodeButtons.child("return_click").attribute("h").as_int() };
}

void j1Scene1::SpawnTutorialEntities()
{
	App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);
	
	App->entity_manager->AddEnemy(batPos1.x, batPos1.y, BAT_E);
	
	App->entity_manager->AddEntity(item1.x, item1.y, JETPACK_ITEM);
	App->entity_manager->AddEntity(item2.x, item2.y, JETPACK_ITEM);
	App->entity_manager->AddEntity(item3.x, item3.y, JETPACK_ITEM);
	App->entity_manager->AddEntity(item4.x, item4.y, JETPACK_ITEM);
	App->entity_manager->AddEntity(item5.x, item5.y, JETPACK_ITEM);

	SpawnTutorialCoins();

	App->entity_manager->AddPlatform(2050, 214, PLATFORM, { 40,40 }, 1, true);

	App->entity_manager->CreateEntity(BAT);
	App->entity_manager->CreateEntity(PLAYER);
}

void j1Scene1::SpawnLevel1Entities()
{
	App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);
	App->entity_manager->AddEnemy(obstacle2.x, obstacle2.y, OBSTACLE);

	SpawnLevel1Coins();

	if (!midlevel_completed)
	{
		App->entity_manager->AddEnemy(batPos1.x, batPos1.y, BAT_E);
		App->entity_manager->AddEnemy(batPos2.x, batPos2.y, BAT_E);
		App->entity_manager->AddEnemy(batPos3.x, batPos3.y, BAT_E);
		App->entity_manager->AddEnemy(batPos4.x, batPos4.y, BAT_E);

		App->entity_manager->AddEnemy(demonPos1.x, demonPos1.y, DEMON);

		App->entity_manager->AddEntity(item1.x, item1.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item2.x, item2.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item3.x, item3.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item4.x, item4.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item5.x, item5.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item6.x, item6.y, JETPACK_ITEM);

		App->entity_manager->AddPlatform(1683, 477, PLATFORM, { 360,350 }, 3);
		App->entity_manager->AddPlatform(1468, 205, PLATFORM, { 80,20 });
		App->entity_manager->AddPlatform(2246, 231, PLATFORM, { 60,60 });
		App->entity_manager->AddPlatform(1590, 147, PLATFORM, { 60,60 }, 2, true);
		App->entity_manager->AddPlatform(2044, 210, PLATFORM, { 60,145 }, 1, true);
		App->entity_manager->AddPlatform(1730, 147, PLATFORM, { 80,60 });
	}

	else
	{
		App->entity_manager->AddEnemy(batPos1.x, batPos1.y, BAT_E);
		App->entity_manager->AddEnemy(batPos2.x, batPos2.y, BAT_E);

		App->entity_manager->AddEnemy(demonPos1.x, demonPos1.y, DEMON);
		App->entity_manager->AddEnemy(demonPos2.x, demonPos2.y, DEMON);

		App->entity_manager->AddEntity(item1.x, item1.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item2.x, item2.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item3.x, item3.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item4.x, item4.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item5.x, item5.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item6.x, item6.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item7.x, item7.y, JETPACK_ITEM);
		App->entity_manager->AddEntity(item8.x, item8.y, JETPACK_ITEM);
	}

	App->entity_manager->AddEntity(doorPosition.x, doorPosition.y, DOOR);
	App->entity_manager->CreateEntity(BAT);
	App->entity_manager->CreateEntity(PLAYER);
}

void j1Scene1::SpawnMidLevelEntities()
{
	App->entity_manager->AddEnemy(obstacle1.x, obstacle1.y, OBSTACLE);
	App->entity_manager->AddEnemy(obstacle2.x, obstacle2.y, OBSTACLE);

	App->entity_manager->AddEnemy(batPos1.x, batPos1.y, BAT_E);
	App->entity_manager->AddEnemy(batPos2.x, batPos2.y, BAT_E);
	App->entity_manager->AddEnemy(batPos3.x, batPos3.y, BAT_E);

	App->entity_manager->AddEntity(item1.x, item1.y, JETPACK_ITEM);

	SpawnMidLevelCoins();

	App->entity_manager->AddPlatform(409, 145, PLATFORM, { 70,50 }, 3, true);
	App->entity_manager->AddPlatform(503, 214, PLATFORM, { 70,30 }, 3, true);
	App->entity_manager->AddPlatform(580, 153, PLATFORM, { 20,40 }, 3, true);

	App->entity_manager->AddEntity(doorPosition.x, doorPosition.y, DOOR);
	App->entity_manager->CreateEntity(BAT);
	App->entity_manager->CreateEntity(PLAYER);
}

void j1Scene1::BlitKeys()
{
	if (!App->pause)
	{
		// a
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			App->render->Blit(keys, posA.x, posA.y, &APressed);
		else
			App->render->Blit(keys, posA.x, posA.y - differenceY, &A);

		// d
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			App->render->Blit(keys, posD.x, posD.y, &DPressed);
		else
			App->render->Blit(keys, posD.x, posD.y - differenceY, &D);

		// normal jump
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			App->render->Blit(keys, posW1.x, posW1.y, &WPressed);
		else
			App->render->Blit(keys, posW1.x, posW1.y - differenceY, &W);

		// doble jump
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->entity_manager->player->doubleJump == 1)
		{
			App->render->Blit(keys, posW2.x, posW2.y, &WPressed);
			App->render->Blit(keys, posW3.x, posW3.y - differenceY, &W);
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->entity_manager->player->doubleJump == 0)
		{
			App->render->Blit(keys, posW3.x, posW3.y, &WPressed);
			App->render->Blit(keys, posW2.x, posW2.y - differenceY, &W);
		}
		else
		{
			App->render->Blit(keys, posW2.x, posW2.y - differenceY, &W);
			App->render->Blit(keys, posW3.x, posW3.y - differenceY, &W);
		}

		// w and SPACE
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			App->render->Blit(keys, posW4.x, posW4.y, &WPressed);
		else
			App->render->Blit(keys, posW4.x, posW4.y - differenceY, &W);

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
			App->render->Blit(keys, posSPACE.x, posSPACE.y, &SPACEPressed);
		else
			App->render->Blit(keys, posSPACE.x, posSPACE.y - differenceY, &SPACE);

		// Mouse
		mouseBlinking = &blinkAnim;
		SDL_Rect blinkRect = mouseBlinking->GetCurrentFrame(App->GetDT());
	
		if (App->input->GetMouseButtonDown(1) == KEY_REPEAT)
		{
			clicked = true;
			App->render->Blit(mouse, posMLeft.x, posMLeft.y, &mouseLeft);
		}
		else
		{
			if(!clicked)
				App->render->Blit(mouse, posMIdle.x, posMIdle.y, &blinkRect);
			else

				App->render->Blit(mouse, posMIdle.x, posMIdle.y, &mouseIdle);
		}
	}
}

ENTITY_TYPE j1Scene1::RandomItem()
{
	ENTITY_TYPE type;

	int value = rand() % 2;

	if (value == 0)
		type = LIFE_ITEM;
	else if (value == 1)
		type = JETPACK_ITEM;
	//else if (value == 2)
	//	type = DAMAGE_ITEM;
	//else if (value == 3)
	//	type = POINTS_ITEM;

	return type;
}

void j1Scene1::DrawPath(p2DynArray<iPoint>* path)
{
	for (int i = 0; i < path->Count(); i++)
	{
		iPoint tile = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debugPath, tile.x, tile.y);
	}
}

void j1Scene1::ReSpawnEntities()
{
	if (tutorial_active)
		SpawnTutorialEntities();

	else if (midlevel_active)
		SpawnMidLevelEntities();

	else if (level1_active)
		SpawnLevel1Entities();
}

void j1Scene1::ShowCursor(bool hide)
{
	if (hide)
		SDL_ShowCursor(SDL_DISABLE);
	else SDL_ShowCursor(SDL_ENABLE);
}

void j1Scene1::DrawJetLife()
{
	jetPackLife = { 0, pos_bar.x, (int)App->entity_manager->player->jetPackLife, pos_bar.y };

	App->render->Blit(jetPack_tex, pos_bar2.x, pos_bar2.y, &jetPackLife, SDL_FLIP_NONE, 1.0F, (0, 0), false);
}

void j1Scene1::SpawnTutorialCoins()
{
	App->entity_manager->AddEntity(553, 175, COIN);
	App->entity_manager->AddEntity(939, 162, COIN);
	App->entity_manager->AddEntity(969, 162, COIN);
	App->entity_manager->AddEntity(999, 162, COIN);
	App->entity_manager->AddEntity(1091, 57, COIN);
	App->entity_manager->AddEntity(1115, 57, COIN);
	App->entity_manager->AddEntity(1067, 57, COIN);
	App->entity_manager->AddEntity(1572, 32, COIN);
	App->entity_manager->AddEntity(1606, 52, COIN);
	App->entity_manager->AddEntity(1637, 57, COIN);
	App->entity_manager->AddEntity(1517, 255, COIN);
	App->entity_manager->AddEntity(1549, 345, COIN);
	App->entity_manager->AddEntity(1573, 345, COIN);
	App->entity_manager->AddEntity(1597, 345, COIN);
	App->entity_manager->AddEntity(1621, 345, COIN);
	App->entity_manager->AddEntity(1815, 287, COIN);
	App->entity_manager->AddEntity(1909, 167, COIN);
	App->entity_manager->AddEntity(2075, 132, COIN);
	App->entity_manager->AddEntity(2075, 162, COIN);
	App->entity_manager->AddEntity(2075, 192, COIN);
	App->entity_manager->AddEntity(2075, 222, COIN);
	App->entity_manager->AddEntity(2362, 159, COIN);
}

void j1Scene1::SpawnLevel1Coins()
{
	App->entity_manager->AddEntity(15, 475, COIN);
	App->entity_manager->AddEntity(40, 475, COIN);
	App->entity_manager->AddEntity(65, 475, COIN);
	App->entity_manager->AddEntity(80, 475, COIN);
	App->entity_manager->AddEntity(95, 475, COIN);
	App->entity_manager->AddEntity(648, 322, COIN);
	App->entity_manager->AddEntity(810, 302, COIN);
	App->entity_manager->AddEntity(659, 219, COIN);
	App->entity_manager->AddEntity(848, 219, COIN);
	App->entity_manager->AddEntity(1100, 219, COIN);
	App->entity_manager->AddEntity(1195, 208, COIN);
	App->entity_manager->AddEntity(1343, 200, COIN);
	App->entity_manager->AddEntity(1470, 186, COIN);
	App->entity_manager->AddEntity(1490, 186, COIN);
	App->entity_manager->AddEntity(1510, 186, COIN);
	App->entity_manager->AddEntity(1530, 186, COIN);
	App->entity_manager->AddEntity(1862, 201, COIN);
	App->entity_manager->AddEntity(1795, 180, COIN);
	App->entity_manager->AddEntity(2057, 326, COIN);
	App->entity_manager->AddEntity(1168, 432, COIN);
	App->entity_manager->AddEntity(1180, 400, COIN);
	App->entity_manager->AddEntity(1215, 450, COIN);
	App->entity_manager->AddEntity(1129, 466, COIN);
	App->entity_manager->AddEntity(1255, 435, COIN);
	App->entity_manager->AddEntity(1270, 446, COIN);
	App->entity_manager->AddEntity(1290, 496, COIN);
	App->entity_manager->AddEntity(2500, 196, COIN);
	App->entity_manager->AddEntity(2515, 196, COIN);
	App->entity_manager->AddEntity(2530, 196, COIN);
	App->entity_manager->AddEntity(3515, 212, COIN);
	App->entity_manager->AddEntity(3540, 212, COIN);
	App->entity_manager->AddEntity(3575, 212, COIN);
	App->entity_manager->AddEntity(4479, 70, COIN);
}

void j1Scene1::SpawnMidLevelCoins()
{
	App->entity_manager->AddEntity(52, 213, COIN);
	App->entity_manager->AddEntity(30, 213, COIN);
	App->entity_manager->AddEntity(74, 213, COIN);
	App->entity_manager->AddEntity(268, 277, COIN);
	App->entity_manager->AddEntity(290, 277, COIN);
	App->entity_manager->AddEntity(874, 333, COIN);
	App->entity_manager->AddEntity(896, 333, COIN);
	App->entity_manager->AddEntity(918, 333, COIN);
}