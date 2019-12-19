#include "j1Label.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Label::j1Label(UI_ELEMENTS type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color, j1UIElement* parent) :
	j1UIElement(UI_ELEMENTS::LABEL, x, y, App->font->Print(text, temp.w, temp.h, 0, color, font), parent), font(font), text(text), color(color) {}

j1Label::~j1Label() {}

void j1Label::Draw(float scale, int x, int y, bool use_camera)
{
	App->tex->UnLoad(sprites);
	sprites = App->font->Print(text, temp.w, temp.h, 0, color, font);
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, NULL, SDL_FLIP_NONE, false, scale);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, NULL, SDL_FLIP_NONE, true, scale);
}

bool j1Label::CleanUp()
{
	LOG("Unloading label textures");
	App->tex->UnLoad(sprites);

	return true;
}