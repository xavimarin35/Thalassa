#include "j1MainMenu.h"
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
#include "j1Pathfinding.h"
#include "SDL/include/SDL_mouse.h"

#include "Brofiler/Brofiler.h"

j1MainMenu::j1MainMenu() 
{
	name.create("mainMenu");
}

j1MainMenu::~j1MainMenu() {}

bool j1MainMenu::Awake(pugi::xml_node &)
{
	return true;
}

bool j1MainMenu::Start()
{
	/*App->map->Load("MainMenu.tmx");*/

	startScene1 = false;

	return true;
}

bool j1MainMenu::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		App->scene1->scene1_active = true;
		App->scene1->tutorial_active = true;
		App->scene1->Start();
		App->entity_manager->player->Start();
		App->entity_manager->bat->Start();
	}

	return true;
}

bool j1MainMenu::PostUpdate()
{
	return true;
}

bool j1MainMenu::CleanUp()
{
	App->map->CleanUp();

	return true;
}

void j1MainMenu::ChangeScene(SCENE_TYPE next)
{
	if (!startScene1)
	{
		this->active = false;
		startGame = false;
		startCredits = false;

		CleanUp();
		if (next == SCENE_TYPE::CREDITS)
		{
			/*App->credits->active = true;
			App->credits->Start();*/
		}
		else {
			if (next == SCENE_TYPE::SCENE)
			{
				App->scene1->active = true;
				App->scene1->Start();
			}

			App->entity_manager->active = true;
		}
	}
	
	
}
