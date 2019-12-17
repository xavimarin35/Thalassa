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
	if (active)
		App->scene1->active = false;
	else
		App->scene1->active = true;

	return active;
}

bool j1MainMenu::Start()
{
	texture = App->tex->Load("gui/BG_MAINMENU.png");

	

	return true;
}

bool j1MainMenu::Update(float dt)
{
	SDL_Rect bg_rect = { 0,0,1280,640 };
	App->render->Blit(texture, 0, 0, &bg_rect);

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		active = false;
		App->scene1->active = true;
	}

	return true;
}

bool j1MainMenu::PostUpdate()
{
	return true;
}