#include "j1Box.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Render.h"

j1Box::j1Box(UI_ELEMENTS type, int x, int y, SDL_Rect section, SDL_Texture* text, j1UIElement* parent, uint minimum, uint maximum) :
	j1UIElement(UI_ELEMENTS::BOX, x, y, text, parent), section(section), originalMinimum(minimum), originalMaximum(maximum), minimum(minimum), maximum(maximum) {}

j1Box::~j1Box() {}

void j1Box::Draw(float scale, int x, int y, bool use_camera)
{
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, false, scale);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, true, scale);
}

bool j1Box::CleanUp()
{
	LOG("Unloading button textures");
	App->tex->UnLoad(sprites);

	return true;
}