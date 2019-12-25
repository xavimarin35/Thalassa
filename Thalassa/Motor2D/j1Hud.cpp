#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Gui.h"

j1Hud::j1Hud() {}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	hud_text = App->tex->Load("gui/gui.png");
	lifes_text = App->tex->Load("textures/playerLife.png");

	color = { 0,0,0,0 };

	return true;
}

bool j1Hud::Update(float dt)
{
	// HUD
	SDL_Rect hud_rect = { 0,0,342,256 };
	App->render->BlitHUD(hud_text, 0, 0, &hud_rect, SDL_FLIP_NONE, false);

	// Score
	current_points = App->entity_manager->player->current_points.c_str();

	SDL_Rect temp;
	temp.x = temp.y = 0;
	temp.w = temp.h = 10;

	App->tex->UnLoad(score);
	score = App->font->Print(current_points, temp.w, temp.h, 0, color, App->gui->font1);
	App->render->BlitHUD(score, 900, 18, &temp, SDL_FLIP_NONE, false);

	// Player Lifes
	player_lifes = App->scene1->player_lifes;
	SDL_Rect life = { 0,0,10,13 };

	if (player_lifes == 3)
		App->render->BlitHUD(lifes_text, 175, 87, &life, SDL_FLIP_NONE, false);

	if (player_lifes >= 2)
		App->render->BlitHUD(lifes_text, 124, 87, &life, SDL_FLIP_NONE, false);

	if (player_lifes >= 1)
		App->render->BlitHUD(lifes_text, 72, 87, &life, SDL_FLIP_NONE, false);



	// JetPack Bar
	App->scene1->DrawJetLife();

	return true;
}

bool j1Hud::CleanUp()
{
	App->tex->UnLoad(hud_text);

	return true;
}