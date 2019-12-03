#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Window.h"

j1Hud::j1Hud() {}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	hud_text = App->tex->Load("");

	return true;
}

bool j1Hud::Update(float dt)
{
	App->render->BlitHUD(hud_text, 0, 0, &hud_rect, SDL_FLIP_NONE, false);

	return true;
}

bool j1Hud::CleanUp()
{
	App->tex->UnLoad(hud_text);

	return true;
}