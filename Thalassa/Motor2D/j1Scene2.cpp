#include "j1Scene2.h"
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

j1Scene2::j1Scene2()
{
	name.create("scene2");
}

j1Scene2::~j1Scene2()
{
}

bool j1Scene2::Awake()
{
	LOG("Loading Scene2");
	bool ret = true;

	return ret;
}

bool j1Scene2::Start()
{
	//App->map->Load("Map1_Tutorial.tmx");

	//App->entity_manager->CreateEntity(CHEST);
	//App->entity_manager->CreateEntity(PLAYER);

	//App->audio->PlayMusic("audio/music/loading.ogg");
	return true;
}

bool j1Scene2::PreUpdate()
{
	return true;
}

bool j1Scene2::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	/*if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 1;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 1;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 1;*/

	if (App->entity_manager->player != nullptr) {
		App->render->camera.x = -App->entity_manager->player->position.x * App->win->GetScale() + App->win->width / 2;
		if (-App->render->camera.x < 0) App->render->camera.x = 0;
		else if (App->render->camera.x < -6290) App->render->camera.x = -6290;

		App->render->camera.y = -App->entity_manager->player->position.y * App->win->GetScale() + App->win->height / 2;
		if (App->render->camera.y > 0) App->render->camera.y = 0;
		else if (App->render->camera.y < -360) App->render->camera.y = -360;
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

bool j1Scene2::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool j1Scene2::CleanUp()
{
	LOG("Freeing scene2");

	return true;
}
